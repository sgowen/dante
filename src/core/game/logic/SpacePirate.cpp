//
//  SpacePirate.cpp
//  dante
//
//  Created by Stephen Gowen on 6/4/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/SpacePirate.h"

#include "Box2D/Box2D.h"
#include "framework/network/portable/OutputMemoryBitStream.h"
#include "framework/network/portable/InputMemoryBitStream.h"
#include "game/logic/Robot.h"
#include "game/logic/Ground.h"
#include "game/logic/Crate.h"

#include "game/logic/World.h"
#include "framework/util/macros.h"
#include "game/logic/GameConstants.h"
#include "framework/util/Timing.h"
#include "framework/util/StringUtil.h"
#include "framework/network/portable/Move.h"
#include "framework/math/MathUtil.h"
#include "framework/math/NGRect.h"
#include "framework/math/OverlapTester.h"
#include "framework/network/server/NetworkManagerServer.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "game/network/InstanceManager.h"
#include "game/logic/Projectile.h"
#include "game/logic/Util.h"
#include "framework/entity/EntityRegistry.h"
#include "framework/network/portable/FWInstanceManager.h"
#include "game/logic/SpacePirateChunk.h"

#include <math.h>

SpacePirate::SpacePirate(b2World& world, bool isServer) : Entity(world, 0, 0, 2.0f, 2.347826086956522f, isServer, constructEntityDef()),
_speed(0.0),
_health(8),
_isFacingLeft(false),
_isJumping(false),
_jumpCooldown(0.0f),
_jumpSpeed(7.0f),
_healthLeftLastKnown(8)
{
    _jumpCooldown = Timing::getInstance()->getFrameStartTime();
    _jumpCooldown += (rand() % 100) * 0.1f + 1.0f;
}

EntityDef SpacePirate::constructEntityDef()
{
    EntityDef ret = EntityDef();
    
    ret.isStaticBody = false;
    ret.restitution = 0.1f;
    ret.isCharacter = true;
    ret.density = 32.0f;
    
    return ret;
}

void SpacePirate::update()
{
    _stateTime += FRAME_RATE;
    
    if (_isServer)
    {
        if (getPosition().y < DEAD_ZONE_BOTTOM)
        {
            _health = 0;
        }
        
        if (_health == 0 && !isRequestingDeletion())
        {
            requestDeletion();
            
            for (int i = 0; i < 4; ++i)
            {
                SpacePirateChunk* chunk = static_cast<SpacePirateChunk*>(SERVER_ENTITY_REG->createEntity(NW_TYPE_SpacePirateChunk));
                chunk->initFromSpacePirate(this, _lastForce, i);
            }
        }
        
        if (isGrounded())
        {
            _jumpCooldown -= FRAME_RATE;
            
            if (_jumpCooldown < 0)
            {
                _jumpCooldown += (rand() % 100) * 0.1f + 1.0f;
                
                _isJumping = true;
                
                setVelocity(b2Vec2(getVelocity().x, _jumpSpeed));
            }
        }
        
        bool targetFound = false;
        
        float shortestDistance = GAME_WIDTH * 3;
        std::vector<Entity*> players = InstanceManager::getServerWorld()->getPlayers();
        for (Entity* entity : players)
        {
            Robot* robot = static_cast<Robot*>(entity);
            
            if (!robot->isRequestingDeletion())
            {
                float dist = b2Distance(robot->getPosition(), getPosition());
                if (dist < shortestDistance)
                {
                    shortestDistance = dist;
                    _isFacingLeft = robot->getPosition().x < getPosition().x;
                    
                    setVelocity(b2Vec2(_isFacingLeft ? -_speed : _speed, getVelocity().y));
                    
                    targetFound = true;
                }
            }
        }
        
        if (!targetFound)
        {
            _isFacingLeft = true;
            
            setVelocity(b2Vec2(0, getVelocity().y));
        }
        
        NG_SERVER->setStateDirty(getID(), SPCP_Pose);
        
        if (_healthLeftLastKnown != _health)
        {
            NG_SERVER->setStateDirty(getID(), SPCP_Health);
        }
    }
    
    _velocityLastKnown = b2Vec2(getVelocity().x, getVelocity().y);
    _positionLastKnown = b2Vec2(getPosition().x, getPosition().y);
    _healthLeftLastKnown = _health;
}

bool SpacePirate::shouldCollide(Entity *inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == _footSensorFixture)
    {
        return inEntity->getRTTI().derivesFrom(Ground::rtti)
        || inEntity->getRTTI().derivesFrom(Crate::rtti);
    }
    
    return inEntity->getRTTI().derivesFrom(Robot::rtti)
    || inEntity->getRTTI().derivesFrom(Projectile::rtti)
    || inEntity->getRTTI().derivesFrom(Ground::rtti)
    || inEntity->getRTTI().derivesFrom(Crate::rtti);
}

void SpacePirate::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == _footSensorFixture)
    {
        _isJumping = false;
        
        handleBeginFootContact(inEntity);
        
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
    {
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(Robot::rtti)
        || inEntity->getRTTI().derivesFrom(Projectile::rtti))
    {
        inEntity->handleBeginContact(this, inFixtureB, inFixtureA);
    }
}

void SpacePirate::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == _footSensorFixture)
    {
        if (inEntity->getRTTI().derivesFrom(Ground::rtti)
            || inEntity->getRTTI().derivesFrom(Crate::rtti))
        {
            handleEndFootContact(inEntity);
        }
        
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
    {
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(Robot::rtti)
        || inEntity->getRTTI().derivesFrom(Projectile::rtti))
    {
        inEntity->handleEndContact(this, inFixtureB, inFixtureA);
    }
}

uint32_t SpacePirate::getAllStateMask() const
{
    return SPCP_AllState;
}

void SpacePirate::read(InputMemoryBitStream& inInputStream)
{
    float oldWidth = _width;
    
    bool stateBit;
    
    _readState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_color);
        
        inInputStream.read(_width);
        inInputStream.read(_height);
        _readState |= SPCP_Info;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_stateTime);
        
        b2Vec2 velocity;
        inInputStream.read(velocity);
        setVelocity(velocity);
        
        b2Vec2 position;
        inInputStream.read(position);
        setPosition(position);
        
        inInputStream.read(_isFacingLeft);
        inInputStream.read(_isJumping);
        
        _readState |= SPCP_Pose;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_health);
        _readState |= SPCP_Health;
    }
    
    if (!areFloatsPracticallyEqual(oldWidth, _width))
    {
        initPhysics(constructEntityDef());
    }
}

uint32_t SpacePirate::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & SPCP_Info)
    {
        inOutputStream.write(true);
        
        inOutputStream.write(_color);
        
        inOutputStream.write(_width);
        inOutputStream.write(_height);
        
        writtenState |= SPCP_Info;
    }
    else
    {
        inOutputStream.write(false);
    }
    
    if (inDirtyState & SPCP_Pose)
    {
        inOutputStream.write(true);
        
        inOutputStream.write(_stateTime);
        
        inOutputStream.write(getVelocity());
        
        inOutputStream.write(getPosition());
        
        inOutputStream.write(_isFacingLeft);
        inOutputStream.write(_isJumping);
        
        writtenState |= SPCP_Pose;
    }
    else
    {
        inOutputStream.write(false);
    }
    
    if (inDirtyState & SPCP_Health)
    {
        inOutputStream.write(true);
        
        inOutputStream.write(_health);
        
        writtenState |= SPCP_Health;
    }
    else
    {
        inOutputStream.write(false);
    }
    
    return writtenState;
}

bool SpacePirate::needsMoveReplay()
{
    return (_readState & SPCP_Pose) != 0;
}

void SpacePirate::init(float x, float y, float speed, int scale, uint8_t health)
{
    setPosition(b2Vec2(x, y));
    
    _speed = speed;
    
    _width *= scale;
    _height *= scale;
    
    _health = health;
    
    initPhysics(constructEntityDef());
}

void SpacePirate::takeDamage(b2Vec2 force, bool isHeadshot)
{
    _lastForce.Set(force.x, force.y);
    uint8_t oldHealth = _health;
    _health -= isHeadshot ? 8 : 1;
    if (_health > oldHealth)
    {
        // We are using unsigned values for health
        _health = 0;
    }
}

uint8_t SpacePirate::getHealth()
{
    return _health;
}

float SpacePirate::getSpeed()
{
    return _speed;
}

bool SpacePirate::isFacingLeft()
{
    return _isFacingLeft;
}

RTTI_IMPL(SpacePirate, Entity);

NW_TYPE_IMPL(SpacePirate);
