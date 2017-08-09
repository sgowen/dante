//
//  World.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "World.h"

#include "Entity.h"
#include "Robot.h"
#include "Projectile.h"
#include "SpacePirate.h"
#include "Crate.h"
#include "SpacePirateChunk.h"
#include "Ground.h"
#include "Box2D/Box2D.h"

#include "NetworkManagerServer.h"
#include "InstanceManager.h"
#include "Timing.h"
#include "Server.h"
#include "ClientProxy.h"
#include "MoveList.h"
#include "InputManager.h"
#include "NetworkManagerClient.h"
#include "StringUtil.h"

#define WORLD_CREATE_CLIENT_IMPL(name) \
Entity* World::sClientCreate##name() \
{ \
    b2World& world = InstanceManager::getClientWorld()->getWorld(); \
    return new name(world, false); \
}

#define WORLD_CREATE_SERVER_IMPL(name) \
Entity* World::sServerCreate##name() \
{ \
    b2World& world = InstanceManager::getServerWorld()->getWorld(); \
    Entity* ret = new name(world, true); \
    NG_SERVER->registerEntity(ret); \
    return ret; \
}

WORLD_CREATE_CLIENT_IMPL(Robot);
WORLD_CREATE_CLIENT_IMPL(Projectile);
WORLD_CREATE_CLIENT_IMPL(SpacePirate);
WORLD_CREATE_CLIENT_IMPL(Crate);
WORLD_CREATE_CLIENT_IMPL(SpacePirateChunk);

WORLD_CREATE_SERVER_IMPL(Robot);
WORLD_CREATE_SERVER_IMPL(Projectile);
WORLD_CREATE_SERVER_IMPL(SpacePirate);
WORLD_CREATE_SERVER_IMPL(Crate);
WORLD_CREATE_SERVER_IMPL(SpacePirateChunk);

World::World(bool isServer) :
m_entityContactListener(new EntityContactListener()),
m_entityContactFilter(new EntityContactFilter()),
m_isServer(isServer)
{
    // Define the gravity vector.
    b2Vec2 gravity(0.0f, -9.8f);
    
    // Construct a world object, which will hold and simulate the rigid bodies.
    m_world = new b2World(gravity);
    
    m_ground = new Ground(*m_world, m_isServer);
    
    m_world->SetContactListener(m_entityContactListener);
    m_world->SetContactFilter(m_entityContactFilter);
}

World::~World()
{
    m_entities.clear();
    
    delete m_entityContactListener;
    delete m_entityContactFilter;
    
    delete m_ground;
    
    delete m_world;
}

void World::addEntity(Entity* inEntity)
{
    if (inEntity->getRTTI().derivesFrom(Robot::rtti))
    {
        m_players.push_back(inEntity);
    }
    else
    {
        m_entities.push_back(inEntity);
    }
}

void World::removeEntity(Entity* inEntity)
{
    std::vector<Entity*>* pEntities = nullptr;
    if (inEntity->getRTTI().derivesFrom(Robot::rtti))
    {
        pEntities = &m_players;
    }
    else
    {
        pEntities = &m_entities;
    }
    
    std::vector<Entity*>& entities = *pEntities;
    int len = static_cast<int>(entities.size());
    int indexToRemove = -1;
    for (int i = 0; i < len; ++i)
    {
        if (entities[i]->getID() == inEntity->getID())
        {
            indexToRemove = i;
            break;
        }
    }
    
    if (indexToRemove > -1)
    {
        int lastIndex = len - 1;
        
        if (indexToRemove != lastIndex)
        {
            entities[indexToRemove] = entities[lastIndex];
        }
        
        entities.pop_back();
        
        if (m_isServer)
        {
            NG_SERVER->deregisterEntity(inEntity);
        }
    }
}

void World::postRead()
{
    // all processed moves have been removed, so all that are left are unprocessed moves so we must apply them...
    MoveList& moveList = InputManager::getInstance()->getMoveList();
    
    for (const Move& move : moveList)
    {
        for (Entity* entity : m_entities)
        {
            entity->recallIfNecessary(move);
        }
        
        for (Entity* entity : m_players)
        {
            entity->recallIfNecessary(move);
            
            Robot* robot = static_cast<Robot*>(entity);
            if (NG_CLIENT->isPlayerIdLocal(robot->getPlayerId()))
            {
                robot->processInput(move.getInputState());
            }
        }
        
        stepPhysics(FRAME_RATE);
        
        for (Entity* entity : m_entities)
        {
            entity->update();
        }
        
        for (Entity* entity : m_players)
        {
            entity->update();
        }
    }
    
    for (Entity* entity : m_entities)
    {
        entity->postRead();
    }
    
    for (Entity* entity : m_players)
    {
        entity->postRead();
    }
}

void World::update()
{
    if (m_isServer)
    {
        bool isAvgMethod = Server::getInstance()->isAvgMethod();
        int avgMoveCount = NG_SERVER->getAverageMoveCount();
        int lowestNonHostMoveCount = NG_SERVER->getLowestNonHostMoveCount();
        int hostMoveCount = NG_SERVER->getHostMoveCount();
        
        int finalMoveCount = Server::getInstance()->isAvgMethod() ? avgMoveCount : lowestNonHostMoveCount;
        
        if (!isAvgMethod)
        {
            if (hostMoveCount > 6 || lowestNonHostMoveCount > 6)
            {
                finalMoveCount = avgMoveCount;
            }
            else if (hostMoveCount >= lowestNonHostMoveCount)
            {
                finalMoveCount = lowestNonHostMoveCount;
            }
            else if (hostMoveCount < lowestNonHostMoveCount)
            {
                finalMoveCount = 0;
            }
        }
        
        LOG("avgMoveCount: %d, lowestNonHostMoveCount: %d, hostMoveCount: %d, finalMoveCount: %d", avgMoveCount, lowestNonHostMoveCount, hostMoveCount, finalMoveCount)
        
        if (finalMoveCount > 0)
        {
            for (int i = 0; i < finalMoveCount; ++i)
            {
                for (Entity* entity : m_players)
                {
                    Robot* robot = static_cast<Robot*>(entity);
                    
                    // is there a move we haven't processed yet?
                    ClientProxy* client = NG_SERVER->getClientProxy(robot->getPlayerId());
                    if (client)
                    {
                        MoveList& moveList = client->getUnprocessedMoveList();
                        
                        Move* move = moveList.getMoveAtIndex(i);
                        
                        if (move)
                        {
                            robot->processInput(move->getInputState());
                            
                            moveList.markMoveAsProcessed(move);
                        }
                    }
                }
                
                stepPhysics(FRAME_RATE);
                
                // Update all game objects- sometimes they want to die, so we need to tread carefully...
                
                int len = static_cast<int>(m_players.size());
                for (int i = 0, c = len; i < c; ++i)
                {
                    Entity* entity = m_players[i];
                    
                    if (!entity->isRequestingDeletion())
                    {
                        entity->update();
                    }
                    
                    // You might suddenly want to die after your update, so check again
                    if (entity->isRequestingDeletion())
                    {
                        removeEntity(entity);
                        --i;
                        --c;
                    }
                }
                
                len = static_cast<int>(m_entities.size());
                for (int i = 0, c = len; i < c; ++i)
                {
                    Entity* entity = m_entities[i];
                    
                    if (!entity->isRequestingDeletion())
                    {
                        entity->update();
                    }
                    
                    // You might suddenly want to die after your update, so check again
                    if (entity->isRequestingDeletion())
                    {
                        removeEntity(entity);
                        --i;
                        --c;
                    }
                }
            }
        }
    }
    else
    {
        const Move* pendingMove = InputManager::getInstance()->getPendingMove();
        if (pendingMove)
        {
            for (Entity* entity : m_entities)
            {
                entity->cacheToMove(*pendingMove);
            }
            
            for (Entity* entity : m_players)
            {
                entity->cacheToMove(*pendingMove);
                
                Robot* robot = static_cast<Robot*>(entity);
                if (NG_CLIENT->isPlayerIdLocal(robot->getPlayerId()))
                {
                    robot->processInput(pendingMove->getInputState(), true);
                }
            }
            
            stepPhysics(FRAME_RATE);
            
            for (Entity* entity : m_entities)
            {
                entity->update();
            }
            
            for (Entity* entity : m_players)
            {
                entity->update();
            }
        }
    }
}

Robot* World::getRobotWithPlayerId(uint8_t inPlayerID)
{
    for (Entity* entity : m_players)
    {
        Robot* robot = static_cast<Robot*>(entity);
        if (robot->getPlayerId() == inPlayerID)
        {
            return robot;
        }
    }
    
    return nullptr;
}

void World::killAllSpacePirates()
{
    for (Entity* entity : m_entities)
    {
        if (entity->getRTTI().derivesFrom(SpacePirate::rtti))
        {
            entity->requestDeletion();
        }
    }
}

void World::removeAllCrates()
{
    for (Entity* entity : m_entities)
    {
        if (entity->getRTTI().derivesFrom(Crate::rtti))
        {
            entity->requestDeletion();
        }
    }
}

bool World::hasSpacePirates()
{
    for (Entity* entity : m_entities)
    {
        if (entity->getRTTI().derivesFrom(SpacePirate::rtti))
        {
            return true;
        }
    }
    
    return false;
}

bool World::hasCrates()
{
    for (Entity* entity : m_entities)
    {
        if (entity->getRTTI().derivesFrom(Crate::rtti))
        {
            return true;
        }
    }
    
    return false;
}

std::vector<Entity*>& World::getPlayers()
{
    return m_players;
}

std::vector<Entity*>& World::getEntities()
{
    return m_entities;
}

b2World& World::getWorld()
{
    return *m_world;
}

void World::stepPhysics(float deltaTime)
{
    static int32 velocityIterations = 6;
    static int32 positionIterations = 2;
    
    // Instruct the world to perform a single step of simulation.
    // It is generally best to keep the time step and iterations fixed.
    m_world->Step(deltaTime, velocityIterations, positionIterations);
}

void EntityContactListener::BeginContact(b2Contact* contact)
{
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    
    Entity* entityA = static_cast<Entity*>(fixtureA->GetBody()->GetUserData());
    Entity* entityB = static_cast<Entity*>(fixtureB->GetBody()->GetUserData());
    
    entityA->handleBeginContact(entityB, fixtureA, fixtureB);
}

void EntityContactListener::EndContact(b2Contact* contact)
{
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    
    Entity* entityA = static_cast<Entity*>(fixtureA->GetBody()->GetUserData());
    Entity* entityB = static_cast<Entity*>(fixtureB->GetBody()->GetUserData());
    
    entityA->handleEndContact(entityB, fixtureA, fixtureB);
}

bool EntityContactFilter::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
    Entity* entityA = static_cast<Entity*>(fixtureA->GetUserData());
    Entity* entityB = static_cast<Entity*>(fixtureB->GetUserData());
    
    return entityA->shouldCollide(entityB, fixtureA, fixtureB);
}
