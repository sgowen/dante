//
//  DefaultController.cpp
//  noctisgames
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
    // Empty
}

DefaultController::~DefaultController()
{
    // Empty
}

void DefaultController::update(bool isLive)
{
    // Empty
}

void DefaultController::postUpdate()
{
    // Empty
}

void DefaultController::receiveMessage(uint16_t message, bool isLive, void* data)
{
    // Empty
}

void DefaultController::onFixturesCreated(std::vector<b2Fixture*>& fixtures)
{
    // Empty
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

void DefaultController::recallCache(uint16_t& inReadState)
{
    // Empty
}

uint16_t DefaultController::write(OutputMemoryBitStream& inOutputStream, uint16_t inWrittenState, uint16_t inDirtyState)
{
    return inWrittenState;
}
