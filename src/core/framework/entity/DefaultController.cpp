//
//  DefaultController.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/entity/DefaultController.h>

NGRTTI_IMPL(DefaultController, EntityController);

EntityController* DefaultController::create(Entity* inEntity)
{
    return new DefaultController(inEntity);
}

DefaultController::DefaultController(Entity* inEntity) : EntityController(inEntity)
{
    _stateMappings.insert(std::make_pair(0, "Idle"));
}

DefaultController::~DefaultController()
{
    // Empty
}

uint8_t DefaultController::update()
{
    return 0;
}

bool DefaultController::shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    return true;
}

void DefaultController::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

void DefaultController::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

void DefaultController::read(InputMemoryBitStream& inInputStream, uint16_t& inReadState)
{
    // Empty
}

void DefaultController::recallLastReadState(uint16_t& inReadState)
{
    // Empty
}

uint16_t DefaultController::write(OutputMemoryBitStream& inOutputStream, uint16_t inWrittenState, uint16_t inDirtyState)
{
    return inWrittenState;
}
