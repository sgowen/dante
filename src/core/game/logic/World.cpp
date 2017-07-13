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
#include "Box2D/Box2D.h"
#include "Robot.h"

#include "Projectile.h"
#include "SpacePirate.h"
#include "NetworkManagerServer.h"
#include "InstanceManager.h"
#include "Timing.h"
#include "Ground.h"

#define WORLD_CREATE_CLIENT(name) \
b2World& world = InstanceManager::getClientWorld()->getWorld(); \
return new name(world, false);

#define WORLD_CREATE_SERVER(name) \
    b2World& world = InstanceManager::getServerWorld()->getWorld(); \
    Entity* ret = new name(world, true); \
    NG_SERVER->registerEntity(ret); \
    return ret;

Entity* World::sClientCreateRobot()
{
    WORLD_CREATE_CLIENT(Robot);
}

Entity* World::sServerCreateRobot()
{
    WORLD_CREATE_SERVER(Robot);
}

Entity* World::sClientCreateProjectile()
{
    WORLD_CREATE_CLIENT(Projectile);
}

Entity* World::sServerCreateProjectile()
{
    WORLD_CREATE_SERVER(Projectile);
}

Entity* World::sClientCreateSpacePirate()
{
    WORLD_CREATE_CLIENT(SpacePirate);
}

Entity* World::sServerCreateSpacePirate()
{
    WORLD_CREATE_SERVER(SpacePirate);
}

World::World(bool isServer) : m_world(nullptr), m_isServer(isServer)
{
    // Define the gravity vector.
    b2Vec2 gravity(0.0f, -9.8f);
    
    // Construct a world object, which will hold and simulate the rigid bodies.
    m_world = new b2World(gravity);
    
    static Ground ground(*m_world);
    
    static EntityContactListener entityContactListener;
    
    m_world->SetContactListener(&entityContactListener);
}

World::~World()
{
    m_entities.clear();
}

void World::addEntity(Entity* inEntity)
{
    m_entities.push_back(inEntity);
}

void World::removeEntity(Entity* inEntity)
{
    int len = static_cast<int>(m_entities.size());
    int indexToRemove = -1;
    for (int i = 0; i < len; ++i)
    {
        if (m_entities[i]->getID() == inEntity->getID())
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
            m_entities[indexToRemove] = m_entities[lastIndex];
        }
        
        m_entities.pop_back();
        
        if (m_isServer)
        {
            NG_SERVER->deregisterEntity(inEntity);
        }
    }
}

void World::update()
{
    static int32 velocityIterations = 6;
    static int32 positionIterations = 2;
    
    // Instruct the world to perform a single step of simulation.
    // It is generally best to keep the time step and iterations fixed.
    m_world->Step(Timing::getInstance()->getDeltaTime(), velocityIterations, positionIterations);
    
    // Update all game objects- sometimes they want to die, so we need to tread carefully...
    
    int len = static_cast<int>(m_entities.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        Entity* entity = m_entities[i];
        
        if (!entity->isRequestingDeletion())
        {
            entity->update();
        }
        
        if (m_isServer)
        {
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

Robot* World::getRobotWithPlayerId(uint8_t inPlayerID)
{
    for (Entity* entity : m_entities)
    {
        Robot* robot = entity->getRTTI().derivesFrom(Robot::rtti) ? static_cast<Robot*>(entity) : nullptr;
        if (robot && robot->getPlayerId() == inPlayerID)
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

std::vector<Entity*>& World::getEntities()
{
    return m_entities;
}

b2World& World::getWorld()
{
    return *m_world;
}

void EntityContactListener::BeginContact(b2Contact* contact)
{
    Entity* entityA = static_cast<Entity*>(contact->GetFixtureA()->GetBody()->GetUserData());
    
    Entity* entityB = static_cast<Entity*>(contact->GetFixtureB()->GetBody()->GetUserData());
    
    entityA->handleContact(entityB);
}
