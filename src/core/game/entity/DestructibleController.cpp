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
#include <Box2D/Box2D.h>

#include <framework/studio/World.h>
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
#include <game/game/Util.h>
#include <game/game/Server.h>
#include <framework/util/Config.h>
#include <game/game/GameEngine.h>

IMPL_RTTI(DestructibleController, EntityController);

IMPL_EntityController_create(DestructibleController);

DestructibleController::DestructibleController(Entity* e) : EntityController(e),
_stats(),
_statsNetworkCache(_stats)
{
    // Empty
}

DestructibleController::~DestructibleController()
{
    // Empty
}

void DestructibleController::update()
{
    if (_entity->getNetworkController()->isServer())
    {
        uint8_t state = _entity->getState().state;
        switch (state)
        {
            case State_Undamaged:
                // Empty
                break;
            case State_OneThirdDamaged:
                // Empty
                break;
            case State_TwoThirdsDamaged:
                // Empty
                break;
            case State_Destructing:
                if (_entity->getState().stateTime >= 180)
                {
                    _entity->requestDeletion();
                }
                break;
            default:
                break;
        }
    }
}

void DestructibleController::receiveMessage(uint16_t message, void* data)
{
    uint8_t fromState = _entity->getState().state;
    uint8_t& state = _entity->getState().state;
    uint16_t& stateTime = _entity->getState().stateTime;
    
    switch (message)
    {
        case ENTITY_MESSAGE_DAMAGE:
        {
            uint8_t& health = _stats.health;
            
            uint32_t* damageP = static_cast<uint32_t*>(data);
            uint32_t& damage = *damageP;
            damage = clamp(damage, 0, health);
            health -= damage;
            if (health == 2 && state != State_OneThirdDamaged)
            {
                state = State_OneThirdDamaged;
                stateTime = 0;
            }
            else if (health == 1 && state != State_TwoThirdsDamaged)
            {
                state = State_TwoThirdsDamaged;
                stateTime = 0;
            }
            else if (health == 0 && state != State_Destructing)
            {
                state = State_Destructing;
                stateTime = 0;
            }
        }
            break;
        default:
            break;
    }
    
    if (GameEngine::getInstance()->isLive())
    {
        Util::handleSound(_entity, fromState, state);
    }
}

void DestructibleController::onFixturesCreated(std::vector<b2Fixture*>& fixtures)
{
    // Empty
}

bool DestructibleController::shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    return _entity->getState().state != State_Destructing;
}

void DestructibleController::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

void DestructibleController::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

#include <framework/network/portable/InputMemoryBitStream.h>
#include <framework/network/portable/OutputMemoryBitStream.h>

IMPL_RTTI(DestructibleNetworkController, EntityNetworkController);

IMPL_EntityNetworkController_create(DestructibleNetworkController);

DestructibleNetworkController::DestructibleNetworkController(Entity* e, bool isServer) : EntityNetworkController(e, isServer), _controller(static_cast<DestructibleController*>(e->getController()))
{
    // Empty
}

DestructibleNetworkController::~DestructibleNetworkController()
{
    // Empty
}

void DestructibleNetworkController::read(InputMemoryBitStream& ip)
{
    EntityNetworkController::read(ip);
    
    DestructibleController& c = *_controller;
    
    bool stateBit;
    
    ip.read(stateBit);
    if (stateBit)
    {
        ip.read(c._stats.health);
        
        c._statsNetworkCache = c._stats;
    }
}

uint16_t DestructibleNetworkController::write(OutputMemoryBitStream& op, uint16_t dirtyState)
{
    uint16_t writtenState = EntityNetworkController::write(op, dirtyState);
    
    DestructibleController& c = *_controller;
    
    bool stats = dirtyState & DestructibleController::ReadStateFlag_Stats;
    op.write(stats);
    if (stats)
    {
        op.write(c._stats.health);
        
        writtenState |= DestructibleController::ReadStateFlag_Stats;
    }
    
    return writtenState;
}

void DestructibleNetworkController::recallNetworkCache()
{
    EntityNetworkController::recallNetworkCache();
    
    DestructibleController& c = *_controller;
    
    c._stats = c._statsNetworkCache;
}

uint16_t DestructibleNetworkController::getDirtyState()
{
    uint16_t ret = EntityNetworkController::getDirtyState();
    
    DestructibleController& c = *_controller;
    
    if (c._statsNetworkCache != c._stats)
    {
        c._statsNetworkCache = c._stats;
        ret |= DestructibleController::ReadStateFlag_Stats;
    }
    
    return ret;
}
