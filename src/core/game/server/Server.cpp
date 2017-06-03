//
//  Server.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Server.h"

#include "ClientProxy.h"

#include "Robot.h"
#include "NetworkManagerServer.h"
#include "EntityRegistry.h"
#include "World.h"
#include "Timing.h"
#include "FrameworkConstants.h"
#include "SocketUtil.h"
#include "PooledObjectsManager.h"

Server* Server::getInstance()
{
    static Server instance = Server();
    return &instance;
}

void Server::staticHandleNewClient(ClientProxy* inClientProxy)
{
    getInstance()->handleNewClient(inClientProxy);
}

void Server::staticHandleLostClient(ClientProxy* inClientProxy)
{
    getInstance()->handleLostClient(inClientProxy);
}

int Server::run()
{
    while (true)
    {
        Timing::getInstance()->update();
        
        m_fFrameStateTime += Timing::getInstance()->getDeltaTime();
        
        if (m_fFrameStateTime >= FRAME_RATE)
        {
            NetworkManagerServer::getInstance()->processIncomingPackets();
            
            NetworkManagerServer::getInstance()->checkForDisconnects();
            
            NetworkManagerServer::getInstance()->respawnCats();
            
            while (m_fFrameStateTime >= FRAME_RATE)
            {
                m_fFrameStateTime -= FRAME_RATE;
                
                World::sInstance->update();
            }
            
            NetworkManagerServer::getInstance()->sendOutgoingPackets();
        }
    }
    
    return 1;
}

void Server::handleNewClient(ClientProxy* inClientProxy)
{
    int playerId = inClientProxy->getPlayerId();
    
    spawnCatForPlayer(playerId);
}

void Server::handleLostClient(ClientProxy* inClientProxy)
{
    //kill client's cat
    //remove client from scoreboard
    int playerId = inClientProxy->getPlayerId();
    
    Robot* cat = getCatForPlayer(playerId);
    if (cat)
    {
        cat->requestDeletion();
    }
}

void Server::spawnCatForPlayer(int inPlayerId)
{
    Robot* cat = static_cast<Robot*>(EntityRegistry::getInstance()->createEntity(NETWORK_TYPE_Robot));
    cat->setPlayerId(inPlayerId);
    //gotta pick a better spawn location than this...
    cat->setPosition(Vector2(8.f - static_cast<float>(inPlayerId), 7.0f));
    
    static Color White(1.0f, 1.0f, 1.0f, 1);
    static Color Red(1.0f, 0.0f, 0.0f, 1);
    static Color Blue(0.0f, 0.0f, 1.0f, 1);
    static Color Green(0.0f, 1.0f, 0.0f, 1);
    
    switch (inPlayerId)
    {
        case 1:
            cat->setColor(White);
            break;
        case 2:
            cat->setColor(Red);
            break;
        case 3:
            cat->setColor(Green);
            break;
        case 4:
            cat->setColor(Blue);
            break;
        default:
            break;
    }
}

bool Server::isInitialized()
{
    return m_isInitialized;
}

Robot* Server::getCatForPlayer(int inPlayerId)
{
    //run through the objects till we find the cat...
    //it would be nice if we kept a pointer to the cat on the clientproxy
    //but then we'd have to clean it up when the cat died, etc.
    //this will work for now until it's a perf issue
    const auto& gameObjects = World::sInstance->GetRobots();
    int len = static_cast<int>(gameObjects.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        Entity* go = gameObjects[i];
        Robot* cat = go->getRTTI().derivesFrom(Robot::rtti) ? static_cast<Robot*>(go) : nullptr;
        if (cat && cat->getPlayerId() == inPlayerId)
        {
            return static_cast<Robot*>(go);
        }
    }
    
    return nullptr;
}

Server::Server() : m_fFrameStateTime(0), m_isInitialized(false)
{
    m_isInitialized = SOCKET_UTIL->init() && NetworkManagerServer::getInstance()->init(9997, World::removeEntityIfPossible, Server::staticHandleNewClient, Server::staticHandleLostClient, PooledObjectsManager::borrowInputState);
    
    World::staticInit();
    
    EntityRegistry::getInstance()->init(World::addEntityIfPossible);
    
    EntityRegistry::getInstance()->registerCreationFunction(NETWORK_TYPE_Robot, Robot::create);
}

Server::~Server()
{
    // Empty
}
