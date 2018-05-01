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
#include <Box2D/Box2D.h>

#include <framework/studio/World.h>
#include <framework/util/macros.h>
#include <framework/util/Constants.h>
#include <framework/util/Timing.h>
#include <framework/util/StringUtil.h>
#include <framework/util/MathUtil.h>
#include <framework/network/server/NetworkManagerServer.h>
#include <game/game/GameInputManager.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <game/game/Util.h>
#include <game/game/Server.h>
#include <game/config/GameConfig.h>
#include <game/entity/PlayerController.h>
#include <framework/util/Config.h>
#include <game/game/GameEngine.h>

IMPL_RTTI(BasicFollowAndAttackController, EntityController);

IMPL_EntityController_create(BasicFollowAndAttackController);

BasicFollowAndAttackController::BasicFollowAndAttackController(Entity* e) : EntityController(e),
_stats(),
_statsNetworkCache(_stats),
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

void BasicFollowAndAttackController::update()
{
    if (_entity->getNetworkController()->isServer())
    {
        uint8_t state = _entity->getState().state;
        switch (state)
        {
            case State_Idle:
                handleIdleState();
                break;
            case State_Moving:
                handleMovingState();
                break;
            case State_Attacking:
                handleAttackingState();
                break;
            case State_Dying:
                handleDyingState();
                break;
            default:
                break;
        }
    }
}

void BasicFollowAndAttackController::receiveMessage(uint16_t message, void* data)
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
    
    if (GameEngine::getInstance()->isLive())
    {
        Util::handleSound(_entity, fromState, state);
    }
}

void BasicFollowAndAttackController::onFixturesCreated(std::vector<b2Fixture*>& fixtures)
{
    assert(fixtures.size() >= 2);
    _attackSensorFixture = fixtures[1];
}

bool BasicFollowAndAttackController::shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Don't collide with other crawlers
    return inEntity->getEntityDef().type != _entity->getEntityDef().type &&
    _entity->getState().state != State_Dying;
}

void BasicFollowAndAttackController::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (_entity->getNetworkController()->isServer() &&
        inFixtureA == _attackSensorFixture &&
        !inFixtureB->IsSensor() &&
        inEntity->getController()->getRTTI().derivesFrom(PlayerController::rtti))
    {
        _targetTouching = inEntity->getID();
    }
}

void BasicFollowAndAttackController::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (_entity->getNetworkController()->isServer() &&
        inFixtureA == _attackSensorFixture &&
        !inFixtureB->IsSensor() &&
        inEntity->getController()->getRTTI().derivesFrom(PlayerController::rtti))
    {
        _targetTouching = 0;
    }
}

void BasicFollowAndAttackController::handleIdleState()
{
    World* world = Server::getInstance()->getWorld();
    assert(world);
    
    std::vector<Entity*>& players = world->getPlayers();
    
    float shortestDistance = FW_CFG->_camWidth / 3;
    
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

void BasicFollowAndAttackController::handleMovingState()
{
    EntityManager* em = NG_SERVER->getEntityManager();
    uint8_t& state = _entity->getState().state;
    uint16_t& stateTime = _entity->getState().stateTime;
    if (_targetTouching &&
        em->getEntityByID(_targetTouching))
    {
        state = State_Attacking;
        stateTime = 0;
        return;
    }
    
    Entity* target = em->getEntityByID(_target);
    if (target)
    {
        float distance = b2Distance(target->getPosition(), target->getPosition());
        if (distance > FW_CFG->_camWidth / 3)
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

void BasicFollowAndAttackController::handleAttackingState()
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
            targetTouching->getController()->receiveMessage(ENTITY_MESSAGE_DAMAGE, &damage);
        }
    }
    else
    {
        state = State_Idle;
        stateTime = 0;
    }
}

void BasicFollowAndAttackController::handleDyingState()
{
    if (_entity->getState().stateTime >= 180)
    {
        _entity->requestDeletion();
    }
}

#include <framework/network/portable/InputMemoryBitStream.h>
#include <framework/network/portable/OutputMemoryBitStream.h>

IMPL_RTTI(BasicFollowAndAttackNetworkController, EntityNetworkController);

IMPL_EntityNetworkController_create(BasicFollowAndAttackNetworkController);

BasicFollowAndAttackNetworkController::BasicFollowAndAttackNetworkController(Entity* e, bool isServer) : EntityNetworkController(e, isServer), _controller(static_cast<BasicFollowAndAttackController*>(e->getController()))
{
    // Empty
}

BasicFollowAndAttackNetworkController::~BasicFollowAndAttackNetworkController()
{
    // Empty
}

void BasicFollowAndAttackNetworkController::read(InputMemoryBitStream& ip)
{
    uint8_t fromState = _entity->getStateNetworkCache().state;
    
    EntityNetworkController::read(ip);
    
    BasicFollowAndAttackController& c = *_controller;
    
    bool stateBit;
    
    ip.read(stateBit);
    if (stateBit)
    {
        ip.read(c._stats.health);
        
        c._statsNetworkCache = c._stats;
    }
    
    Util::handleSound(_entity, fromState, _entity->getState().state);
}

uint16_t BasicFollowAndAttackNetworkController::write(OutputMemoryBitStream& op, uint16_t dirtyState)
{
    uint16_t writtenState = EntityNetworkController::write(op, dirtyState);
    
    BasicFollowAndAttackController& c = *_controller;
    
    bool stats = dirtyState & BasicFollowAndAttackController::ReadStateFlag_Stats;
    op.write(stats);
    if (stats)
    {
        op.write(c._stats.health);
        
        writtenState |= BasicFollowAndAttackController::ReadStateFlag_Stats;
    }
    
    return writtenState;
}

void BasicFollowAndAttackNetworkController::recallNetworkCache()
{
    EntityNetworkController::recallNetworkCache();
    
    BasicFollowAndAttackController& c = *_controller;
    
    c._stats = c._statsNetworkCache;
}

uint16_t BasicFollowAndAttackNetworkController::getDirtyState()
{
    uint16_t ret = EntityNetworkController::getDirtyState();
    
    BasicFollowAndAttackController& c = *_controller;
    
    if (c._statsNetworkCache != c._stats)
    {
        c._statsNetworkCache = c._stats;
        ret |= BasicFollowAndAttackController::ReadStateFlag_Stats;
    }
    
    return ret;
}
