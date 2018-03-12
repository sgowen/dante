//
//  BasicFollowAndAttackController.cpp
//  dante
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/entity/BasicFollowAndAttackController.h>

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
#include <game/logic/GameConfig.h>
#include <game/entity/PlayerController.h>

NGRTTI_IMPL(BasicFollowAndAttackController, EntityController);

EntityController* BasicFollowAndAttackController::create(Entity* inEntity)
{
    return new BasicFollowAndAttackController(inEntity);
}

BasicFollowAndAttackController::BasicFollowAndAttackController(Entity* inEntity) : EntityController(inEntity),
_stats(),
_statsCache(_stats),
_attackSensorFixture(NULL),
_target(0),
_targetTouching(0)
{
    // Empty
}

BasicFollowAndAttackController::~BasicFollowAndAttackController()
{
    // Empty
}

void BasicFollowAndAttackController::update(bool isLive)
{
    if (_entity->isServer())
    {
        uint8_t state = _entity->getState().state;
        switch (state)
        {
            case State_Idle:
                handleIdleState(isLive);
                break;
            case State_Moving:
                handleMovingState(isLive);
                break;
            case State_Attacking:
                handleAttackingState(isLive);
                break;
            case State_Dying:
                handleDyingState(isLive);
                break;
            default:
                break;
        }
    }
}

void BasicFollowAndAttackController::postUpdate()
{
    uint8_t state = _entity->getState().state;
    uint8_t stateTime = _entity->getState().stateTime;
    
    if (state == State_Dying && stateTime >= 120)
    {
        _entity->requestDeletion();
    }
    
    if (_statsCache != _stats)
    {
        _statsCache = _stats;
        NG_SERVER->setStateDirty(_entity->getID(), ReadStateFlag_Stats);
    }
}

void BasicFollowAndAttackController::receiveMessage(uint16_t message, bool isLive, void* data)
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
            damage = clamp(damage, health, 0);
            health -= damage;
            if (health == 0 && state != State_Dying)
            {
                state = State_Dying;
                stateTime = 0;
            }
        }
            break;
        default:
            break;
    }
    
    if (isLive)
    {
        Util::handleSound(_entity, fromState, state);
    }
}

void BasicFollowAndAttackController::onFixturesCreated(std::vector<b2Fixture*>& fixtures)
{
    assert(fixtures.size() == 2);
    _attackSensorFixture = fixtures[1];
    assert(_attackSensorFixture);
}

bool BasicFollowAndAttackController::shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Don't collide with other crawlers
    return inEntity->getEntityDef().type != _entity->getEntityDef().type &&
    _entity->getState().state != State_Dying;
}

void BasicFollowAndAttackController::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (_entity->isServer() &&
        inFixtureA == _attackSensorFixture &&
        !inFixtureB->IsSensor() &&
        inEntity->getController()->getRTTI().derivesFrom(PlayerController::rtti))
    {
        _targetTouching = inEntity->getID();
    }
}

void BasicFollowAndAttackController::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (_entity->isServer() &&
        inFixtureA == _attackSensorFixture &&
        !inFixtureB->IsSensor() &&
        inEntity->getController()->getRTTI().derivesFrom(PlayerController::rtti))
    {
        _targetTouching = 0;
    }
}

void BasicFollowAndAttackController::read(InputMemoryBitStream& inInputStream, uint16_t& inReadState)
{
    bool stateBit;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_stats.health);
        
        inReadState |= ReadStateFlag_Stats;
        _statsCache = _stats;
    }
    
    Util::handleSound(_entity, _entity->getStateCache().state, _entity->getState().state);
}

void BasicFollowAndAttackController::recallLastReadState(uint16_t& inReadState)
{
    _stats = _statsCache;
}

uint16_t BasicFollowAndAttackController::write(OutputMemoryBitStream& inOutputStream, uint16_t inWrittenState, uint16_t inDirtyState)
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

void BasicFollowAndAttackController::handleIdleState(bool isLive)
{
    World* world = GM_CFG->getWorld();
    assert(world);
    
    std::vector<Entity*>& players = world->getPlayers();
    
    float shortestDistance = GM_CFG->_camWidth / 3;
    
    for (Entity* e : players)
    {
        float distance = b2Distance(e->getPosition(), _entity->getPosition());
        if (distance < shortestDistance)
        {
            shortestDistance = distance;
            _target = e->getID();
        }
    }
    
    uint8_t& state = _entity->getState().state;
    uint16_t& stateTime = _entity->getState().stateTime;
    
    if (_target)
    {
        state = State_Moving;
        stateTime = 0;
    }
}

void BasicFollowAndAttackController::handleMovingState(bool isLive)
{
    uint8_t& state = _entity->getState().state;
    uint16_t& stateTime = _entity->getState().stateTime;
    if (_targetTouching &&
        NG_SERVER->getEntityManager()->getEntityByID(_targetTouching))
    {
        state = State_Attacking;
        stateTime = 0;
        return;
    }
    
    Entity* target = NG_SERVER->getEntityManager()->getEntityByID(_target);
    if (target)
    {
        float distance = b2Distance(target->getPosition(), target->getPosition());
        if (distance > GM_CFG->_camWidth / 3)
        {
            _target = 0;
            state = State_Idle;
            stateTime = 0;
            
            return;
        }
    }
    else
    {
        _target = 0;
        state = State_Idle;
        stateTime = 0;
        
        return;
    }
    
    bool isRight = target->getPosition().x > _entity->getPosition().x;
    bool isLeft = target->getPosition().x < _entity->getPosition().x;
    
    static const int LEFT = 0;
    static const int STOP = 1;
    static const int RIGHT = 2;
    
    int moveState = STOP;
    if (isRight)
    {
        moveState = RIGHT;
    }
    else if (isLeft)
    {
        moveState = LEFT;
    }
    
    const b2Vec2& vel = _entity->getVelocity();
    float desiredVel = 0;
    switch (moveState)
    {
        case LEFT:
            desiredVel = b2Max(vel.x - 1, -GM_CFG->_maxCrawlerVelocityX);
            break;
        case STOP:
            desiredVel = vel.x * 0.99f;
            break;
        case RIGHT:
            desiredVel = b2Min(vel.x + 1, GM_CFG->_maxCrawlerVelocityX);
            break;
    }
    
    float velChange = desiredVel - vel.x;
    float impulse = _entity->getBody()->GetMass() * velChange;
    
    _entity->getPose().isFacingLeft = moveState == LEFT ? true : moveState == RIGHT ? false : _entity->getPose().isFacingLeft;
    _entity->updateBodyFromPose();
    
    if (!isCloseEnough(impulse, 0))
    {
        _entity->getBody()->ApplyLinearImpulse(b2Vec2(impulse, 0), _entity->getBody()->GetWorldCenter(), true);
    }
}

void BasicFollowAndAttackController::handleAttackingState(bool isLive)
{
    uint8_t& state = _entity->getState().state;
    uint16_t& stateTime = _entity->getState().stateTime;
    
    Entity* targetTouching = NG_SERVER->getEntityManager()->getEntityByID(_targetTouching);
    if (targetTouching)
    {
        uint16_t animTime = stateTime % 30;
        if (animTime == 24)
        {
            uint32_t damage = 1;
            targetTouching->getController()->receiveMessage(ENTITY_MESSAGE_DAMAGE, isLive, &damage);
        }
    }
    else
    {
        state = State_Idle;
        stateTime = 0;
    }
}

void BasicFollowAndAttackController::handleDyingState(bool isLive)
{
    // Empty
}
