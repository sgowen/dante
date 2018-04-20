//
//  World.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/logic/World.h>

#include <framework/entity/Entity.h>
#include <framework/entity/EntityIDManager.h>
#include <Box2D/Box2D.h>

#include <framework/network/server/NetworkManagerServer.h>
#include <framework/util/Timing.h>
#include <framework/network/server/ClientProxy.h>
#include <framework/network/client/NetworkManagerClient.h>
#include <framework/util/StringUtil.h>
#include <framework/util/NGSTDUtil.h>
#include <framework/entity/EntityMapper.h>
#include <framework/entity/EntityLayoutMapper.h>
#include <framework/entity/EntityNetworkController.h>
#include <framework/util/InstanceManager.h>
#include <framework/util/Config.h>

World::World(uint32_t flags) :
_flags(flags),
_world(new b2World(b2Vec2(0.0f, FW_CFG->_gravity))),
_entityContactListener(new EntityContactListener()),
_entityContactFilter(new EntityContactFilter()),
_entityIDManager(static_cast<EntityIDManager*>(INSTANCE_MANAGER->getInstance(_flags & WorldFlag_Studio ? INSTANCE_ENTITY_ID_MANAGER_STUDIO : _flags & WorldFlag_Server ? INSTANCE_ENTITY_ID_MANAGER_SERVER : INSTANCE_ENTITY_ID_MANAGER_CLIENT))),
_map()
{
    _world->SetContactListener(_entityContactListener);
    _world->SetContactFilter(_entityContactFilter);
}

World::~World()
{
    clear();
    
    deinitPhysics(_dynamicEntities);
    NGSTDUtil::cleanUpVectorOfPointers(_dynamicEntities);
    
    delete _entityContactListener;
    delete _entityContactFilter;
    delete _world;
}

void World::loadMap(uint32_t map)
{
    assert(map);
    
    clear();
    
    EntityLayoutMapper* elm = EntityLayoutMapper::getInstance();
    
    _map.key = map;
    _map.name = StringUtil::stringFromFourChar(map);
    _map.fileName = elm->getJsonConfigFilePath(map);
    
    elm->loadEntityLayout(_map.key, _entityIDManager);
    
    EntityLayoutDef& eld = elm->getEntityLayoutDef();
    for (EntityInstanceDef eid : eld.entities)
    {
        EntityDef* ed = EntityMapper::getInstance()->getEntityDef(eid.type);
        if (_flags == 0 && isDynamic(*ed))
        {
            // On the client, Dynamic Entities must arrive via network
            continue;
        }
        
        Entity* e = EntityMapper::getInstance()->createEntity(&eid, _flags & WorldFlag_Server);
        addEntity(e);
    }
}

void World::saveMap()
{
    saveMapAs(_map.key);
}

void World::saveMapAs(uint32_t map)
{
    EntityLayoutDef layout;
    
    for (Entity* e : _layers)
    {
        layout.entities.push_back(EntityInstanceDef(e->getID(), e->getEntityDef().type, e->getPosition().x, e->getPosition().y));
    }
    
    for (Entity* e : _waterBodies)
    {
        layout.entities.push_back(EntityInstanceDef(e->getID(), e->getEntityDef().type, e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight()));
    }
    
    for (Entity* e : _staticEntities)
    {
        layout.entities.push_back(EntityInstanceDef(e->getID(), e->getEntityDef().type, e->getPosition().x, e->getPosition().y));
    }
    
    for (Entity* e : _dynamicEntities)
    {
        if (e->getEntityDef().bodyFlags & BodyFlag_Player)
        {
            continue;
        }
        
        layout.entities.push_back(EntityInstanceDef(e->getID(), e->getEntityDef().type, e->getPosition().x, e->getPosition().y));
    }
    
    EntityLayoutMapper* elm = EntityLayoutMapper::getInstance();
    elm->saveEntityLayout(map, &layout);
    
    if (map != 'TEST')
    {
        // If the save was successful, update current map
        _map.key = map;
    }
}

void World::addEntity(Entity *e)
{
    if (isLayer(e))
    {
        _layers.push_back(e);
    }
    else if (isWater(e))
    {
        e->initPhysics(getWorld());
        
        _waterBodies.push_back(e);
    }
    else if (isStatic(e))
    {
        e->initPhysics(getWorld());
        
        _staticEntities.push_back(e);
    }
    else if (isDynamic(e))
    {
        if (_flags & WorldFlag_Server)
        {
            NG_SERVER->registerEntity(e);
        }
        
        e->initPhysics(getWorld());
        
        _dynamicEntities.push_back(e);
        
        refreshPlayers();
    }
}

void World::removeEntity(Entity* e)
{
    if (isLayer(e))
    {
        removeEntity(e, _layers);
    }
    else if (isWater(e))
    {
        e->deinitPhysics();
        
        removeEntity(e, _waterBodies);
    }
    else if (isStatic(e))
    {
        e->deinitPhysics();
        
        removeEntity(e, _staticEntities);
    }
    else if (isDynamic(e))
    {
        if (_flags & WorldFlag_Server)
        {
            NG_SERVER->deregisterEntity(e);
        }
        
        e->deinitPhysics();
        
        removeEntity(e, _dynamicEntities);
        
        refreshPlayers();
    }
}

void World::stepPhysics()
{
    static int32 velocityIterations = 6;
    static int32 positionIterations = 2;
    
    // Instruct the world to perform a single step of simulation.
    // It is generally best to keep the time step and iterations fixed.
    _world->Step(FRAME_RATE, velocityIterations, positionIterations);
}

void World::updateAndRemoveEntitiesAsNeeded(std::vector<Entity*>& entities)
{
    std::vector<Entity*> toDelete;
    
    for (Entity* e : entities)
    {
        if (!e->isRequestingDeletion())
        {
            e->update();
        }
        
        if (e->isRequestingDeletion())
        {
            toDelete.push_back(e);
        }
    }
    
    for (Entity* e : toDelete)
    {
        removeEntity(e);
    }
}

void World::handleDirtyStates(std::vector<Entity*>& entities)
{
    for (Entity* e : entities)
    {
        uint16_t dirtyState = e->getNetworkController()->getDirtyState();
        if (dirtyState > 0)
        {
            NG_SERVER->setStateDirty(e->getID(), dirtyState);
        }
    }
}

void World::interpolate(double alpha)
{
    for (Entity* entity : _players)
    {
        entity->interpolate(alpha);
    }
}

void World::endInterpolation()
{
    for (Entity* entity : _players)
    {
        entity->endInterpolation();
    }
}

void World::clear()
{
    deinitPhysics(_waterBodies);
    deinitPhysics(_staticEntities);
    
    NGSTDUtil::cleanUpVectorOfPointers(_layers);
    NGSTDUtil::cleanUpVectorOfPointers(_waterBodies);
    NGSTDUtil::cleanUpVectorOfPointers(_staticEntities);
    
    if (_flags > 0)
    {
        deinitPhysics(_dynamicEntities);
        NGSTDUtil::cleanUpVectorOfPointers(_dynamicEntities);
        refreshPlayers();
    }
}

bool World::isMapLoaded()
{
    return _map.key > 0;
}

std::string& World::getMapName()
{
    return _map.name;
}

std::string& World::getMapFileName()
{
    return _map.fileName;
}

std::vector<Entity*>& World::getPlayers()
{
    return _players;
}

std::vector<Entity*>& World::getDynamicEntities()
{
    return _dynamicEntities;
}

std::vector<Entity*>& World::getWaterBodies()
{
    return _waterBodies;
}

std::vector<Entity*>& World::getStaticEntities()
{
    return _staticEntities;
}

std::vector<Entity*>& World::getLayers()
{
    return _layers;
}

b2World& World::getWorld()
{
    return *_world;
}

bool World::isLayer(Entity* e)
{
    return e->getEntityDef().fixtures.size() == 0 &&
    e->getEntityDef().bodyFlags == 0;
}

bool World::isWater(Entity* e)
{
    return e->getEntityDef().bodyFlags & BodyFlag_Water;
}

bool World::isStatic(Entity* e)
{
    return e->getEntityDef().fixtures.size() > 0 &&
    (e->getEntityDef().bodyFlags & BodyFlag_Static) &&
    !e->getEntityDef().stateSensitive;
}

bool World::isDynamic(Entity* e)
{
    return isDynamic(e->getEntityDef());
}

bool World::isDynamic(EntityDef& ed)
{
    return ed.fixtures.size() > 0 && (!(ed.bodyFlags & BodyFlag_Static) || ed.stateSensitive);
}

void World::refreshPlayers()
{
    _players.clear();
    
    for (Entity* e : _dynamicEntities)
    {
        if (e->getEntityDef().bodyFlags & BodyFlag_Player)
        {
            _players.push_back(e);
        }
    }
}

void World::removeEntity(Entity* e, std::vector<Entity*>& entities)
{
    assert(e);
    
    for (std::vector<Entity*>::iterator i = entities.begin(); i != entities.end(); )
    {
        if (e->getID() == (*i)->getID())
        {
            delete *i;
            
            i = entities.erase(i);
            return;
        }
        else
        {
            ++i;
        }
    }
}

void World::deinitPhysics(std::vector<Entity*>& entities)
{
    for (Entity* e : entities)
    {
        e->deinitPhysics();
    }
}

void EntityContactListener::BeginContact(b2Contact* contact)
{
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    
    Entity* entityA = static_cast<Entity*>(fixtureA->GetBody()->GetUserData());
    Entity* entityB = static_cast<Entity*>(fixtureB->GetBody()->GetUserData());
    
    entityA->handleBeginContact(entityB, fixtureA, fixtureB);
    entityB->handleBeginContact(entityA, fixtureB, fixtureA);
}

void EntityContactListener::EndContact(b2Contact* contact)
{
    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();
    
    Entity* entityA = static_cast<Entity*>(fixtureA->GetBody()->GetUserData());
    Entity* entityB = static_cast<Entity*>(fixtureB->GetBody()->GetUserData());
    
    entityA->handleEndContact(entityB, fixtureA, fixtureB);
    entityB->handleEndContact(entityA, fixtureB, fixtureA);
}

bool EntityContactFilter::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
    Entity* entityA = static_cast<Entity*>(fixtureA->GetUserData());
    Entity* entityB = static_cast<Entity*>(fixtureB->GetUserData());
    
    return entityA->shouldCollide(entityB, fixtureA, fixtureB) &&
    entityB->shouldCollide(entityA, fixtureB, fixtureA);
}
