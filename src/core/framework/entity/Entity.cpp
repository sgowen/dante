//
//  Entity.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/3/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Entity.h"

#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"

#include "Timing.h"
#include "macros.h"

Entity::Entity() : m_fStateTime(0.0f), m_isRequestingDeletion(false), m_ID(getUniqueEntityID())
{
    // Empty
}

Entity::~Entity()
{
    // Empty
}

void Entity::update()
{
    float deltaTime = Timing::getInstance()->getDeltaTime();
    
    update(deltaTime);
}

void Entity::update(float deltaTime)
{
    m_fStateTime += deltaTime;
}

void Entity::onDeletion()
{
    // Empty
}

uint32_t Entity::getAllStateMask() const
{
    return 0;
}

void Entity::read(InputMemoryBitStream& inInputStream)
{
    UNUSED(inInputStream);
}

uint32_t Entity::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    UNUSED(inOutputStream);
    UNUSED(inDirtyState);
    
    return 0;
}

int Entity::getID()
{
    return m_ID;
}

void Entity::setID(int inID)
{
    m_ID = inID;
}

float Entity::getStateTime()
{
    return m_fStateTime;
}

void Entity::setStateTime(float stateTime)
{
    m_fStateTime = stateTime;
}

void Entity::requestDeletion()
{
    m_isRequestingDeletion = true;
}

bool Entity::isRequestingDeletion()
{
    return m_isRequestingDeletion;
}

int Entity::getUniqueEntityID()
{
    static int entityID = 0;
    
    return entityID++;
}

RTTI_IMPL_NOPARENT(Entity);

NETWORK_TYPE_IMPL(Entity);
