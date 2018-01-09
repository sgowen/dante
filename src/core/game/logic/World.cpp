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
#include "game/logic/RobotController.h"
#include "Box2D/Box2D.h"

#include "framework/network/server/NetworkManagerServer.h"
#include "game/logic/InstanceManager.h"
#include "framework/util/Timing.h"
#include "game/logic/Server.h"
#include "framework/network/server/ClientProxy.h"
#include "framework/network/portable/MoveList.h"
#include "game/logic/GameInputManager.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "framework/util/StringUtil.h"
#include <framework/entity/EntityMapper.h>

Entity* World::sServerCreateEntity(uint32_t inFourCCName)
{
    b2World& world = InstanceManager::getServerWorld()->getWorld();
    Entity* ret = EntityMapper::getInstance()->createEntity(inFourCCName, world, true);
    NG_SERVER->registerEntity(ret);
    return ret;
}

Entity* World::sClientCreateEntity(uint32_t inFourCCName)
{
    b2World& world = InstanceManager::getClientWorld()->getWorld();
    return EntityMapper::getInstance()->createEntity(inFourCCName, world, false);
}

World::World(bool isServer) :
_entityContactListener(new EntityContactListener()),
_entityContactFilter(new EntityContactFilter()),
_isServer(isServer)
{
    // Define the gravity vector.
    b2Vec2 gravity(0.0f, -9.8f);
    
    // Construct a world object, which will hold and simulate the rigid bodies.
    _world = new b2World(gravity);
    _world->SetContactListener(_entityContactListener);
    _world->SetContactFilter(_entityContactFilter);
}

World::~World()
{
    int len = static_cast<int>(_entities.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        Entity* entity = _entities[i];
        
        removeEntity(entity);
        --i;
        --c;
    }
    
    delete _entityContactListener;
    delete _entityContactFilter;
    
    delete _world;
}

void World::addEntity(Entity* inEntity)
{
    _entities.push_back(inEntity);
    
    if (inEntity->getEntityDef().type == 'ROBT')
    {
        _players.push_back(inEntity);
    }
}

void World::removeEntity(Entity* inEntity)
{
    if (inEntity->getEntityDef().type == 'ROBT')
    {
        int len = static_cast<int>(_players.size());
        int indexToRemove = -1;
        for (int i = 0; i < len; ++i)
        {
            if (_players[i]->getID() == inEntity->getID())
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
                _players[indexToRemove] = _players[lastIndex];
            }
            
            _players.pop_back();
        }
    }
    
    int len = static_cast<int>(_entities.size());
    int indexToRemove = -1;
    for (int i = 0; i < len; ++i)
    {
        if (_entities[i]->getID() == inEntity->getID())
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
            _entities[indexToRemove] = _entities[lastIndex];
        }
        
        _entities.pop_back();
        
        inEntity->deinitPhysics();
        
        if (_isServer)
        {
            NG_SERVER->deregisterEntity(inEntity);
        }
    }
}

void World::postRead()
{
    if (!NG_CLIENT->hasReceivedNewState())
    {
        return;
    }
    
    for (Entity* entity : _entities)
    {
        entity->recallLastReadState();
    }
    
    // all processed moves have been removed, so all that are left are unprocessed moves so we must apply them...
    MoveList& moveList = GameInputManager::getInstance()->getMoveList();
    
    for (const Move& move : moveList)
    {
        for (Entity* entity : _players)
        {
            RobotController* robot = static_cast<RobotController*>(entity->getEntityController());
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
                    RobotController* robot = static_cast<RobotController*>(entity->getEntityController());
                    
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
                
                int len = static_cast<int>(_entities.size());
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
                RobotController* robot = static_cast<RobotController*>(entity->getEntityController());
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
        }
    }
}

Entity* World::getRobotWithPlayerId(uint8_t inPlayerID)
{
    for (Entity* entity : _players)
    {
        RobotController* robot = static_cast<RobotController*>(entity->getEntityController());
        if (robot->getPlayerId() == inPlayerID)
        {
            return entity;
        }
    }
    
    return NULL;
}

void World::removeAllCrates()
{
    for (Entity* entity : _entities)
    {
        if (entity->getEntityDef().type == 'CRAT')
        {
            entity->requestDeletion();
        }
    }
}

bool World::hasCrates()
{
    for (Entity* entity : _entities)
    {
        if (entity->getEntityDef().type == 'CRAT')
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
