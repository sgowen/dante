//
//  EntityController.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/entity/EntityController.h>

#include <framework/entity/Entity.h>
#include <Box2D/Box2D.h>

IMPL_RTTI_NOPARENT(EntityController);

IMPL_EntityController_create(EntityController);

EntityController::EntityController(Entity* inEntity) : _entity(inEntity)
{
    // Empty
}

EntityController::~EntityController()
{
    // Empty
}

void EntityController::update()
{
    // Empty
}

void EntityController::postUpdate()
{
    // Empty
}

void EntityController::receiveMessage(uint16_t message, void* data)
{
    // Empty
}

void EntityController::onFixturesCreated(std::vector<b2Fixture*>& fixtures)
{
    // Empty
}

bool EntityController::shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    return true;
}

void EntityController::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}

void EntityController::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    // Empty
}
