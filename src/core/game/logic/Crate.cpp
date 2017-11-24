//
//  Crate.cpp
//  dante
//
//  Created by Stephen Gowen on 7/23/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/Crate.h"

#include "Box2D/Box2D.h"
#include "framework/network/portable/OutputMemoryBitStream.h"
#include "framework/network/portable/InputMemoryBitStream.h"

#include "game/logic/Ground.h"

#include "game/logic/Robot.h"
#include "game/logic/Projectile.h"
#include "game/logic/SpacePirate.h"
#include "framework/network/server/NetworkManagerServer.h"
#include "framework/math/MathUtil.h"

#include <math.h>

Crate::Crate(b2World& world, bool isServer) : Entity(world, 0.0f, 0.0f, 1.0f, 1.0f, isServer, constructEntityDef())
{
    // Empty
}

EntityDef Crate::constructEntityDef()
{
    EntityDef ret = EntityDef();
    
    ret.isStaticBody = false;
    ret.fixedRotation = false;
    ret.restitution = 0.5f;
    ret.density = 8.0f;
    
    return ret;
}

void Crate::update()
{
    if (getPosition().y < DEAD_ZONE_BOTTOM
        || getPosition().x < DEAD_ZONE_LEFT
        || getPosition().x > DEAD_ZONE_RIGHT)
    {
        requestDeletion();
    }
    
    if (_isServer)
    {
        NG_SERVER->setStateDirty(getID(), CRAT_Pose);
    }
    
    _velocityLastKnown = b2Vec2(getVelocity().x, getVelocity().y);
    _positionLastKnown = b2Vec2(getPosition().x, getPosition().y);
    _angleLastKnown = getAngle();
}

bool Crate::shouldCollide(Entity *inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    return true;
}

void Crate::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inEntity->getRTTI().derivesFrom(Ground::rtti)
        || inEntity->getRTTI().derivesFrom(Crate::rtti))
    {
        return;
    }
    
    inEntity->handleBeginContact(this, inFixtureB, inFixtureA);
}

void Crate::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inEntity->getRTTI().derivesFrom(Ground::rtti)
        || inEntity->getRTTI().derivesFrom(Crate::rtti))
    {
        return;
    }
    
    inEntity->handleEndContact(this, inFixtureB, inFixtureA);
}

uint32_t Crate::getAllStateMask() const
{
    return CRAT_AllState;
}

void Crate::read(InputMemoryBitStream& inInputStream)
{
    bool stateBit;
    
    _readState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        b2Vec2 velocity;
        inInputStream.read(velocity);
        setVelocity(velocity);
        
        b2Vec2 position;
        inInputStream.read(position);
        
        float angle;
        inInputStream.read(angle);
        
        setTransform(position, angle);
        
        _readState |= CRAT_Pose;
    }
}

uint32_t Crate::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & CRAT_Pose)
    {
        inOutputStream.write(true);
        
        inOutputStream.write(getVelocity());
        
        inOutputStream.write(getPosition());
        
        inOutputStream.write(getAngle());
        
        writtenState |= CRAT_Pose;
    }
    else
    {
        inOutputStream.write(false);
    }
    
    return writtenState;
}

bool Crate::needsMoveReplay()
{
    return (_readState & CRAT_Pose) != 0;
}

RTTI_IMPL(Crate, Entity);

NW_TYPE_IMPL(Crate);
