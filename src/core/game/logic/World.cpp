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

WORLD_CREATE_SERVER_IMPL(Robot);
WORLD_CREATE_SERVER_IMPL(Projectile);
WORLD_CREATE_SERVER_IMPL(SpacePirate);

World::World(bool isServer) : m_world(nullptr), m_isServer(isServer)
{
    // Define the gravity vector.
    b2Vec2 gravity(0.0f, -9.8f);
    
    // Construct a world object, which will hold and simulate the rigid bodies.
    m_world = new b2World(gravity);
    
    m_ground = new Ground(*m_world);
    
    m_entityContactListener = new EntityContactListener();
    m_entityContactFilter = new EntityContactFilter();
    
    m_world->SetContactListener(m_entityContactListener);
    m_world->SetContactFilter(m_entityContactFilter);
}

World::~World()
{
    m_entities.clear();
    
    delete m_ground;
    
    delete m_entityContactListener;
    delete m_entityContactFilter;
    
    delete m_world;
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
    
    static int32 velocityIterations = 12;
    static int32 positionIterations = 4;
    
    // Instruct the world to perform a single step of simulation.
    // It is generally best to keep the time step and iterations fixed.
    m_world->Step(Timing::getInstance()->getDeltaTime(), velocityIterations, positionIterations);
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
    
    if (entityA && entityB)
    {
        entityA->handleContact(entityB);
    }
}

bool EntityContactFilter::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
    Entity* entityA = static_cast<Entity*>(fixtureA->GetUserData());
    
    Entity* entityB = static_cast<Entity*>(fixtureB->GetUserData());
    
    return entityA->shouldCollide(entityB);
}
