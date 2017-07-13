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

#include "World.h"
#include "macros.h"
#include "GameConstants.h"
#include "Timing.h"
#include "StringUtil.h"
#include "Move.h"
#include "MathUtil.h"
#include "NGRect.h"
#include "SpacePirate.h"
#include "OverlapTester.h"
#include "NetworkManagerServer.h"
#include "NetworkManagerClient.h"
#include "NGAudioEngine.h"
#include "InstanceManager.h"
#include "Util.h"

#include <math.h>

Ground::Ground(b2World& world) : Entity(world, 0.0f, -10.0f, GAME_WIDTH, 20.0f, true)
{
    // Empty
}

void Ground::update()
{
    // Empty
}

void Ground::handleContact(Entity* inEntity)
{
    // Empty
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

NETWORK_TYPE_IMPL(Ground);
