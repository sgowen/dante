//
//  Entity.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/3/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Entity.h"

#include "NGRect.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"

#include "NGSTDUtil.h"
#include "Timing.h"
#include "macros.h"

Entity::Entity(float x, float y, float width, float height) :
m_fStateTime(0.0f),
m_position(x, y),
m_velocity(),
m_acceleration(),
m_color(1.0f, 1.0f, 1.0f, 1.0f),
m_fWidth(width),
m_fHeight(height),
m_fAngle(0),
m_iID(getUniqueEntityID()),
m_isRequestingDeletion(false)
{
    m_bounds.push_back(new NGRect(x - width / 2, y - height / 2, width, height));
}

Entity::~Entity()
{
    NGSTDUtil::cleanUpVectorOfPointers(m_bounds);
}

void Entity::update(float inDeltaTime)
{
    m_fStateTime += inDeltaTime;
    
    m_velocity.add(m_acceleration.getX() * inDeltaTime, m_acceleration.getY() * inDeltaTime);
    m_position.add(m_velocity.getX() * inDeltaTime, m_velocity.getY() * inDeltaTime);
    
    updateBounds();
}

void Entity::resetBounds(float width, float height)
{
    Vector2 &lowerLeft = m_bounds.at(0)->getLowerLeft();
    lowerLeft.set(m_position.getX() - width / 2, m_position.getY() - height / 2);
    m_bounds.at(0)->setWidth(width);
    m_bounds.at(0)->setHeight(height);
}

void Entity::updateBounds()
{
    Vector2 &lowerLeft = m_bounds.at(0)->getLowerLeft();
    lowerLeft.set(m_position.getX() - m_bounds.at(0)->getWidth() / 2, m_position.getY() - m_bounds.at(0)->getHeight() / 2);
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

float Entity::getStateTime()
{
    return m_fStateTime;
}

void Entity::setStateTime(float stateTime)
{
    m_fStateTime = stateTime;
}

void Entity::setPosition(Vector2 position)
{
    m_position = position;
}

Vector2& Entity::getPosition()
{
    return m_position;
}

Vector2& Entity::getVelocity()
{
    return m_velocity;
}

Vector2& Entity::getAcceleration()
{
    return m_acceleration;
}

std::vector<NGRect *>& Entity::getBounds()
{
    return m_bounds;
}

NGRect& Entity::getMainBounds()
{
    return *m_bounds.at(0);
}

void Entity::setColor(Color color)
{
    m_color = color;
}

Color& Entity::getColor()
{
    return m_color;
}

const float& Entity::getWidth()
{
    return m_fWidth;
}

void Entity::setWidth(float width)
{
    m_fWidth = width;
}

const float& Entity::getHeight()
{
    return m_fHeight;
}

void Entity::setHeight(float height)
{
    m_fHeight = height;
}

void Entity::setAngle(float angle)
{
    m_fAngle = angle;
}

float Entity::getAngle()
{
    return m_fAngle;
}

int Entity::getID()
{
    return m_iID;
}

void Entity::setID(int inID)
{
    m_iID = inID;
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
