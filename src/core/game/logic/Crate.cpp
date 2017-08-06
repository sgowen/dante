//
//  Crate.cpp
//  dante
//
//  Created by Stephen Gowen on 7/23/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Crate.h"

#include "Box2D/Box2D.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"

#include "Ground.h"

#include "Robot.h"
#include "Projectile.h"
#include "SpacePirate.h"
#include "NetworkManagerServer.h"
#include "MathUtil.h"

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
    ret.density = 16.0f;
    
    return ret;
}

void Crate::update()
{
    if (m_isServer)
    {
        if (getPosition().y < DEAD_ZONE_BOTTOM
            || getPosition().x < DEAD_ZONE_LEFT
            || getPosition().x > DEAD_ZONE_RIGHT)
        {
            requestDeletion();
        }
        
        if (!areBox2DVectorsEqual(m_velocityLastKnown, getVelocity())
            || !areBox2DVectorsEqual(m_positionLastKnown, getPosition()))
        {
            NG_SERVER->setStateDirty(getID(), CRAT_Pose);
        }
    }
    
    m_velocityLastKnown = b2Vec2(getVelocity().x, getVelocity().y);
    m_positionLastKnown = b2Vec2(getPosition().x, getPosition().y);
}

bool Crate::shouldCollide(Entity *inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inEntity->getRTTI().derivesFrom(Projectile::rtti))
    {
        return (static_cast<Projectile*>(inEntity))->getState() == Projectile::ProjectileState_Active;
    }
    
    return true;
}

void Crate::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inEntity->getRTTI().derivesFrom(Robot::rtti))
    {
        (static_cast<Robot*>(inEntity))->handleBeginContact(this, inFixtureB, inFixtureA);
    }
    else if (inEntity->getRTTI().derivesFrom(Projectile::rtti))
    {
        (static_cast<Projectile*>(inEntity))->handleBeginContactWithCrate(this);
    }
    else if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
    {
        (static_cast<SpacePirate*>(inEntity))->handleBeginContact(this, inFixtureB, inFixtureA);
    }
    else if (inEntity->getRTTI().derivesFrom(Ground::rtti))
    {
        handleBeginContactWithGround(nullptr);
    }
}

void Crate::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inEntity->getRTTI().derivesFrom(Robot::rtti))
    {
        (static_cast<Robot*>(inEntity))->handleEndContact(this, inFixtureB, inFixtureA);
    }
    else if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
    {
        (static_cast<SpacePirate*>(inEntity))->handleEndContact(this, inFixtureB, inFixtureA);
    }
}

uint32_t Crate::getAllStateMask() const
{
    return CRAT_AllState;
}

void Crate::read(InputMemoryBitStream& inInputStream)
{
    bool stateBit;
    
    uint32_t readState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        b2Vec2 velocity;
        inInputStream.read(velocity);
        setVelocity(velocity);
        
        b2Vec2 position;
        inInputStream.read(position);
        setPosition(position);
        
        readState |= CRAT_Pose;
    }
}

uint32_t Crate::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & CRAT_Pose)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(getVelocity());
        
        inOutputStream.write(getPosition());
        
        writtenState |= CRAT_Pose;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    return writtenState;
}

void Crate::handleBeginContactWithGround(Ground* inGround)
{
    // TODO
}

RTTI_IMPL(Crate, Entity);

NW_TYPE_IMPL(Crate);
