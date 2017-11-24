//
//  SpacePirateChunk.cpp
//  dante
//
//  Created by Stephen Gowen on 7/24/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/SpacePirateChunk.h"

#include "Box2D/Box2D.h"
#include "framework/network/portable/OutputMemoryBitStream.h"
#include "framework/network/portable/InputMemoryBitStream.h"

#include "game/logic/Ground.h"

#include "game/logic/Robot.h"
#include "game/logic/Projectile.h"
#include "game/logic/SpacePirate.h"
#include "framework/network/server/NetworkManagerServer.h"
#include "framework/math/MathUtil.h"
#include "framework/util/Timing.h"

#include <math.h>

SpacePirateChunk::SpacePirateChunk(b2World& world, bool isServer) : Entity(world, 0.0f, 0.0f, 1.0f, 1.0f, isServer, constructEntityDef()), _type(Space_Pirate_Chunk_Top_Left), _isFacingLeft(false)
{
    // Empty
}

EntityDef SpacePirateChunk::constructEntityDef()
{
    EntityDef ret = EntityDef();
    
    ret.isStaticBody = false;
    ret.fixedRotation = false;
    ret.isSensor = true;
    ret.restitution = 1.0f;
    ret.density = 8.0f;
    
    return ret;
}

void SpacePirateChunk::update()
{
    _stateTime += FRAME_RATE;
    
    if (_stateTime > 3)
    {
        _color.alpha = (5 - _stateTime) / 2.0f;
        _color.alpha = clamp(_color.alpha, 1, 0);
    }
    
    if (_isServer)
    {
        if (_stateTime > 5)
        {
            requestDeletion();
        }
        
        if (getPosition().y < DEAD_ZONE_BOTTOM
            || getPosition().x < DEAD_ZONE_LEFT
            || getPosition().x > DEAD_ZONE_RIGHT)
        {
            requestDeletion();
        }
        
        NG_SERVER->setStateDirty(getID(), SPCH_Pose);
    }
    
    _velocityLastKnown = b2Vec2(getVelocity().x, getVelocity().y);
    _positionLastKnown = b2Vec2(getPosition().x, getPosition().y);
    _angleLastKnown = getAngle();
}

bool SpacePirateChunk::shouldCollide(Entity *inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    return true;
}

void SpacePirateChunk::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inEntity->getRTTI().derivesFrom(SpacePirateChunk::rtti))
    {
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(Projectile::rtti))
    {
        inEntity->handleBeginContact(this, inFixtureB, inFixtureA);
    }
}

void SpacePirateChunk::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inEntity->getRTTI().derivesFrom(SpacePirateChunk::rtti))
    {
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(Projectile::rtti))
    {
        inEntity->handleEndContact(this, inFixtureB, inFixtureA);
    }
}

uint32_t SpacePirateChunk::getAllStateMask() const
{
    return SPCH_AllState;
}

void SpacePirateChunk::read(InputMemoryBitStream& inInputStream)
{
    float oldWidth = _width;
    
    bool stateBit;
    
    _readState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(_type);
        inInputStream.read(_isFacingLeft);
        
        inInputStream.read(_color);
        
        inInputStream.read(_width);
        inInputStream.read(_height);
        
        _readState |= SPCH_Info;
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
        
        float angle;
        inInputStream.read(angle);
        
        setTransform(position, angle);
        
        _readState |= SPCH_Pose;
    }
    
    if (!areFloatsPracticallyEqual(oldWidth, _width))
    {
        initPhysics(constructEntityDef());
    }
}

uint32_t SpacePirateChunk::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & SPCH_Info)
    {
        inOutputStream.write(true);
        
        inOutputStream.write(_type);
        inOutputStream.write(_isFacingLeft);
        
        inOutputStream.write(_color);
        
        inOutputStream.write(_width);
        inOutputStream.write(_height);
        
        writtenState |= SPCH_Info;
    }
    else
    {
        inOutputStream.write(false);
    }
    
    if (inDirtyState & SPCH_Pose)
    {
        inOutputStream.write(true);
        
        inOutputStream.write(_stateTime);
        
        inOutputStream.write(getVelocity());
        
        inOutputStream.write(getPosition());
        
        inOutputStream.write(getAngle());
        
        writtenState |= SPCH_Pose;
    }
    else
    {
        inOutputStream.write(false);
    }
    
    return writtenState;
}

bool SpacePirateChunk::needsMoveReplay()
{
    return (_readState & SPCH_Pose) != 0;
}

void SpacePirateChunk::initFromSpacePirate(SpacePirate* spacePirate, b2Vec2 force, int type)
{
    float x = spacePirate->getPosition().x;
    float y = spacePirate->getPosition().y;
    float w = spacePirate->getWidth();
    float h = spacePirate->getHeight();
    bool flipX = spacePirate->isFacingLeft();
    _isFacingLeft = flipX;
    _type = type;
    
    switch (type)
    {
        case Space_Pirate_Chunk_Top_Left:
        {
            x += flipX ? (w / 4) : (-w / 4);
            y += h / 4;
        }
            break;
        case Space_Pirate_Chunk_Top_Right:
        {
            x += flipX ? (-w / 4) : (w / 4);
            y += h / 4;
        }
            break;
        case Space_Pirate_Chunk_Botto_Left:
        {
            x += flipX ? (w / 4) : (-w / 4);
            y -= h / 4;
        }
            break;
        case Space_Pirate_Chunk_Botto_Right:
        {
            x += flipX ? (-w / 4) : (w / 4);
            y -= h / 4;
        }
            break;
        default:
            break;
    }
    
    Color spColor = spacePirate->getColor();
    _color = Color(spColor.red, spColor.green, spColor.blue, spColor.alpha);
    
    setPosition(b2Vec2(x, y));
    
    _width = w / 2;
    _height = h / 2;
    
    initPhysics(constructEntityDef());
    
    getBody()->ApplyLinearImpulseToCenter(force, true);
}

int SpacePirateChunk::getType()
{
    return _type;
}

bool SpacePirateChunk::isFacingLeft()
{
    return _isFacingLeft;
}

RTTI_IMPL(SpacePirateChunk, Entity);

NW_TYPE_IMPL(SpacePirateChunk);
