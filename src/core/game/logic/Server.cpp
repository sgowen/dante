//
//  Server.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/logic/Server.h>

#include <game/logic/World.h>
#include <framework/network/server/ClientProxy.h>
#include <framework/entity/Entity.h>
#include <framework/util/Timing.h>
#include <framework/entity/EntityIDManager.h>

#include <framework/network/server/NetworkManagerServer.h>
#include <framework/util/Constants.h>
#include <framework/network/portable/SocketUtil.h>
#include <framework/entity/EntityManager.h>
#include <framework/network/server/SocketServerHelper.h>
#include <framework/network/portable/MachineAddress.h>
#include <framework/util/Constants.h>
#include <game/entity/PlayerController.h>
#include <framework/entity/EntityMapper.h>
#include <framework/entity/EntityLayoutMapper.h>
#include <framework/util/InstanceManager.h>
#include <framework/util/Config.h>

#ifdef NG_STEAM
#include <framework/network/steam/NGSteamServerHelper.h>
#include <framework/network/steam/NGSteamGameServices.h>
#endif

#include <Box2D/Box2D.h>

#include <ctime> // rand
#include <assert.h>

#define SERVER_CALLBACKS Server::sHandleDynamicEntityCreatedOnServer, Server::sHandleDynamicEntityDeletedOnServer, Server::sHandleNewClient, Server::sHandleLostClient, Server::sHandleInputStateCreation, Server::sHandleInputStateRelease

Server* Server::s_instance = NULL;

void Server::create(uint32_t flags, void* data)
{
    assert(!s_instance);
    
    s_instance = new Server(flags, data);
}

Server * Server::getInstance()
{
    return s_instance;
}

void Server::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = NULL;
}

void Server::sHandleDynamicEntityCreatedOnServer(Entity* entity)
{
    // Empty
}

void Server::sHandleDynamicEntityDeletedOnServer(Entity* entity)
{
    EntityController* controller = entity->getController();
    if (controller->getRTTI().derivesFrom(PlayerController::rtti))
    {
        PlayerController* robot = static_cast<PlayerController*>(controller);
        assert(robot);
        
        // This is my shoddy respawn implementation
        getInstance()->spawnRobotForPlayer(robot->getPlayerId(), robot->getPlayerName());
    }
}

void Server::sHandleNewClient(uint8_t playerId, std::string playerName)
{
    getInstance()->handleNewClient(playerId, playerName);
}

void Server::sHandleLostClient(ClientProxy* inClientProxy, uint8_t index)
{
    getInstance()->handleLostClient(inClientProxy, index);
}

InputState* Server::sHandleInputStateCreation()
{
    return getInstance()->handleInputStateCreation();
}

void Server::sHandleInputStateRelease(InputState* inputState)
{
    getInstance()->handleInputStateRelease(inputState);
}

void Server::update()
{
    _timing->onFrame();
    
    NG_SERVER->processIncomingPackets();
    
    int moveCount = NG_SERVER->getMoveCount();
    if (moveCount > 0)
    {
        for (int i = 0; i < moveCount; ++i)
        {
            for (Entity* entity : _world->getPlayers())
            {
                PlayerController* robot = static_cast<PlayerController*>(entity->getController());
                
                ClientProxy* client = NG_SERVER->getClientProxy(robot->getPlayerId());
                if (client)
                {
                    MoveList& moveList = client->getUnprocessedMoveList();
                    Move* move = moveList.getMoveAtIndex(i);
                    if (move)
                    {
                        robot->processInput(move->getInputState());
                        
                        moveList.markMoveAsProcessed(move);
                        
                        client->setLastMoveTimestampDirty(true);
                    }
                }
            }
            
            _world->stepPhysics();
            _world->updateAndRemoveEntitiesAsNeeded(_world->getDynamicEntities());
            _world->handleDirtyStates(_world->getDynamicEntities());
        }
        
        for (uint8_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
        {
            uint8_t playerId = i + 1;
            ClientProxy* client = NG_SERVER->getClientProxy(playerId);
            if (client)
            {
                MoveList& moveList = client->getUnprocessedMoveList();
                moveList.removeProcessedMoves(client->getUnprocessedMoveList().getLastProcessedMoveTimestamp(), Server::sHandleInputStateRelease);
            }
        }
    }
    
    NG_SERVER->sendOutgoingPackets();
    
#ifdef NG_STEAM
    NG_STEAM_GAME_SERVICES->update(true);
#endif
}

void Server::toggleMap()
{
    if (_flags & ServerFlag_TestSession)
    {
        return;
    }
    
    if (_map == 'Z001')
    {
        _map = 'Z002';
    }
    else
    {
        _map = 'Z001';
    }
    
    loadMap();
}

uint32_t Server::getFlags()
{
    return _flags;
}

World* Server::getWorld()
{
    return _world;
}

void Server::handleNewClient(uint8_t playerId, std::string playerName)
{
    if (NG_SERVER->getNumClientsConnected() == 1)
    {
        // This is our first client!
        
        if (_flags & ServerFlag_TestSession)
        {
            uint32_t* map = static_cast<uint32_t*>(_data);
            _map = *map;
            loadMap();
        }
        else
        {
            if (_map == 0)
            {
                _map = 'Z001';
                
                loadMap();
            }
        }
    }
    
    spawnRobotForPlayer(playerId, playerName);
    
    _playerIds.push_back(playerId);
    _playerNames.push_back(playerName);
}

void Server::handleLostClient(ClientProxy* inClientProxy, uint8_t index)
{
    if (index >= 1)
    {
        uint8_t playerId = inClientProxy->getPlayerId(index);
        
        deleteRobotWithPlayerId(playerId);
    }
    else
    {
        for (int i = 0; i < inClientProxy->getNumPlayers(); ++i)
        {
            uint8_t playerId = inClientProxy->getPlayerId(i);
            
            deleteRobotWithPlayerId(playerId);
        }
    }
}

InputState* Server::handleInputStateCreation()
{
    InputState* ret = _inputStates.obtain();
    ret->reset();
    
    return ret;
}

void Server::handleInputStateRelease(InputState* inputState)
{
    GameInputState* mainInputState = static_cast<GameInputState*>(inputState);
    _inputStates.free(mainInputState);
}

void Server::deleteRobotWithPlayerId(uint8_t playerId)
{
    Entity* player = NULL;
    for (Entity* e : _world->getPlayers())
    {
        PlayerController* robot = static_cast<PlayerController*>(e->getController());
        if (robot->getPlayerId() == playerId)
        {
            player = e;
            break;
        }
    }
    
    if (player)
    {
        for (std::vector<uint8_t>::iterator i = _playerIds.begin(); i != _playerIds.end(); )
        {
            if (playerId == (*i))
            {
                i = _playerIds.erase(i);
            }
            else
            {
                ++i;
            }
        }
        
        PlayerController* robot = static_cast<PlayerController*>(player->getController());
        std::string& playerName = robot->getPlayerName();
        for (std::vector<std::string>::iterator i = _playerNames.begin(); i != _playerNames.end(); )
        {
            if (playerName == (*i))
            {
                i = _playerNames.erase(i);
            }
            else
            {
                ++i;
            }
        }
        
        player->requestDeletion();
    }
}

void Server::spawnRobotForPlayer(uint8_t inPlayerId, std::string inPlayerName)
{
    ClientProxy* client = NG_SERVER->getClientProxy(inPlayerId);
    if (!client)
    {
        return;
    }
    
    float spawnX = 32;
    float spawnY = 16;
    for (Entity* e : _world->getLayers())
    {
        if (e->getEntityDef().type == 'Z1S1')
        {
            spawnX = e->getPosition().x;
            spawnY = e->getPosition().y - e->getHeight() / 2 + 9;
            break;
        }
    }
    
    EntityInstanceDef eid(_entityIDManager->getNextDynamicEntityID(), 'ROBT', spawnX, spawnY);
    Entity* e = EntityMapper::getInstance()->createEntity(&eid, true);
    PlayerController* robot = static_cast<PlayerController*>(e->getController());
    robot->setAddressHash(client->getMachineAddress()->getHash());
    robot->setPlayerName(inPlayerName);
    robot->setPlayerId(inPlayerId);
    
    _world->addEntity(e);
}

void Server::loadMap()
{
    std::vector<uint8_t> playerIds = _playerIds;
    std::vector<std::string> playerNames = _playerNames;
    
    _world->loadMap(_map);
    
    NG_SERVER->setMap(_map);
    
    _playerIds.clear();
    _playerNames.clear();
    
    for (int i = 0; i < playerIds.size(); ++i)
    {
        handleNewClient(playerIds[i], playerNames[i]);
    }
}

Server::Server(uint32_t flags, void* data) :
_flags(flags),
_data(data),
_timing(static_cast<Timing*>(INSTANCE_MANAGER->getInstance(INSTANCE_TIME_SERVER))),
_entityIDManager(static_cast<EntityIDManager*>(INSTANCE_MANAGER->getInstance(INSTANCE_ENTITY_ID_MANAGER_SERVER))),
_world(new World(WorldFlag_Server)),
_map(0)
{
    _timing->reset();
    
    if (_flags & ServerFlag_Steam)
    {
#ifdef NG_STEAM
        NetworkManagerServer::create(new NGSteamServerHelper(FW_CFG->_steamGameDir, FW_CFG->_versionName, FW_CFG->_steamProductName, FW_CFG->_steamProductDescription, FW_CFG->_serverPort, NG_SERVER_CALLBACKS), SERVER_CALLBACKS);
#endif
    }
    else
    {
        NetworkManagerServer::create(new SocketServerHelper(FW_CFG->_serverPort, NG_SERVER_CALLBACKS), SERVER_CALLBACKS);
    }
    
    assert(NG_SERVER);
}

Server::~Server()
{
    delete _world;
    NetworkManagerServer::destroy();
}
