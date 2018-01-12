//
//  Server.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/Server.h"

#include "framework/network/server/ClientProxy.h"
#include "framework/entity/Entity.h"

#include "framework/network/server/NetworkManagerServer.h"
#include "game/logic/World.h"
#include "framework/util/Timing.h"
#include "framework/util/FrameworkConstants.h"
#include "framework/network/portable/SocketUtil.h"
#include "game/logic/PooledObjectsManager.h"
#include "game/logic/InstanceManager.h"
#include "framework/network/portable/FWInstanceManager.h"
#include "framework/entity/EntityManager.h"
#include "framework/network/server/SocketServerHelper.h"
#include "framework/network/portable/MachineAddress.h"
#include <game/logic/GameConstants.h>
#include <game/logic/PlayerController.h>
#include "framework/entity/EntityMapper.h"

#ifdef NG_STEAM
#include <framework/network/steam/NGSteamServerHelper.h>
#include <framework/network/steam/NGSteamGameServices.h>
#endif

#include "Box2D/Box2D.h"

#include <ctime> // rand
#include <assert.h>

#define SERVER_CALLBACKS Server::sHandleNewClient, Server::sHandleLostClient, PooledObjectsManager::borrowInputState

Server* Server::s_instance = NULL;

void Server::create(bool isSteam)
{
    assert(!s_instance);
    
    s_instance = new Server(isSteam);
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

void Server::sHandleNewClient(uint8_t playerId, std::string playerName)
{
    getInstance()->handleNewClient(playerId, playerName);
}

void Server::sHandleLostClient(ClientProxy* inClientProxy, uint8_t index)
{
    getInstance()->handleLostClient(inClientProxy, index);
}

void Server::update()
{
    _stateTime += FRAME_RATE;
    
    Timing::getInstance()->updateManual(_stateTime, FRAME_RATE);
    
    NG_SERVER->processIncomingPackets();
    
    InstanceManager::getServerWorld()->update();
    
    NG_SERVER->sendOutgoingPackets();
    
#ifdef NG_STEAM
    NG_STEAM_GAME_SERVICES->update(true);
#endif
}

uint8_t Server::getPlayerIdForRobotBeingCreated()
{
    return _playerIdForRobotBeingCreated;
}

void Server::toggleDisplaying()
{
    _isDisplaying = !_isDisplaying;
}

bool Server::isDisplaying()
{
    return _isDisplaying;
}

void Server::handleNewClient(uint8_t playerId, std::string playerName)
{
    spawnRobotForPlayer(playerId, playerName);
    
    if (NG_SERVER->getNumClientsConnected() == 1)
    {
        // This is our first client!
        
        spawnObjectsIfNecessary();
    }
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

void Server::deleteRobotWithPlayerId(uint8_t playerId)
{
    Entity* entity = InstanceManager::getServerWorld()->getRobotWithPlayerId(playerId);
    if (entity)
    {
        entity->requestDeletion();
    }
}

void Server::spawnRobotForPlayer(uint8_t inPlayerId, std::string inPlayerName)
{
    _playerIdForRobotBeingCreated = inPlayerId;
    Entity* e = createAndRegisterEntity('ROBT', 2, 8);
    
    PlayerController* robot = static_cast<PlayerController*>(e->getController());
    
    ClientProxy* client = NG_SERVER->getClientProxy(inPlayerId);
    robot->setAddressHash(client->getMachineAddress()->getHash());
    robot->setPlayerName(inPlayerName);
    robot->setPlayerId(inPlayerId);
}

void Server::spawnObjectsIfNecessary()
{
    if (_hasSpawnedObjects)
    {
        return;
    }
    
    InstanceManager::getServerWorld()->loadMap();
    
    _hasSpawnedObjects = true;
    
    srand(static_cast<unsigned>(time(0)));
    
    /// We really shouldn't have any more than 32 dynamic
    /// objects in addition to the players in a given zone
    /// so that we can fit our entire state inside
    /// of a single packet, which needs to be a max of
    /// 1200 bytes (any larger will be dropped by some routers).
    for (uint32_t i = 0; i < 4; ++i)
    {
        int xSeed = rand() % 2 + 1;
        float posX = xSeed * 8;
        float posY = (rand() % static_cast<int>(GAME_HEIGHT - 8)) + 20;
        
        createAndRegisterEntity('CRAT', posX, posY);
    }
}

Entity* Server::createAndRegisterEntity(uint32_t inFourCCName, int x, int y)
{
    Entity* ret = EntityMapper::getInstance()->createEntity(inFourCCName, x, y, true);
    
    NG_SERVER->registerEntity(ret);
    
    return ret;
}

Server::Server(bool isSteam) : _stateTime(0), _frameStateTime(0), _playerIdForRobotBeingCreated(0), _isDisplaying(false), _hasSpawnedObjects(false)
{
    FWInstanceManager::createServerEntityManager(InstanceManager::sHandleEntityCreatedOnServer, InstanceManager::sHandleEntityDeletedOnServer);
    
    if (isSteam)
    {
#ifdef NG_STEAM
        NetworkManagerServer::create(new NGSteamServerHelper(STEAM_GAME_DIR, VERSION_STRING, STEAM_PRODUCT_NAME, STEAM_PRODUCT_DESC, SERVER_PORT, NG_SERVER_CALLBACKS), SERVER_CALLBACKS);
#endif
    }
    else
    {
        NetworkManagerServer::create(new SocketServerHelper(SERVER_PORT, NG_SERVER_CALLBACKS), SERVER_CALLBACKS);
    }
    assert(NG_SERVER);
    
    InstanceManager::createServerWorld();
}

Server::~Server()
{
    InstanceManager::destroyServerWorld();
    NetworkManagerServer::destroy();
    FWInstanceManager::destroyServerEntityManager();
}
