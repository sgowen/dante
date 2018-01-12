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
#include "game/logic/PlayerController.h"
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
#include <framework/util/NGSTDUtil.h>
#include <framework/entity/EntityMapper.h>

World::World(bool isServer) :
_world(new b2World(b2Vec2(0.0f, -9.8f))),
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
    
    if (inEntity->getController()->getRTTI().derivesFrom(PlayerController::rtti))
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
    if (inEntity->getController()->getRTTI().derivesFrom(PlayerController::rtti))
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
    
    /// TODO, load from json
    for (int i = 0; i < 10; ++i)
    {
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('G001', 0 + i * 30, 2, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('G002', 2 + i * 30, 2, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('G003', 4 + i * 30, 2, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('G004', 6 + i * 30, 2, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('G005', 8 + i * 30, 2, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('G006', 10 + i * 30, 2, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('G007', 12 + i * 30, 2, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('G008', 14 + i * 30, 2, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('G009', 16 + i * 30, 2, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('G010', 18 + i * 30, 2, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('G011', 20 + i * 30, 2, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('G012', 22 + i * 30, 2, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('G013', 24 + i * 30, 2, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('G014', 26 + i * 30, 2, _isServer));
        
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('T001', 6 + i * 22, 7, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('T002', 14 + i * 22, 9, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('T003', 16 + i * 22, 15, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('T003', 18 + i * 22, 14, _isServer));
        _staticEntities.push_back(EntityMapper::getInstance()->createEntity('T003', 20 + i * 22, 13, _isServer));
    }
    
    for (Entity* entity : _staticEntities)
    {
        entity->initPhysics(getWorld());
    }
    
    if (_isServer)
    {
        srand(static_cast<unsigned>(time(0)));
        
        /// We really shouldn't have any more than 32 dynamic
        /// objects including the players in a given zone
        /// so that we can fit our entire state inside
        /// of a single packet, which needs to be a max of
        /// 1200 bytes (any larger will be dropped by some routers).
        for (uint32_t i = 0; i < 28; ++i)
        {
            int xSeed = rand() % 10 + 1;
            float posX = xSeed * 20;
            float posY = (rand() % static_cast<int>(8)) + 20;
            
            createAndRegisterEntity('CRAT', posX, posY);
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

Entity* World::createAndRegisterEntity(uint32_t inFourCCName, int x, int y)
{
    Entity* ret = EntityMapper::getInstance()->createEntity(inFourCCName, x, y, true);
    
    NG_SERVER->registerEntity(ret);
    
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
