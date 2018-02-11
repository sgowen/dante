//
//  DestructibleController.cpp
//  dante
//
//  Created by Stephen Gowen on 2/2/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/entity/DestructibleController.h>

#include <framework/entity/Entity.h>
#include <framework/network/portable/InputMemoryBitStream.h>
#include <framework/network/portable/OutputMemoryBitStream.h>
#include <Box2D/Box2D.h>
#include <game/game/GameInputState.h>
#include <framework/network/portable/Move.h>

#include <game/logic/World.h>
#include <framework/util/macros.h>
#include <framework/util/Constants.h>
#include <framework/util/Timing.h>
#include <framework/util/StringUtil.h>
#include <framework/util/MathUtil.h>
#include <framework/network/server/NetworkManagerServer.h>
#include <framework/network/server/ClientProxy.h>
#include <framework/network/client/NetworkManagerClient.h>
#include <game/game/GameInputManager.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <game/logic/Util.h>
#include <game/logic/Server.h>
#include <framework/util/Config.h>

NGRTTI_IMPL(DestructibleController, EntityController);

EntityController* DestructibleController::create(Entity* inEntity)
{
    return new DestructibleController(inEntity);
}

DestructibleController::DestructibleController(Entity* inEntity) : EntityController(inEntity),
_stats(),
_statsCache(_stats)
{
    // Empty
}

DestructibleController::~DestructibleController()
{
    // Empty
}

void DestructibleController::update()
{
    /// TODO
}

void DestructibleController::postUpdate()
{
    if (isDestructing() && _entity->getPose().stateTime >= 120)
    {
        _entity->requestDeletion();
    }
    
    if (_statsCache != _stats)
    {
        _statsCache = _stats;
        NG_SERVER->setStateDirty(_entity->getID(), ReadStateFlag_Stats);
    }
}

void DestructibleController::receiveMessage(uint16_t message, void* data)
{
    switch (message)
    {
        case ENTITY_MESSAGE_DAMAGE:
        {
            uint32_t* damageP = static_cast<uint32_t*>(data);
            uint32_t& damage = *damageP;
            damage = clamp(damage, _stats.health, 0);
            _stats.health -= damage;
            if (_stats.health == 0)
            {
                _entity->getPose().state |= State_Destructing;
                _entity->getPose().stateTime = 0;
            }
        }
            break;
        default:
            break;
    }
}

void DestructibleController::onFixturesCreated(std::vector<b2Fixture*>& fixtures)
{
    // Empty
}

bool DestructibleController::shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    return true;
}

void DestructibleController::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

void DestructibleController::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

void DestructibleController::read(InputMemoryBitStream& inInputStream, uint16_t& inReadState)
{
    bool stateBit;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_stats.health);
        
        inReadState |= ReadStateFlag_Stats;
        _statsCache = _stats;
    }
}

void DestructibleController::recallLastReadState(uint16_t& inReadState)
{
    if (inReadState & ReadStateFlag_Stats)
    {
        _stats = _statsCache;
    }
}

uint16_t DestructibleController::write(OutputMemoryBitStream& inOutputStream, uint16_t inWrittenState, uint16_t inDirtyState)
{
    uint16_t writtenState = inWrittenState;
    
    bool stats = inDirtyState & ReadStateFlag_Stats;
    inOutputStream.write(stats);
    if (stats)
    {
        inOutputStream.write(_stats.health);
        
        writtenState |= ReadStateFlag_Stats;
    }
    
    return writtenState;
}

uint8_t DestructibleController::getHealth()
{
    return _stats.health;
}

bool DestructibleController::isDestructing()
{
    return _entity->getPose().state & StateFlag_Destructing;
}

bool DestructibleController::isMoving()
{
    return !isCloseEnough(_entity->getVelocity().x, 0);
}
