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
        
        spawnGroundsIfNecessary();
        spawnCratesIfNecessary();
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
    Entity* entity = SERVER_ENTITY_MGR->createEntity('ROBT');
    PlayerController* robot = static_cast<PlayerController*>(entity->getEntityController());
    
    ClientProxy* client = NG_SERVER->getClientProxy(inPlayerId);
    robot->setAddressHash(client->getMachineAddress()->getHash());
    
    robot->setPlayerName(inPlayerName);
    float posX = 5;
    entity->setPosition(b2Vec2(posX, 8));
    
    // Doing this last on purpose
    robot->setPlayerId(inPlayerId);
}

void Server::spawnGroundsIfNecessary()
{
    if (_hasSpawnedGrounds)
    {
        return;
    }
    
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('G001');
        e->setPosition(b2Vec2(0, 2));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('G002');
        e->setPosition(b2Vec2(2, 2));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('G003');
        e->setPosition(b2Vec2(4, 2));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('G004');
        e->setPosition(b2Vec2(6, 2));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('G005');
        e->setPosition(b2Vec2(8, 2));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('G006');
        e->setPosition(b2Vec2(10, 2));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('G007');
        e->setPosition(b2Vec2(12, 2));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('G008');
        e->setPosition(b2Vec2(14, 2));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('G009');
        e->setPosition(b2Vec2(16, 2));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('G010');
        e->setPosition(b2Vec2(18, 2));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('G011');
        e->setPosition(b2Vec2(20, 2));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('G012');
        e->setPosition(b2Vec2(22, 2));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('G013');
        e->setPosition(b2Vec2(24, 2));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('G014');
        e->setPosition(b2Vec2(26, 2));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('T001');
        e->setPosition(b2Vec2(7, 8));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('T002');
        e->setPosition(b2Vec2(14, 10));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('T003');
        e->setPosition(b2Vec2(13, 16));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('T003');
        e->setPosition(b2Vec2(15, 16));
    }
    {
        Entity* e = SERVER_ENTITY_MGR->createEntity('T003');
        e->setPosition(b2Vec2(17, 16));
    }
    
    _hasSpawnedGrounds = true;
}

void Server::spawnCratesIfNecessary()
{
    if (_hasSpawnedCrates)
    {
        return;
    }
    
    srand(static_cast<unsigned>(time(0)));
    
    /// We really shouldn't have any more than 32 dynamic
    /// objects in addition to the players in a given zone
    /// so that we can fit our entire state inside
    /// of a single packet, which needs to be a max of
    /// 1200 bytes (any larger will be dropped by some routers).
    for (uint32_t i = 0; i < 8; ++i)
    {
        Entity* entity = SERVER_ENTITY_MGR->createEntity('CRAT');
        
        int xSeed = rand() % 2 + 1;
        float posX = xSeed * 7;
        float posY = (rand() % static_cast<int>(GAME_HEIGHT - entity->getHeight() * 2)) + (2.0f + entity->getHeight() * 2) + 10;
        
        entity->setPosition(b2Vec2(posX, posY));
    }
    
    _hasSpawnedCrates = true;
}

Server::Server(bool isSteam) : _stateTime(0), _frameStateTime(0), _playerIdForRobotBeingCreated(0), _isDisplaying(false), _hasSpawnedGrounds(false), _hasSpawnedCrates(false)
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
