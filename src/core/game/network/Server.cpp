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
#include "SocketServerHelper.h"
#include "NGSteamServerHelper.h"

#include <ctime> // rand
#include <assert.h>

#define SERVER_CALLBACKS Server::staticHandleNewClient, Server::staticHandleLostClient, PooledObjectsManager::borrowInputState

Server* Server::s_instance = nullptr;

void Server::create(bool isSteam)
{
    assert(!s_instance);
    
    s_instance = new Server(isSteam);
}

void Server::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = nullptr;
}

Server * Server::getInstance()
{
    return s_instance;
}

void Server::staticHandleNewClient(ClientProxy* inClientProxy)
{
    getInstance()->handleNewClient(inClientProxy);
}

void Server::staticHandleLostClient(ClientProxy* inClientProxy)
{
    getInstance()->handleLostClient(inClientProxy);
}

void Server::update(float deltaTime)
{
    m_fStateTime += deltaTime;
    m_fFrameStateTime += deltaTime;
    
    if (m_fFrameStateTime >= FRAME_RATE)
    {
        Timing::getInstance()->updateManual(m_fStateTime, FRAME_RATE);
        
        NG_SERVER->processIncomingPackets();
        
        NG_SERVER->checkForDisconnects();
        
        while (m_fFrameStateTime >= FRAME_RATE)
        {
            m_fFrameStateTime -= FRAME_RATE;
            
            InstanceManager::getServerWorld()->update();
            
            respawnEnemiesIfNecessary();
        }
        
        NG_SERVER->sendOutgoingPackets();
    }
}

void Server::handleNewClient(ClientProxy* inClientProxy)
{
    int playerId = inClientProxy->getPlayerId();
    std::string playerName = inClientProxy->getName();
    
    spawnRobotForPlayer(playerId, playerName);
    
    if (NG_SERVER->getNumClientsConnected() == 1)
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
}

void Server::spawnRobotForPlayer(int inPlayerId, std::string inPlayerName)
{
    Robot* robot = static_cast<Robot*>(FWInstanceManager::getServerEntityRegistry()->createEntity(NETWORK_TYPE_Robot));
    robot->setPlayerId(inPlayerId);
    robot->setPlayerName(inPlayerName);
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
                SpacePirate* spacePirate = static_cast<SpacePirate*>(FWInstanceManager::getServerEntityRegistry()->createEntity(NETWORK_TYPE_SpacePirate));
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

Server::Server(bool isSteam) : m_fStateTime(0), m_fFrameStateTime(0), m_fStateTimeNoEnemies(0)
{
    FWInstanceManager::createServerEntityManager(InstanceManager::staticHandleEntityCreatedOnServer, InstanceManager::staticHandleEntityDeletedOnServer);
    
    InstanceManager::createServerWorld();
    
    FWInstanceManager::getServerEntityRegistry()->registerCreationFunction(NETWORK_TYPE_Robot, Robot::staticCreateServer);
    FWInstanceManager::getServerEntityRegistry()->registerCreationFunction(NETWORK_TYPE_Projectile, Projectile::staticCreateServer);
    FWInstanceManager::getServerEntityRegistry()->registerCreationFunction(NETWORK_TYPE_SpacePirate, SpacePirate::staticCreateServer);
    
    if (isSteam)
    {
        NetworkManagerServer::create(new NGSteamServerHelper(STEAM_GAME_DIR, VERSION_STRING, STEAM_PRODUCT_NAME, STEAM_PRODUCT_DESC, SERVER_PORT, NG_SERVER_CALLBACKS), SERVER_CALLBACKS);
    }
    else
    {
        NetworkManagerServer::create(new SocketServerHelper(SERVER_PORT, NG_SERVER_CALLBACKS), SERVER_CALLBACKS);
    }
}

Server::~Server()
{
    NetworkManagerServer::destroy();
    
    InstanceManager::destroyServerWorld();
    
    FWInstanceManager::destroyServerEntityManager();
}
