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
#include "Crate.h"
#include "IMachineAddress.h"

#ifdef NG_STEAM
#include "NGSteamServerHelper.h"
#endif

#include "Box2D/Box2D.h"

#include <ctime> // rand
#include <assert.h>

#define SERVER_CALLBACKS Server::sHandleNewClient, Server::sHandleLostClient, PooledObjectsManager::borrowInputState

Server* Server::s_instance = nullptr;

void Server::create(bool isSteam, uint32_t inNumCratesToSpawn, uint32_t inNumSpacePiratesToSpawn)
{
    assert(!s_instance);
    
    s_instance = new Server(isSteam, inNumCratesToSpawn, inNumSpacePiratesToSpawn);
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

void Server::sHandleNewClient(uint8_t playerId, std::string playerName)
{
    getInstance()->handleNewClient(playerId, playerName);
}

void Server::sHandleLostClient(ClientProxy* inClientProxy, uint8_t index)
{
    getInstance()->handleLostClient(inClientProxy, index);
}

void Server::update(float deltaTime)
{
    _frameStateTime += deltaTime;
    
    if (_frameStateTime >= FRAME_RATE)
    {
        while (_frameStateTime >= FRAME_RATE)
        {
            _frameStateTime -= FRAME_RATE;
            _stateTime += FRAME_RATE;
            
            Timing::getInstance()->updateManual(_stateTime, FRAME_RATE);
            
            NG_SERVER->processIncomingPackets();
            NG_SERVER->checkForDisconnects();
            
            InstanceManager::getServerWorld()->update();
            
            respawnEnemiesIfNecessary();
            spawnCratesIfNecessary();
            
            clearClientMoves();
        }

		NG_SERVER->sendOutgoingPackets();
    }
}

uint8_t Server::getPlayerIdForRobotBeingCreated()
{
    return m_iPlayerIdForRobotBeingCreated;
}

void Server::toggleEnemies()
{
    m_isSpawningEnemies = !m_isSpawningEnemies;
    
    if (m_isSpawningEnemies)
    {
        _stateTimeNoEnemies = 5; // Spawn em now!
    }
    else
    {
        InstanceManager::getServerWorld()->killAllSpacePirates();
    }
}

void Server::toggleObjects()
{
    m_isSpawningObjects = !m_isSpawningObjects;
    
    if (!m_isSpawningObjects)
    {
        InstanceManager::getServerWorld()->removeAllCrates();
    }
}

void Server::toggleDisplaying()
{
    m_isDisplaying = !m_isDisplaying;
}

bool Server::isSpawningEnemies()
{
    return m_isSpawningEnemies;
}

bool Server::isSpawningObjects()
{
    return m_isSpawningObjects;
}

bool Server::isDisplaying()
{
    return m_isDisplaying;
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
    Robot* robot = InstanceManager::getServerWorld()->getRobotWithPlayerId(playerId);
    if (robot)
    {
        robot->requestDeletion();
    }
}

void Server::spawnRobotForPlayer(uint8_t inPlayerId, std::string inPlayerName)
{
    m_iPlayerIdForRobotBeingCreated = inPlayerId;
    Robot* robot = static_cast<Robot*>(SERVER_ENTITY_REG->createEntity(NW_TYPE_Robot));
    
    ClientProxy* client = NG_SERVER->getClientProxy(inPlayerId);
    robot->setAddressHash(client->getMachineAddress()->getHash());
    
    robot->setPlayerName(inPlayerName);
    float posX = (rand() % static_cast<int>(GAME_WIDTH - robot->getWidth() * 2)) + (robot->getWidth() * 2);
    robot->setPosition(b2Vec2(posX - static_cast<float>(inPlayerId), 8.0f));
    
    static Color Red(1.0f, 0.0f, 0.0f, 1);
    static Color Green(0.0f, 1.0f, 0.0f, 1);
    static Color Blue(0.0f, 0.0f, 1.0f, 1);
    
    switch (inPlayerId)
    {
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
    
    // Doing this last on purpose
    robot->setPlayerId(inPlayerId);
}

void Server::respawnEnemiesIfNecessary()
{
    if (m_isSpawningEnemies && !InstanceManager::getServerWorld()->hasSpacePirates())
    {
        _stateTimeNoEnemies += FRAME_RATE;
        if (_stateTimeNoEnemies > 5)
        {
            srand(static_cast<unsigned>(time(0)));
            
            _stateTimeNoEnemies = 0;
            
            for (uint32_t i = 0; i < m_iNumSpacePiratesToSpawn; ++i)
            {
                SpacePirate* spacePirate = static_cast<SpacePirate*>(SERVER_ENTITY_REG->createEntity(NW_TYPE_SpacePirate));
                
                float posX = (rand() % static_cast<int>(GAME_WIDTH - spacePirate->getWidth() * 2)) + (spacePirate->getWidth() * 2);
                float posY = (rand() % static_cast<int>(GAME_HEIGHT - spacePirate->getHeight() * 2)) + (2.0f + spacePirate->getHeight() * 2);
                float speed = (rand() % 100) * 0.05f + 1.0f;
                int scale = static_cast<int>(rand() % 3) + 1;
                uint8_t health = static_cast<uint8_t>(rand() % 60) + 4;
                
                spacePirate->init(posX, posY, speed, scale, health);
                
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

void Server::spawnCratesIfNecessary()
{
    if (!m_isSpawningObjects || InstanceManager::getServerWorld()->hasCrates())
    {
        return;
    }
    
    srand(static_cast<unsigned>(time(0)));
    
    for (uint32_t i = 0; i < m_iNumCratesToSpawn; ++i)
    {
        Crate* crate = static_cast<Crate*>(SERVER_ENTITY_REG->createEntity(NW_TYPE_Crate));
        
        int xSeed = rand() % 3 + 1;
        float posX = xSeed * GAME_WIDTH / 4;
        float posY = (rand() % static_cast<int>(GAME_HEIGHT - crate->getHeight() * 2)) + (2.0f + crate->getHeight() * 2);
        
        crate->setPosition(b2Vec2(posX, posY));
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

Server::Server(bool isSteam, uint32_t inNumCratesToSpawn, uint32_t inNumSpacePiratesToSpawn) : _stateTime(0), _frameStateTime(0), _stateTimeNoEnemies(0), m_iPlayerIdForRobotBeingCreated(0), m_iNumCratesToSpawn(inNumCratesToSpawn), m_iNumSpacePiratesToSpawn(inNumSpacePiratesToSpawn), m_isSpawningEnemies(false), m_isSpawningObjects(false), m_isDisplaying(false)
{
    FWInstanceManager::createServerEntityManager(InstanceManager::sHandleEntityCreatedOnServer, InstanceManager::sHandleEntityDeletedOnServer);
    
    InstanceManager::createServerWorld();
    
    SERVER_ENTITY_REG->registerCreationFunction(NW_TYPE_Robot, World::sServerCreateRobot);
    SERVER_ENTITY_REG->registerCreationFunction(NW_TYPE_Projectile, World::sServerCreateProjectile);
    SERVER_ENTITY_REG->registerCreationFunction(NW_TYPE_SpacePirate, World::sServerCreateSpacePirate);
    SERVER_ENTITY_REG->registerCreationFunction(NW_TYPE_Crate, World::sServerCreateCrate);
    SERVER_ENTITY_REG->registerCreationFunction(NW_TYPE_SpacePirateChunk, World::sServerCreateSpacePirateChunk);
    
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
}

Server::~Server()
{
    NetworkManagerServer::destroy();
    
    FWInstanceManager::destroyServerEntityManager();
    
    InstanceManager::destroyServerWorld();
}
