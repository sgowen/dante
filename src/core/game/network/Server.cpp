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
#include "Entity.h"
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
#include "InstanceManager.h"
#include "FWInstanceManager.h"
#include "EntityManager.h"

#include <ctime> // rand
#include <assert.h>

Server* Server::s_pInstance = nullptr;

void Server::create()
{
    assert(!s_pInstance);
    
    s_pInstance = new Server();
}

void Server::destroy()
{
    assert(s_pInstance);
    
    delete s_pInstance;
    s_pInstance = nullptr;
}

Server * Server::getInstance()
{
    return s_pInstance;
}

void Server::staticHandleNewClient(ClientProxy* inClientProxy)
{
    getInstance()->handleNewClient(inClientProxy);
}

void Server::staticHandleLostClient(ClientProxy* inClientProxy)
{
    getInstance()->handleLostClient(inClientProxy);
}

void Server::staticAddEntity(Entity* inEntity)
{
    InstanceManager::getServerWorld()->addEntity(inEntity);
}

void Server::staticRemoveEntity(Entity* inEntity)
{
    InstanceManager::getServerWorld()->removeEntity(inEntity);
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
                
                InstanceManager::getServerWorld()->update();
                
                respawnEnemiesIfNecessary();
            }
            
            NetworkManagerServer::getInstance()->sendOutgoingPackets();
        }
    }
    
    return 1;
}

void Server::update(float deltaTime)
{
    m_fStateTime += deltaTime;
    m_fFrameStateTime += deltaTime;
    
    if (m_fFrameStateTime >= FRAME_RATE)
    {
        Timing::getInstance()->updateManual(m_fStateTime, FRAME_RATE);
        
        NetworkManagerServer::getInstance()->processIncomingPackets();
        
        NetworkManagerServer::getInstance()->checkForDisconnects();
        
        while (m_fFrameStateTime >= FRAME_RATE)
        {
            m_fFrameStateTime -= FRAME_RATE;
            
            InstanceManager::getServerWorld()->update();
            
            respawnEnemiesIfNecessary();
        }
        
        NetworkManagerServer::getInstance()->sendOutgoingPackets();
    }
}

bool Server::isInitialized()
{
    return m_isInitialized;
}

void Server::handleNewClient(ClientProxy* inClientProxy)
{
    int playerId = inClientProxy->getPlayerId();
    
    spawnRobotForPlayer(playerId);
    
    if (NetworkManagerServer::getInstance()->getNumClientsConnected() == 1)
    {
        // This is our first client!
        // Let's spawn some nasty stuff for it to fight!
        
        m_fStateTimeNoEnemies = 0;
    }
}

void Server::handleLostClient(ClientProxy* inClientProxy)
{
    int playerId = inClientProxy->getPlayerId();
    
    Robot* robot = InstanceManager::getServerWorld()->getRobotWithPlayerId(playerId);
    if (robot)
    {
        robot->requestDeletion();
    }
    
    delete inClientProxy;
}

void Server::spawnRobotForPlayer(int inPlayerId)
{
    Robot* robot = static_cast<Robot*>(InstanceManager::getServerEntityRegistry()->createEntity(NETWORK_TYPE_Robot));
    robot->setPlayerId(inPlayerId);
    robot->setPosition(Vector2(8.f - static_cast<float>(inPlayerId), 7.0f));
    
    static Color Red(1.0f, 0.0f, 0.0f, 1);
    static Color Green(0.0f, 1.0f, 0.0f, 1);
    static Color Yellow(1.0f, 1.0f, 0.0f, 1);
    
    switch (inPlayerId)
    {
        case 2:
            robot->setColor(Red);
            break;
        case 3:
            robot->setColor(Green);
            break;
        case 4:
            robot->setColor(Yellow);
            break;
        default:
            break;
    }
}

void Server::respawnEnemiesIfNecessary()
{
    if (!InstanceManager::getServerWorld()->hasSpacePirates())
    {
        m_fStateTimeNoEnemies += Timing::getInstance()->getDeltaTime();
        if (m_fStateTimeNoEnemies > 10)
        {
            srand(static_cast<unsigned>(time(0)));
            
            m_fStateTimeNoEnemies = 0;
            
            int numSpacePirates = rand() % 12 + 1;
            
            for (int i = 0; i < numSpacePirates; ++i)
            {
                SpacePirate* spacePirate = static_cast<SpacePirate*>(InstanceManager::getServerEntityRegistry()->createEntity(NETWORK_TYPE_SpacePirate));
                float speed = (rand() % 100) * 0.05f + 1.0f;
                spacePirate->init(CAM_WIDTH - static_cast<float>(i), 7.0f, speed);
                
                static Color Red(1.0f, 0.0f, 0.0f, 1);
                static Color Blue(0.0f, 0.0f, 1.0f, 1);
                static Color Green(0.0f, 1.0f, 0.0f, 1);
                
                if (spacePirate->getSpeed() > 4)
                {
                    spacePirate->setColor(Red);
                }
                else if (spacePirate->getSpeed() > 3)
                {
                    spacePirate->setColor(Green);
                }
                else if (spacePirate->getSpeed() > 2)
                {
                    spacePirate->setColor(Blue);
                }
            }
        }
    }
}

Server::Server() : m_fStateTime(0), m_fFrameStateTime(0), m_fStateTimeNoEnemies(0), m_isInitialized(false)
{
    FWInstanceManager::getServerEntityManager()->init(Server::staticRemoveEntity);
    
    InstanceManager::getServerEntityRegistry()->init(Server::staticAddEntity);
    
    InstanceManager::getServerEntityRegistry()->registerCreationFunction(NETWORK_TYPE_Robot, Robot::staticCreateServer);
    InstanceManager::getServerEntityRegistry()->registerCreationFunction(NETWORK_TYPE_Projectile, Projectile::staticCreateServer);
    InstanceManager::getServerEntityRegistry()->registerCreationFunction(NETWORK_TYPE_SpacePirate, SpacePirate::staticCreateServer);
    
    m_isInitialized = NetworkManagerServer::getInstance()->init(9999, Server::staticHandleNewClient, Server::staticHandleLostClient, PooledObjectsManager::borrowInputState);
}

Server::~Server()
{
    // Empty
}
