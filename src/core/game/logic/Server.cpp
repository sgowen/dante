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
#include <game/logic/RobotController.h>

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
    NG_SERVER->checkForDisconnects();
    
    InstanceManager::getServerWorld()->update();
    
    spawnCratesIfNecessary();
    
    clearClientMoves();
    
    NG_SERVER->sendOutgoingPackets();
    
#ifdef NG_STEAM
    NG_STEAM_GAME_SERVICES->update(true);
#endif
}

uint8_t Server::getPlayerIdForRobotBeingCreated()
{
    return _playerIdForRobotBeingCreated;
}

void Server::toggleObjects()
{
    _isSpawningObjects = !_isSpawningObjects;
    
    if (!_isSpawningObjects)
    {
        InstanceManager::getServerWorld()->removeAllCrates();
    }
}

void Server::toggleDisplaying()
{
    _isDisplaying = !_isDisplaying;
}

bool Server::isSpawningObjects()
{
    return _isSpawningObjects;
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
        // Let's spawn some nasty stuff for it to fight!
        
        _stateTimeNoEnemies = 0;
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

void Server::spawnGroundsIfNecessary()
{
    if (_hasSpawnedGrounds)
    {
        return;
    }
    
    for (int i = 0; i < 3; ++i)
    {
        Entity* entity = SERVER_ENTITY_MGR->createEntity('GRND');
        int xSeed = rand() % 5 + 1;
        float posX = i == 0 ? GAME_WIDTH / 2 : xSeed * GAME_WIDTH / 3;
        entity->setPosition(b2Vec2(posX, 2));
    }
    
    _hasSpawnedGrounds = true;
}

void Server::spawnRobotForPlayer(uint8_t inPlayerId, std::string inPlayerName)
{
    _playerIdForRobotBeingCreated = inPlayerId;
    Entity* entity = SERVER_ENTITY_MGR->createEntity('ROBT');
    RobotController* robot = static_cast<RobotController*>(entity->getEntityController());
    
    ClientProxy* client = NG_SERVER->getClientProxy(inPlayerId);
    robot->setAddressHash(client->getMachineAddress()->getHash());
    
    robot->setPlayerName(inPlayerName);
    float posX = GAME_WIDTH / 2;
    entity->setPosition(b2Vec2(posX, 8));
    
    // Doing this last on purpose
    robot->setPlayerId(inPlayerId);
    
    spawnGroundsIfNecessary();
}

void Server::spawnCratesIfNecessary()
{
    if (!_isSpawningObjects || InstanceManager::getServerWorld()->hasCrates())
    {
        return;
    }
    
    srand(static_cast<unsigned>(time(0)));
    
    for (uint32_t i = 0; i < 4; ++i)
    {
        Entity* entity = SERVER_ENTITY_MGR->createEntity('CRAT');
        
        int xSeed = rand() % 3 + 1;
        float posX = xSeed * GAME_WIDTH / 4;
        float posY = (rand() % static_cast<int>(GAME_HEIGHT - entity->getHeight() * 2)) + (2.0f + entity->getHeight() * 2);
        
        entity->setPosition(b2Vec2(posX, posY));
    }
}

void Server::clearClientMoves()
{
    for (uint8_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        ClientProxy* client = NG_SERVER->getClientProxy(i + 1);
        if (client)
        {
            MoveList& moveList = client->getUnprocessedMoveList();
            moveList.removeProcessedMoves(client->getUnprocessedMoveList().getLastProcessedMoveTimestamp());
        }
    }
}

Server::Server(bool isSteam) : _stateTime(0), _frameStateTime(0), _stateTimeNoEnemies(0), _playerIdForRobotBeingCreated(0), _isSpawningObjects(false), _isDisplaying(false), _hasSpawnedGrounds(false)
{
    FWInstanceManager::createServerEntityManager(InstanceManager::sHandleEntityCreatedOnServer, InstanceManager::sHandleEntityDeletedOnServer, World::sServerCreateEntity);
    
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
