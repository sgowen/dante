//
//  Ground.cpp
//  dante
//
//  Created by Stephen Gowen on 7/13/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/Ground.h"

#include "Box2D/Box2D.h"
#include "framework/network/portable/OutputMemoryBitStream.h"
#include "framework/network/portable/InputMemoryBitStream.h"

#include "game/logic/Robot.h"
#include "game/logic/Crate.h"
#include "framework/network/server/NetworkManagerServer.h"

#include <math.h>

NGRTTI_IMPL(Ground, Entity);

NW_TYPE_IMPL(Ground);

namespace
{
    inline EntityDef constructEntityDef()
    {
        EntityDef ret;
        
        ret.width = 32.0f;
        ret.height = 4.0f;
        ret.friction = 32.0f;
        
        return ret;
    }
}

Ground::Ground(b2World& world, bool isServer) : Entity(constructEntityDef(), world, isServer)
{
    // Empty
}

void Ground::update()
{
    if (_isServer)
    {
        NG_SERVER->setStateDirty(getID(), GRND_Pose);
    }
    
    _velocityLastKnown.Set(getVelocity().x, getVelocity().y);
    _positionLastKnown.Set(getPosition().x, getPosition().y);
}

bool Ground::shouldCollide(Entity *inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    return true;
}

void Ground::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

void Ground::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

uint32_t Ground::getAllStateMask() const
{
    return GRND_AllState;
}

void Ground::read(InputMemoryBitStream& inInputStream)
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
        setPosition(position);
        
        _readState |= GRND_Pose;
    }
}

uint32_t Ground::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    bool pose = inDirtyState & GRND_Pose;
    inOutputStream.write(pose);
    if (pose)
    {
        inOutputStream.write(getVelocity());
        
        inOutputStream.write(getPosition());
        
        writtenState |= GRND_Pose;
    }
    
    return writtenState;
}

bool Ground::needsMoveReplay()
{
    return (_readState & GRND_Pose) != 0;
}
