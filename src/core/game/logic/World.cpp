//
//  World.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/World.h"

#include "framework/entity/Entity.h"
#include "Box2D/Box2D.h"

#include "framework/network/server/NetworkManagerServer.h"
#include "game/logic/InstanceManager.h"
#include "framework/util/Timing.h"
#include "game/logic/Server.h"
#include "framework/network/server/ClientProxy.h"
#include "framework/network/portable/MoveList.h"
#include "game/logic/GameInputManager.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "game/logic/PlayerController.h"
#include "framework/util/StringUtil.h"
#include <framework/util/NGSTDUtil.h>
#include <framework/entity/EntityMapper.h>
#include <framework/entity/EntityLayoutMapper.h>
#include <framework/util/Config.h>

World::World(bool isServer) :
_world(new b2World(b2Vec2(0.0f, Config::getInstance()->getFloat("gravity")))),
_entityContactListener(new EntityContactListener()),
_entityContactFilter(new EntityContactFilter()),
_map(0),
_isServer(isServer)
{
    _world->SetContactListener(_entityContactListener);
    _world->SetContactFilter(_entityContactFilter);
}

World::~World()
{
    clearEntities(_players);
    clearEntities(_dynamicEntities);
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

void World::addEntity(Entity* inEntity)
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

void World::removeEntity(Entity* inEntity)
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
        
        inEntity->deinitPhysics();
        
        if (_isServer)
        {
            NG_SERVER->deregisterEntity(inEntity);
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

void World::update()
{
    if (_isServer)
    {
        int moveCount = getMoveCount();
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

							client->setIsLastMoveTimestampDirty(true);
                        }
                    }
                }
                
                stepPhysics();
                
                updateAndRemoveEntitiesAsNeeded(_players);
                updateAndRemoveEntitiesAsNeeded(_dynamicEntities);
            }
            
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
    }
    else
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

void World::loadMapIfNecessary(uint32_t map)
{
    if (_map == map)
    {
        return;
    }
    
    _map = map;
    
    char chars[5];
    
    chars[4] = '\0';
    chars[3] = (char)(map & 0xFF);
    chars[2] = (char)(map >> 8 & 0xFF);
    chars[1] = (char)(map >> 16 & 0xFF);
    chars[0] = (char)(map >> 24 & 0xFF);
    
    _mapName = std::string(chars);
    
    for (Entity* entity : _staticEntities)
    {
        entity->deinitPhysics();
    }
    
    NGSTDUtil::cleanUpVectorOfPointers(_staticEntities);
    NGSTDUtil::cleanUpVectorOfPointers(_layers);
    
    std::vector<uint8_t> playerIds;
    std::vector<std::string> playerNames;
    
    if (_isServer)
    {
        for (Entity* entity : _players)
        {
            PlayerController* robot = static_cast<PlayerController*>(entity->getController());
            
            playerIds.push_back(robot->getPlayerId());
            playerNames.push_back(robot->getPlayerName());
        }
        
        clearEntities(_players);
        clearEntities(_dynamicEntities);
        
        NG_SERVER->setMap(_map);
    }
    
    if (_map == 0)
    {
        return;
    }
    
    for (int i = 0; i < playerIds.size(); ++i)
    {
        Server::sHandleNewClient(playerIds[i], playerNames[i]);
    }
    
    EntityLayoutMapper::getInstance()->loadEntityLayout(_map);
    EntityLayoutDef& entityLayoutDef = EntityLayoutMapper::getInstance()->getEntityLayoutDef();
    
    for (EntityPosDef epd : entityLayoutDef.layers)
    {
        Entity* e = EntityMapper::getInstance()->createEntity(epd.type, epd.x, epd.y, _isServer);
        _layers.push_back(e);
    }
    
    for (EntityPosDef epd : entityLayoutDef.staticEntities)
    {
        Entity* e = EntityMapper::getInstance()->createEntity(epd.type, epd.x, epd.y, _isServer);
        e->initPhysics(getWorld());
        _staticEntities.push_back(e);
    }
    
    if (_isServer)
    {
        for (EntityPosDef epd : entityLayoutDef.dynamicEntities)
        {
            Entity* e = EntityMapper::getInstance()->createEntity(epd.type, epd.x, epd.y, _isServer);
            NG_SERVER->registerEntity(e);
        }
    }
}

std::string& World::getMapName()
{
    return _mapName;
}

Entity* World::getRobotWithPlayerId(uint8_t inPlayerID)
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

int World::getMoveCount()
{
    int ret = 0;
    
    int lowestNonHostMoveCount = NG_SERVER->getLowestNonHostMoveCount();
    int hostMoveCount = NG_SERVER->getHostMoveCount();
    
    if (lowestNonHostMoveCount == -1
        || (hostMoveCount <= lowestNonHostMoveCount
            && (hostMoveCount * 2) >= lowestNonHostMoveCount))
    {
        ret = hostMoveCount;
    }
    else if (lowestNonHostMoveCount <= hostMoveCount
             && (lowestNonHostMoveCount * 2) >= hostMoveCount)
    {
        ret = lowestNonHostMoveCount;
    }
    else if (lowestNonHostMoveCount >= 8 || hostMoveCount >= 8)
    {
        ret = NG_SERVER->getAverageMoveCount();
        
        LOG("lowestNonHostMoveCount: %d, hostMoveCount: %d, finalMoveCount(avg): %d", lowestNonHostMoveCount, hostMoveCount, ret);
    }
    
    return ret;
}

void World::clearEntities(std::vector<Entity*>& entities)
{
    int len = static_cast<int>(entities.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        Entity* entity = entities[i];
        
        removeEntity(entity);
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
            removeEntity(entity);
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
