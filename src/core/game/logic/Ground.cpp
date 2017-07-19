//
//  Ground.cpp
//  dante
//
//  Created by Stephen Gowen on 7/13/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Ground.h"

#include "Box2D/Box2D.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"

#include "Robot.h"
#include "Projectile.h"
#include "SpacePirate.h"

#include <math.h>

Ground::Ground(b2World& world) : Entity(world, GAME_WIDTH / 2.0f, -10.0f, GAME_WIDTH, 23.6f, constructEntityDef())
{
    // Empty
}

EntityDef Ground::constructEntityDef()
{
    return EntityDef();
}

void Ground::update()
{
    // Empty
}

bool Ground::shouldCollide(Entity *inEntity)
{
    return true;
}

void Ground::handleContact(Entity* inEntity)
{
    if (inEntity != this
        && !inEntity->isRequestingDeletion())
    {
        if (inEntity->getRTTI().derivesFrom(Robot::rtti))
        {
            (static_cast<Robot*>(inEntity))->handleContactWithGround(this);
        }
        else if (inEntity->getRTTI().derivesFrom(Projectile::rtti))
        {
            (static_cast<Projectile*>(inEntity))->handleContactWithGround(this);
        }
        else if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
        {
            (static_cast<SpacePirate*>(inEntity))->handleContactWithGround(this);
        }
    }
}

uint32_t Ground::getAllStateMask() const
{
    return 0;
}

void Ground::read(InputMemoryBitStream& inInputStream)
{
    // Empty
}

uint32_t Ground::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    // Empty
    
    return writtenState;
}

RTTI_IMPL(Ground, Entity);

NW_TYPE_IMPL(Ground);
