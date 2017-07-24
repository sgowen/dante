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

Crate::Crate(b2World& world, bool isServer) : Entity(world, 0.0f, 0.0f, 1.0f, 1.0f, constructEntityDef()), m_isServer(isServer)
{
    // Empty
}

EntityDef Crate::constructEntityDef()
{
    EntityDef ret = EntityDef();
    
    ret.isStaticBody = false;
    ret.fixedRotation = false;
    
    return ret;
}

void Crate::update()
{
    if (getPosition().y < -1
        || getPosition().x < -5
        || getPosition().x > GAME_WIDTH + 5)
    {
        requestDeletion();
    }
    else
    {
        if (m_isServer)
        {
            if (!areBox2DVectorsEqual(m_velocityOld, getVelocity())
                || !areBox2DVectorsEqual(m_positionOld, getPosition()))
            {
                NG_SERVER->setStateDirty(getID(), CRAT_Pose);
            }
            
            m_velocityOld = b2Vec2(getVelocity().x, getVelocity().y);
            m_positionOld = b2Vec2(getPosition().x, getPosition().y);
        }
    }
}

bool Crate::shouldCollide(Entity *inEntity)
{
    return true;
}

void Crate::handleContact(Entity* inEntity)
{
    if (inEntity != this
        && !inEntity->isRequestingDeletion())
    {
        if (inEntity->getRTTI().derivesFrom(Robot::rtti))
        {
            (static_cast<Robot*>(inEntity))->handleContactWithCrate(this);
        }
        else if (inEntity->getRTTI().derivesFrom(Projectile::rtti))
        {
            (static_cast<Projectile*>(inEntity))->handleContactWithCrate(this);
        }
        else if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
        {
            (static_cast<SpacePirate*>(inEntity))->handleContactWithCrate(this);
        }
        else if (inEntity->getRTTI().derivesFrom(Ground::rtti))
        {
            handleContactWithGround(nullptr);
        }
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

void Crate::handleContactWithGround(Ground* inGround)
{
    // TODO
}

RTTI_IMPL(Crate, Entity);

NW_TYPE_IMPL(Crate);
