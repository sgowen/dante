//
//  DefaultEntityController.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "DefaultEntityController.h"

NGRTTI_IMPL(DefaultEntityController, EntityController);

EntityController* DefaultEntityController::create(Entity* inEntity)
{
    return new DefaultEntityController(inEntity);
}

DefaultEntityController::DefaultEntityController(Entity* inEntity) : EntityController(inEntity)
{
    // Empty
}

DefaultEntityController::~DefaultEntityController()
{
    // Empty
}

void DefaultEntityController::update()
{
    //
}

bool DefaultEntityController::shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    return true;
}

void DefaultEntityController::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    //
}

void DefaultEntityController::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    //
}

uint32_t DefaultEntityController::getAllStateMask() const
{
    return 0;
}

void DefaultEntityController::read(InputMemoryBitStream& inInputStream)
{
    //
}

uint32_t DefaultEntityController::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    return 0;
}

bool DefaultEntityController::needsMoveReplay()
{
    return false;
}
