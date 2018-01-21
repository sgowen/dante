//
//  CrawlerController.cpp
//  dante
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/logic/CrawlerController.h>

#include <framework/entity/Entity.h>
#include "framework/network/portable/InputMemoryBitStream.h"
#include "framework/network/portable/OutputMemoryBitStream.h"
#include "Box2D/Box2D.h"
#include "game/logic/MainInputState.h"
#include "framework/network/portable/Move.h"

#include "game/logic/World.h"
#include "framework/util/macros.h"
#include "game/logic/GameConstants.h"
#include "framework/util/Timing.h"
#include "framework/util/StringUtil.h"
#include "framework/math/MathUtil.h"
#include "framework/network/server/NetworkManagerServer.h"
#include "framework/network/server/ClientProxy.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "game/logic/GameInputManager.h"
#include "framework/audio/portable/NGAudioEngine.h"
#include "game/logic/InstanceManager.h"
#include "framework/network/portable/FWInstanceManager.h"
#include "game/logic/Util.h"
#include "game/logic/Server.h"
#include "framework/util/Config.h"

NGRTTI_IMPL(CrawlerController, EntityController);

EntityController* CrawlerController::create(Entity* inEntity)
{
    return new CrawlerController(inEntity);
}

CrawlerController::CrawlerController(Entity* inEntity) : EntityController(inEntity),
_stats(),
_statsCache(_stats),
_maxXVelocity(4)
{
    // Empty
}

CrawlerController::~CrawlerController()
{
    // Empty
}

uint8_t CrawlerController::update()
{
    if (!isDying() && _stats.health == 0)
    {
        _entity->getPose().state |= State_Dying;
    }
    
    if (_entity->isServer())
    {
        if (isDying() && _entity->getPose().stateTime > 300)
        {
            _entity->requestDeletion();
        }
        
        if (_statsCache != _stats)
        {
            _statsCache = _stats;
            NG_SERVER->setStateDirty(_entity->getID(), ReadStateFlag_Stats);
        }
    }
    
    return isDying() ? State_Dying : isAttacking() ? State_Attacking : isMoving() ? State_Moving : State_Idle;
}

bool CrawlerController::shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Don't collide with other crawlers
    return inEntity->getEntityDef().type != _entity->getEntityDef().type;
}

void CrawlerController::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

void CrawlerController::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

void CrawlerController::read(InputMemoryBitStream& inInputStream, uint16_t& inReadState)
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

void CrawlerController::recallLastReadState(uint16_t& inReadState)
{
    if (inReadState & ReadStateFlag_Stats)
    {
        _stats = _statsCache;
    }
}

uint16_t CrawlerController::write(OutputMemoryBitStream& inOutputStream, uint16_t inWrittenState, uint16_t inDirtyState)
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

void CrawlerController::takeDamage(uint8_t damage)
{
    _stats.health -= damage;
}

uint8_t CrawlerController::getHealth()
{
    return _stats.health;
}

bool CrawlerController::isDying()
{
    return _entity->getPose().state & StateFlag_Dying;
}

bool CrawlerController::isAttacking()
{
    return _entity->getPose().state & StateFlag_Attacking;
}

bool CrawlerController::isMoving()
{
    return !isCloseEnough(_entity->getVelocity().x, 0);
}
