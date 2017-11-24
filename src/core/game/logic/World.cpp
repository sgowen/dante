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
#include "game/logic/Robot.h"
#include "game/logic/Projectile.h"
#include "game/logic/SpacePirate.h"
#include "game/logic/Crate.h"
#include "game/logic/SpacePirateChunk.h"
#include "game/logic/Ground.h"
#include "Box2D/Box2D.h"

#include "framework/network/server/NetworkManagerServer.h"
#include "game/network/InstanceManager.h"
#include "framework/util/Timing.h"
#include "game/network/Server.h"
#include "framework/network/server/ClientProxy.h"
#include "framework/network/portable/MoveList.h"
#include "game/logic/InputManager.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "framework/util/StringUtil.h"

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
_entityContactListener(new EntityContactListener()),
_entityContactFilter(new EntityContactFilter()),
_isServer(isServer)
{
    // Define the gravity vector.
    b2Vec2 gravity(0.0f, -9.8f);
    
    // Construct a world object, which will hold and simulate the rigid bodies.
    _world = new b2World(gravity);
    
    _ground = new Ground(*_world, _isServer);
    
    _world->SetContactListener(_entityContactListener);
    _world->SetContactFilter(_entityContactFilter);
}

World::~World()
{
    _entities.clear();
    
    delete _entityContactListener;
    delete _entityContactFilter;
    
    delete _ground;
    
    delete _world;
}

void World::addEntity(Entity* inEntity)
{
    if (inEntity->getRTTI().derivesFrom(Robot::rtti))
    {
        _players.push_back(inEntity);
    }
    else
    {
        _entities.push_back(inEntity);
    }
}

void World::removeEntity(Entity* inEntity)
{
    std::vector<Entity*>* pEntities = NULL;
    if (inEntity->getRTTI().derivesFrom(Robot::rtti))
    {
        pEntities = &_players;
    }
    else
    {
        pEntities = &_entities;
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
        
        if (_isServer)
        {
            NG_SERVER->deregisterEntity(inEntity);
        }
    }
}

void World::postRead()
{
    if (_isServer)
    {
        return;
    }
    
    bool needsMoveReplay = false;
    for (Entity* entity : _players)
    {
        Robot* robot = static_cast<Robot*>(entity);
        if (NG_CLIENT->isPlayerIdLocal(robot->getPlayerId()))
        {
            if (robot->needsMoveReplay())
            {
                needsMoveReplay = true;
            }
        }
    }
    
    if (!needsMoveReplay)
    {
        return;
    }
    
    // all processed moves have been removed, so all that are left are unprocessed moves so we must apply them...
    MoveList& moveList = InputManager::getInstance()->getMoveList();
    
    for (const Move& move : moveList)
    {
        for (Entity* entity : _players)
        {
            Robot* robot = static_cast<Robot*>(entity);
            if (NG_CLIENT->isPlayerIdLocal(robot->getPlayerId()))
            {
                robot->processInput(move.getInputState());
            }
        }
        
        stepPhysics();
        
        for (Entity* entity : _entities)
        {
            entity->update();
        }
        
        for (Entity* entity : _players)
        {
            entity->update();
        }
    }
    
    for (Entity* entity : _entities)
    {
        entity->postRead();
    }
    
    for (Entity* entity : _players)
    {
        entity->postRead();
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

							client->setIsLastMoveTimestampDirty(true);
                        }
                    }
                }
                
                stepPhysics();
                
                // Update all game objects- sometimes they want to die, so we need to tread carefully...
                
                int len = static_cast<int>(_players.size());
                for (int i = 0, c = len; i < c; ++i)
                {
                    Entity* entity = _players[i];
                    
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
                
                len = static_cast<int>(_entities.size());
                for (int i = 0, c = len; i < c; ++i)
                {
                    Entity* entity = _entities[i];
                    
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
            for (Entity* entity : _players)
            {
                Robot* robot = static_cast<Robot*>(entity);
                if (NG_CLIENT->isPlayerIdLocal(robot->getPlayerId()))
                {
                    robot->processInput(pendingMove->getInputState(), true);
                }
            }
            
            stepPhysics();
            
            for (Entity* entity : _entities)
            {
                entity->update();
            }
            
            for (Entity* entity : _players)
            {
                entity->update();
            }
        }
    }
}

Robot* World::getRobotWithPlayerId(uint8_t inPlayerID)
{
    for (Entity* entity : _players)
    {
        Robot* robot = static_cast<Robot*>(entity);
        if (robot->getPlayerId() == inPlayerID)
        {
            return robot;
        }
    }
    
    return NULL;
}

void World::killAllSpacePirates()
{
    for (Entity* entity : _entities)
    {
        if (entity->getRTTI().derivesFrom(SpacePirate::rtti))
        {
            entity->requestDeletion();
        }
    }
}

void World::removeAllCrates()
{
    for (Entity* entity : _entities)
    {
        if (entity->getRTTI().derivesFrom(Crate::rtti))
        {
            entity->requestDeletion();
        }
    }
}

bool World::hasSpacePirates()
{
    for (Entity* entity : _entities)
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
    for (Entity* entity : _entities)
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
    return _players;
}

std::vector<Entity*>& World::getEntities()
{
    return _entities;
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
