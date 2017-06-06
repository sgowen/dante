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
#include "Projectile.h"
#include "SpacePirate.h"
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
            Timing::getInstance()->setDeltaTime(FRAME_RATE);
            
            NetworkManagerServer::getInstance()->processIncomingPackets();
            
            NetworkManagerServer::getInstance()->checkForDisconnects();
            
            while (m_fFrameStateTime >= FRAME_RATE)
            {
                m_fFrameStateTime -= FRAME_RATE;
                
                World::getInstance()->update();
                
                respawnEnemiesIfNecessary();
            }
            
            NetworkManagerServer::getInstance()->sendOutgoingPackets();
        }
    }
    
    return 1;
}

void Server::handleNewClient(ClientProxy* inClientProxy)
{
    int playerId = inClientProxy->getPlayerId();
    
    spawnRobotForPlayer(playerId);
    
    if (NetworkManagerServer::getInstance()->getNumClientsConnected() == 1)
    {
        // This is our first client!
        // Let's spawn some nasty stuff for it to fight!
        
        respawnEnemiesIfNecessary();
    }
}

void Server::handleLostClient(ClientProxy* inClientProxy)
{
    int playerId = inClientProxy->getPlayerId();
    
    Robot* robot = World::staticGetRobotWithPlayerId(playerId);
    if (robot)
    {
        robot->requestDeletion();
    }
    
    delete inClientProxy;
}

void Server::spawnRobotForPlayer(int inPlayerId)
{
    Robot* robot = static_cast<Robot*>(EntityRegistry::getInstance()->createEntity(NETWORK_TYPE_Robot));
    robot->setPlayerId(inPlayerId);
    robot->setPosition(Vector2(8.f - static_cast<float>(inPlayerId), 7.0f));
    
    static Color White(1.0f, 1.0f, 1.0f, 1);
    static Color Red(1.0f, 0.0f, 0.0f, 1);
    static Color Blue(0.0f, 0.0f, 1.0f, 1);
    static Color Green(0.0f, 1.0f, 0.0f, 1);
    
    switch (inPlayerId)
    {
        case 1:
            robot->setColor(White);
            break;
        case 2:
            robot->setColor(Red);
            break;
        case 3:
            robot->setColor(Green);
            break;
        case 4:
            robot->setColor(Blue);
            break;
        default:
            break;
    }
}

bool Server::isInitialized()
{
    return m_isInitialized;
}

void Server::respawnEnemiesIfNecessary()
{
    if (!World::staticHasSpacePirates())
    {
        for (int i = 0; i < 4; ++i)
        {
            SpacePirate* spacePirate = static_cast<SpacePirate*>(EntityRegistry::getInstance()->createEntity(NETWORK_TYPE_SpacePirate));
            spacePirate->init(CAM_WIDTH - static_cast<float>(i + 2), 7.0f, 4.0f - i);
            
            static Color White(1.0f, 1.0f, 1.0f, 1);
            static Color Red(1.0f, 0.0f, 0.0f, 1);
            static Color Blue(0.0f, 0.0f, 1.0f, 1);
            static Color Green(0.0f, 1.0f, 0.0f, 1);
            
            switch ((i + 1))
            {
                case 1:
                    spacePirate->setColor(White);
                    break;
                case 2:
                    spacePirate->setColor(Red);
                    break;
                case 3:
                    spacePirate->setColor(Green);
                    break;
                case 4:
                    spacePirate->setColor(Blue);
                    break;
                default:
                    break;
            }
        }
    }
}

Server::Server() : m_fFrameStateTime(0), m_isInitialized(false)
{
    m_isInitialized = SOCKET_UTIL->init() && NetworkManagerServer::getInstance()->init(9999, World::staticRemoveEntity, Server::staticHandleNewClient, Server::staticHandleLostClient, PooledObjectsManager::borrowInputState);
    
    EntityRegistry::getInstance()->init(World::staticAddEntity);
    
    EntityRegistry::getInstance()->registerCreationFunction(NETWORK_TYPE_Robot, Robot::create);
    EntityRegistry::getInstance()->registerCreationFunction(NETWORK_TYPE_Projectile, Projectile::create);
    EntityRegistry::getInstance()->registerCreationFunction(NETWORK_TYPE_SpacePirate, SpacePirate::create);
}

Server::~Server()
{
    // Empty
}
