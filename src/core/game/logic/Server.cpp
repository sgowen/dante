//
//  Server.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/Server.h"

#include "game/logic/World.h"
#include "framework/network/server/ClientProxy.h"
#include "framework/entity/Entity.h"

#include "framework/network/server/NetworkManagerServer.h"
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
    
    InstanceManager::getServerWorld()->updateServer();
    
    NG_SERVER->sendOutgoingPackets();
    
#ifdef NG_STEAM
    NG_STEAM_GAME_SERVICES->update(true);
#endif
}

void Server::toggleDisplaying()
{
    _isDisplaying = !_isDisplaying;
}

bool Server::isDisplaying()
{
    return _isDisplaying;
}

void Server::toggleMap()
{
    if (_map == 'Z001')
    {
        _map = 'Z002';
    }
    else
    {
        _map = 'Z001';
    }
    
    InstanceManager::getServerWorld()->loadMap(_map);
}

void Server::handleNewClient(uint8_t playerId, std::string playerName)
{
    spawnRobotForPlayer(playerId, playerName);
    
    if (NG_SERVER->getNumClientsConnected() == 1)
    {
        // This is our first client!
        
        if (_map == 0)
        {
            _map = 'Z001';
            
            InstanceManager::getServerWorld()->loadMap(_map);
        }
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
    Entity* entity = InstanceManager::getServerWorld()->getPlayerWithId(playerId);
    if (entity)
    {
        entity->requestDeletion();
    }
}

void Server::spawnRobotForPlayer(uint8_t inPlayerId, std::string inPlayerName)
{
    Entity* e = EntityMapper::getInstance()->createEntity('ROBT', 32, 16, true);
    
    PlayerController* robot = static_cast<PlayerController*>(e->getController());
    
    ClientProxy* client = NG_SERVER->getClientProxy(inPlayerId);
    robot->setAddressHash(client->getMachineAddress()->getHash());
    robot->setPlayerName(inPlayerName);
    robot->setPlayerId(inPlayerId);
    
    NG_SERVER->registerEntity(e);
}

Server::Server(bool isSteam) : _world(NULL), _stateTime(0), _frameStateTime(0), _map(0), _isDisplaying(false)
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
    
    _world = new World(WorldFlag_Server | WorldFlag_MapLoadAll);
    InstanceManager::setServerWorld(_world);
}

Server::~Server()
{
    delete _world;
    InstanceManager::setServerWorld(NULL);
    NetworkManagerServer::destroy();
    FWInstanceManager::destroyServerEntityManager();
}
