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
#include <Box2D/Box2D.h>

#include <framework/network/server/NetworkManagerServer.h>
#include <framework/util/Timing.h>
#include <game/logic/Server.h>
#include <framework/network/server/ClientProxy.h>
#include <framework/network/portable/MoveList.h>
#include <game/game/GameInputManager.h>
#include <framework/network/client/NetworkManagerClient.h>
#include <game/entity/PlayerController.h>
#include <framework/util/StringUtil.h>
#include <framework/util/NGSTDUtil.h>
#include <framework/entity/EntityMapper.h>
#include <framework/entity/EntityLayoutMapper.h>
#include <game/logic/GameConfig.h>

World::World(uint32_t flags) :
_world(new b2World(b2Vec2(0.0f, GM_CFG->_gravity))),
_entityContactListener(new EntityContactListener()),
_entityContactFilter(new EntityContactFilter()),
_map(0),
_mapFileName(),
_mapName(),
_flags(flags)
{
    _world->SetContactListener(_entityContactListener);
    _world->SetContactFilter(_entityContactFilter);
}

World::~World()
{
    clearDynamicEntities(_players);
    clearDynamicEntities(_dynamicEntities);
    for (Entity* entity : _staticEntities)
    {
        entity->deinitPhysics();
    }
    NGSTDUtil::cleanUpVectorOfPointers(_staticEntities);
    NGSTDUtil::cleanUpVectorOfPointers(_layers);
    delete _entityContactListener;
    delete _entityContactFilter;
    delete _world;
}

void World::addDynamicEntity(Entity* inEntity)
{
    inEntity->initPhysics(getWorld());
    
    if (inEntity->getEntityDef().type == 'ROBT')
    {
        _players.push_back(inEntity);
    }
    else
    {
        _dynamicEntities.push_back(inEntity);
    }
}

void World::removeDynamicEntity(Entity* inEntity)
{
    std::vector<Entity*>* pEntities = NULL;
    if (inEntity->getEntityDef().type == 'ROBT')
    {
        pEntities = &_players;
    }
    else
    {
        pEntities = &_dynamicEntities;
    }
    
    std::vector<Entity*>& entities = *pEntities;
    
    int len = static_cast<int>(entities.size());
    int indexToRemove = -1;
    for (int i = 0; i < len; ++i)
    {
        if (_flags & WorldFlag_MapLoadAll)
        {
            if (entities[i] == inEntity)
            {
                indexToRemove = i;
                break;
            }
        }
        else
        {
            if (entities[i]->getID() == inEntity->getID())
            {
                indexToRemove = i;
                break;
            }
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
        
        inEntity->deinitPhysics();
        
        if (_flags & WorldFlag_MapLoadAll)
        {
            if (_flags & WorldFlag_Server)
            {
                NG_SERVER->deregisterEntity(inEntity);
            }
            else
            {
                delete inEntity;
            }
        }
    }
}

void World::updateServer()
{
    int moveCount = NG_SERVER->getMoveCount();
    if (moveCount > 0)
    {
        for (int i = 0; i < moveCount; ++i)
        {
            for (Entity* entity : _players)
            {
                PlayerController* robot = static_cast<PlayerController*>(entity->getController());
                
                ClientProxy* client = NG_SERVER->getClientProxy(robot->getPlayerId());
                if (client)
                {
                    MoveList& moveList = client->getUnprocessedMoveList();
                    Move* move = moveList.getMoveAtIndex(i);
                    if (move)
                    {
                        robot->processInput(move->getInputState());
                        
                        moveList.markMoveAsProcessed(move);
                        
                        client->setLastMoveTimestampDirty(true);
                    }
                }
            }
            
            stepPhysics();
            
            updateAndRemoveEntitiesAsNeeded(_players);
            updateAndRemoveEntitiesAsNeeded(_dynamicEntities);
        }
        
        postUpdateAndRemoveEntitiesAsNeeded(_players);
        postUpdateAndRemoveEntitiesAsNeeded(_dynamicEntities);
        
        for (uint8_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
        {
            ClientProxy* client = NG_SERVER->getClientProxy(i + 1);
            if (client)
            {
                MoveList& moveList = client->getUnprocessedMoveList();
                moveList.removeProcessedMoves(client->getUnprocessedMoveList().getLastProcessedMoveTimestamp(), Server::sHandleInputStateRelease);
            }
        }
    }
}

void World::postRead()
{
    for (Entity* e : _players)
    {
        e->recallLastReadState();
    }
    
    for (Entity* e : _dynamicEntities)
    {
        e->recallLastReadState();
    }
    
    // all processed moves have been removed, so all that are left are unprocessed moves so we must apply them...
    MoveList& moveList = GameInputManager::getInstance()->getMoveList();
    
    for (const Move& move : moveList)
    {
        for (Entity* e : _players)
        {
            PlayerController* robot = static_cast<PlayerController*>(e->getController());
            if (robot->isLocalPlayer())
            {
                robot->processInput(move.getInputState());
            }
        }
        
        stepPhysics();
        
        for (Entity* e : _players)
        {
            e->update();
        }
        
        for (Entity* e : _dynamicEntities)
        {
            e->update();
        }
    }
}

void World::updateClient()
{
    const Move* pendingMove = GameInputManager::getInstance()->getPendingMove();
    if (pendingMove)
    {
        for (Entity* entity : _players)
        {
            PlayerController* robot = static_cast<PlayerController*>(entity->getController());
            if (robot->isLocalPlayer())
            {
                robot->processInput(pendingMove->getInputState(), true);
            }
        }
        
        stepPhysics();
        
        for (Entity* e : _players)
        {
            e->update();
        }
        
        for (Entity* e : _dynamicEntities)
        {
            e->update();
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

void World::postRender()
{
    for (Entity* entity : _players)
    {
        entity->postRender();
    }
}

void World::loadMap(uint32_t map)
{
    _map = map;
    
    char chars[5];
    
    chars[4] = '\0';
    chars[3] = (char)(map & 0xFF);
    chars[2] = (char)(map >> 8 & 0xFF);
    chars[1] = (char)(map >> 16 & 0xFF);
    chars[0] = (char)(map >> 24 & 0xFF);
    
    _mapName = "";
    _mapFileName = "";
    
    clear();
    
    assert(_map != 0);
    
    _mapName = std::string(chars);
    
    if (map == 'TEST')
    {
        _mapFileName = std::string("test.cfg");
        EntityLayoutMapper::getInstance()->loadEntityLayout(_mapFileName);
    }
    else
    {
        _mapFileName = EntityLayoutMapper::getInstance()->getJsonConfigFilePath(_map);
        EntityLayoutMapper::getInstance()->loadEntityLayout(_map);
    }
    
    EntityLayoutDef& entityLayoutDef = EntityLayoutMapper::getInstance()->getEntityLayoutDef();
    for (EntityPosDef epd : entityLayoutDef.entities)
    {
        Entity* e = EntityMapper::getInstance()->createEntity(epd.type, epd.x, epd.y, _flags & WorldFlag_Server);
        mapAddEntity(e);
    }
}

bool World::isMapLoaded()
{
    return _map != 0;
}

void World::mapAddEntity(Entity *e)
{
    bool isLayer = e->getEntityDef().fixtures.size() == 0 && e->getEntityDef().bodyFlags == 0;
    bool isStatic = e->getEntityDef().fixtures.size() > 0 && (e->getEntityDef().bodyFlags & BodyFlag_Static);
    bool isDynamic = e->getEntityDef().fixtures.size() > 0 && !(e->getEntityDef().bodyFlags & BodyFlag_Static);
    
    if (isLayer)
    {
        _layers.push_back(e);
    }
    else if (isStatic)
    {
        e->initPhysics(getWorld());
        _staticEntities.push_back(e);
    }
    else if (isDynamic)
    {
        if (_flags & WorldFlag_MapLoadAll)
        {
            if (_flags & WorldFlag_Server)
            {
                NG_SERVER->registerEntity(e);
            }
            else
            {
                addDynamicEntity(e);
            }
        }
    }
}

void World::mapRemoveEntity(Entity* e)
{
    bool isLayer = e->getEntityDef().fixtures.size() == 0 && e->getEntityDef().bodyFlags == 0;
    bool isStatic = e->getEntityDef().fixtures.size() > 0 && (e->getEntityDef().bodyFlags & BodyFlag_Static);
    bool isDynamic = e->getEntityDef().fixtures.size() > 0 && !(e->getEntityDef().bodyFlags & BodyFlag_Static);
    
    if (isLayer)
    {
        for (std::vector<Entity*>::iterator i = _layers.begin(); i != _layers.end(); )
        {
            if (e == (*i))
            {
                delete *i;
                
                i = _layers.erase(i);
                break;
            }
            else
            {
                ++i;
            }
        }
    }
    else if (isStatic)
    {
        e->deinitPhysics();
        
        for (std::vector<Entity*>::iterator i = _staticEntities.begin(); i != _staticEntities.end(); )
        {
            if (e == (*i))
            {
                delete *i;
                
                i = _staticEntities.erase(i);
                break;
            }
            else
            {
                ++i;
            }
        }
    }
    else if (isDynamic)
    {
        if (_flags & WorldFlag_MapLoadAll)
        {
            removeDynamicEntity(e);
        }
    }
}

void World::saveMap()
{
    saveMapAs(_map);
}

void World::saveMapAs(uint32_t map)
{
    EntityLayoutDef layout;
    
    for (Entity* e : _layers)
    {
        layout.entities.push_back(EntityPosDef(e->getEntityDef().type, e->getPosition().x, e->getPosition().y));
    }
    
    for (Entity* e : _staticEntities)
    {
        layout.entities.push_back(EntityPosDef(e->getEntityDef().type, e->getPosition().x, e->getPosition().y));
    }
    
    for (Entity* e : _dynamicEntities)
    {
        layout.entities.push_back(EntityPosDef(e->getEntityDef().type, e->getPosition().x, e->getPosition().y));
    }
    
    if (map == 'TEST')
    {
        EntityLayoutMapper::getInstance()->saveEntityLayout(std::string("test.cfg"), &layout);
    }
    else
    {
        EntityLayoutMapper::getInstance()->saveEntityLayout(map, &layout);
        
        // If the save was successful, update current map
        _map = map;
    }
}

void World::clear()
{
    for (Entity* e : _staticEntities)
    {
        e->deinitPhysics();
    }
    
    NGSTDUtil::cleanUpVectorOfPointers(_staticEntities);
    NGSTDUtil::cleanUpVectorOfPointers(_layers);
    
    if (_flags & WorldFlag_MapLoadAll)
    {
        clearDynamicEntities(_players);
        clearDynamicEntities(_dynamicEntities);
    }
}

std::string& World::getMapName()
{
    return _mapName;
}

std::string& World::getMapFileName()
{
    return _mapFileName;
}

Entity* World::getPlayerWithId(uint8_t inPlayerID)
{
    for (Entity* entity : _players)
    {
        PlayerController* robot = static_cast<PlayerController*>(entity->getController());
        if (robot->getPlayerId() == inPlayerID)
        {
            return entity;
        }
    }
    
    return NULL;
}

std::vector<Entity*>& World::getPlayers()
{
    return _players;
}

std::vector<Entity*>& World::getDynamicEntities()
{
    return _dynamicEntities;
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

void World::stepPhysics()
{
    static int32 velocityIterations = 6;
    static int32 positionIterations = 2;
    
    // Instruct the world to perform a single step of simulation.
    // It is generally best to keep the time step and iterations fixed.
    _world->Step(FRAME_RATE, velocityIterations, positionIterations);
}

void World::clearDynamicEntities(std::vector<Entity*>& entities)
{
    int len = static_cast<int>(entities.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        Entity* entity = entities[i];
        
        removeDynamicEntity(entity);
        --i;
        --c;
    }
}

void World::updateAndRemoveEntitiesAsNeeded(std::vector<Entity*>& entities)
{
    int len = static_cast<int>(entities.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        Entity* entity = entities[i];
        
        if (!entity->isRequestingDeletion())
        {
            entity->update();
        }
        
        if (entity->isRequestingDeletion())
        {
            removeDynamicEntity(entity);
            --i;
            --c;
        }
    }
}

void World::postUpdateAndRemoveEntitiesAsNeeded(std::vector<Entity*>& entities)
{
    int len = static_cast<int>(entities.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        Entity* entity = entities[i];
        
        if (!entity->isRequestingDeletion())
        {
            entity->postUpdate();
        }
        
        if (entity->isRequestingDeletion())
        {
            removeDynamicEntity(entity);
            --i;
            --c;
        }
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
    
    return entityA->shouldCollide(entityB, fixtureA, fixtureB);
}
