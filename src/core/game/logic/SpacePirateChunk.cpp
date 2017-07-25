//
//  SpacePirateChunk.cpp
//  dante
//
//  Created by Stephen Gowen on 7/24/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SpacePirateChunk.h"

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

SpacePirateChunk::SpacePirateChunk(b2World& world, bool isServer) : Entity(world, 0.0f, 0.0f, 1.0f, 1.0f, constructEntityDef()), m_isServer(isServer)
{
    // Empty
}

EntityDef SpacePirateChunk::constructEntityDef()
{
    EntityDef ret = EntityDef();
    
    ret.isStaticBody = false;
    ret.fixedRotation = false;
    ret.restitution = 0.1f;
    ret.density = 16.0f;
    
    return ret;
}

void SpacePirateChunk::update()
{
    if (getPosition().y < DEAD_ZONE_BOTTOM
        || getPosition().x < DEAD_ZONE_LEFT
        || getPosition().x > DEAD_ZONE_RIGHT)
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

bool SpacePirateChunk::shouldCollide(Entity *inEntity)
{
    return true;
}

void SpacePirateChunk::handleContact(Entity* inEntity)
{
    if (inEntity != this
        && !inEntity->isRequestingDeletion())
    {
        // TODO
    }
}

uint32_t SpacePirateChunk::getAllStateMask() const
{
    return CRAT_AllState;
}

void SpacePirateChunk::read(InputMemoryBitStream& inInputStream)
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

uint32_t SpacePirateChunk::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
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

void SpacePirateChunk::handleContactWithGround(Ground* inGround)
{
    // TODO
}

RTTI_IMPL(SpacePirateChunk, Entity);

NW_TYPE_IMPL(SpacePirateChunk);
