//
//  Projectile.cpp
//  dante
//
//  Created by Stephen Gowen on 6/4/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/Projectile.h"

#include "Box2D/Box2D.h"
#include "framework/network/portable/OutputMemoryBitStream.h"
#include "framework/network/portable/InputMemoryBitStream.h"
#include "game/logic/Robot.h"
#include "game/logic/SpacePirate.h"
#include "game/logic/SpacePirateChunk.h"
#include "game/logic/Ground.h"
#include "game/logic/Crate.h"

#include "game/logic/World.h"
#include "framework/util/macros.h"
#include "game/logic/GameConstants.h"
#include "framework/util/Timing.h"
#include "framework/util/StringUtil.h"
#include "framework/network/portable/Move.h"
#include "framework/math/MathUtil.h"
#include "game/logic/SpacePirate.h"
#include "framework/math/OverlapTester.h"
#include "framework/network/server/NetworkManagerServer.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "framework/audio/portable/NGAudioEngine.h"
#include "game/logic/InstanceManager.h"
#include "game/logic/Util.h"

#include <math.h>

NGRTTI_IMPL(Projectile, Entity);

NW_TYPE_IMPL(Projectile);

Projectile::Projectile(b2World& world, bool isServer) : Entity(world, 0, 0, 1.565217391304348f * 0.444444444444444f, 2.0f * 0.544423440453686f, isServer, constructEntityDef(), false),
_playerId(0),
_isFacingLeft(false),
_hasMadeContact(false),
_state(ProjectileState_Waiting),
_stateLastKnown(ProjectileState_Waiting)
{
    // Empty
}

EntityDef Projectile::constructEntityDef()
{
    EntityDef ret = EntityDef();
    
    ret.isStaticBody = false;
    ret.bullet = true;
    ret.restitution = 1.0f;
    ret.density = 1.0f;
    
    return ret;
}

void Projectile::update()
{
    _stateTime += FRAME_RATE;
    
    if (_state == ProjectileState_Active)
    {
        if (_hasMadeContact)
        {
            _hasMadeContact = false;
            
            explode();
        }
        else if (_stateTime > 0.25f)
        {
            explode();
        }
        else if (getPosition().y < DEAD_ZONE_BOTTOM
            || getPosition().x < DEAD_ZONE_LEFT
            || getPosition().x > DEAD_ZONE_RIGHT)
        {
            explode();
        }
    }
    else if (_state == ProjectileState_Exploding)
    {
        if (_stateTime > 0.5f)
        {
            _stateTime = 0.0f;
            _state = ProjectileState_Waiting;
            
            deinitPhysics();
        }
    }
    
    if (_isServer)
    {
        NG_SERVER->setStateDirty(getID(), PRJC_Pose);
    }
    else
    {
        if (_stateLastKnown == ProjectileState_Exploding
                 && _state == ProjectileState_Waiting)
        {
            deinitPhysics();
        }
    }
    
    _stateLastKnown = _state;
    _velocityLastKnown = b2Vec2(getVelocity().x, getVelocity().y);
    _positionLastKnown = b2Vec2(getPosition().x, getPosition().y);
}

bool Projectile::shouldCollide(Entity *inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (_state != ProjectileState_Active)
    {
        return false;
    }
    
    return inEntity->getRTTI().derivesFrom(SpacePirate::rtti)
    || inEntity->getRTTI().derivesFrom(SpacePirateChunk::rtti)
    || inEntity->getRTTI().derivesFrom(Crate::rtti)
    || inEntity->getRTTI().derivesFrom(Ground::rtti);
}

void Projectile::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (_state != ProjectileState_Active)
    {
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(Projectile::rtti))
    {
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
    {
        handleBeginContactWithSpacePirate(static_cast<SpacePirate*>(inEntity));
    }
    
    _hasMadeContact = true;
}

void Projectile::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

uint32_t Projectile::getAllStateMask() const
{
    return PRJC_AllState;
}

void Projectile::read(InputMemoryBitStream& inInputStream)
{
    bool stateBit;
    
    _readState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_playerId);
        inInputStream.read(_color);
        _readState |= PRJC_PlayerInfo;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        uint8_t state;
        inInputStream.read<uint8_t, 2>(state);
        _state = static_cast<ProjectileState>(state);
        
        inInputStream.read(_stateTime);
        
        inInputStream.read(_isFacingLeft);
        
        b2Vec2 velocity;
        inInputStream.read(velocity);
        setVelocity(velocity);
        
        b2Vec2 position;
        inInputStream.read(position);
        setPosition(position);
        
        _readState |= PRJC_Pose;
    }
}

uint32_t Projectile::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & PRJC_PlayerInfo)
    {
        inOutputStream.write(true);
        inOutputStream.write(_playerId);
        inOutputStream.write(_color);
        
        writtenState |= PRJC_PlayerInfo;
    }
    else
    {
        inOutputStream.write(false);
    }
    
    if (inDirtyState & PRJC_Pose)
    {
        inOutputStream.write(true);
        
        inOutputStream.write<uint8_t, 2>(static_cast<uint8_t>(_state));
        
        inOutputStream.write(_stateTime);
        
        inOutputStream.write(_isFacingLeft);
        
        inOutputStream.write(getVelocity());
        
        inOutputStream.write(getPosition());
        
        writtenState |= PRJC_Pose;
    }
    else
    {
        inOutputStream.write(false);
    }
    
    return writtenState;
}

bool Projectile::needsMoveReplay()
{
    return false;
}

void Projectile::initFromShooter(Robot* inRobot)
{
    _playerId = inRobot->getPlayerId();
    
    setColor(inRobot->getColor());
}

void Projectile::fire(Robot* inRobot)
{
    _stateTime = 0.0f;
    _hasMadeContact = false;
    _state = ProjectileState_Active;
    
    _isFacingLeft = inRobot->isFacingLeft();
    
    b2Vec2 position = inRobot->getPosition();
    position += b2Vec2(0, inRobot->getHeight() / 5);
    setPosition(position);
    
    initPhysics(constructEntityDef());
    
    setVelocity(b2Vec2(_isFacingLeft ? -80 : 80, 0));
}

Projectile::ProjectileState Projectile::getState()
{
    return _state;
}

void Projectile::setPlayerId(uint32_t inPlayerId)
{
    _playerId = inPlayerId;
}

uint32_t Projectile::getPlayerId() const
{
    return _playerId;
}

bool Projectile::isFacingLeft()
{
    return _isFacingLeft;
}

void Projectile::handleBeginContactWithSpacePirate(SpacePirate* inEntity)
{
    float projBottom = getPosition().y - (_height / 2.0f);
    float targTop = inEntity->getPosition().y + (inEntity->getHeight() / 2.0f);
    float targHead = targTop - inEntity->getHeight() * 0.4f;
    
    bool isHeadshot = projBottom > targHead;
    
    inEntity->takeDamage(b2Vec2(getVelocity().x, getVelocity().y), isHeadshot);
    if (inEntity->getHealth() == 0)
    {
        World* world = _isServer ? InstanceManager::getServerWorld() : InstanceManager::getClientWorld();
        Robot* robot = world->getRobotWithPlayerId(getPlayerId());
        if (robot)
        {
            robot->awardKill(isHeadshot);
        }
    }
}

void Projectile::explode()
{
    if (_state == ProjectileState_Exploding)
    {
        return;
    }
    
    _state = ProjectileState_Exploding;
    _stateTime = 0.0f;
    _hasMadeContact = false;
    
    setVelocity(b2Vec2_zero);
    
    if (_isPhysicsOn)
    {
        _body->SetGravityScale(0);
    }
    
    World* world = _isServer ? InstanceManager::getServerWorld() : InstanceManager::getClientWorld();
    Robot* robot = world->getRobotWithPlayerId(getPlayerId());
    if (robot && robot->isPending())
    {
        Util::playSound(SOUND_ID_EXPLOSION, getPosition(), _isServer);
    }
}

