//
//  Entity.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/3/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Entity.h"

#include "Box2D/Box2D.h"
#include "NGRect.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"

#include "NGSTDUtil.h"
#include "Timing.h"
#include "macros.h"

Entity::Entity(b2World& world, float x, float y, float width, float height, EntityDef inEntityDef) :
m_worldRef(world),
m_body(nullptr),
m_fixture(nullptr),
m_fStateTime(0.0f),
m_color(1.0f, 1.0f, 1.0f, 1.0f),
m_fWidth(width),
m_fHeight(height),
m_iID(getUniqueEntityID()),
m_isRequestingDeletion(false)
{
    if (inEntityDef.isStaticBody)
    {
        // Define the ground body.
        b2BodyDef groundBodyDef;
        groundBodyDef.position.Set(x, y);
        
        // Call the body factory which allocates memory for the ground body
        // from a pool and creates the ground box shape (also from a pool).
        // The body is also added to the world.
        m_body = world.CreateBody(&groundBodyDef);
        
        // Define the ground box shape.
        b2PolygonShape groundBox;
        
        // The extents are the half-widths of the box.
        groundBox.SetAsBox(width / 2.0f, height / 2.0f);
        
        // Add the ground fixture to the ground body.
        m_fixture = m_body->CreateFixture(&groundBox, 0.0f);
    }
    else
    {
        // Define the dynamic body. We set its position and call the body factory.
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(x, y);
        bodyDef.fixedRotation = inEntityDef.fixedRotation;
        bodyDef.bullet = inEntityDef.bullet;
        m_body = world.CreateBody(&bodyDef);
        
        // Define another box shape for our dynamic body.
        b2PolygonShape dynamicBox;
        dynamicBox.SetAsBox(width / 2.0f, height / 2.0f);
        
        // Define the dynamic body fixture.
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &dynamicBox;
        fixtureDef.isSensor = inEntityDef.isSensor;
        
        // Set the box density to be non-zero, so it will be dynamic.
        fixtureDef.density = 1.0f;
        
        // Override the default friction.
        fixtureDef.friction = 0.3f;
        
        // Add the shape to the body.
        m_fixture = m_body->CreateFixture(&fixtureDef);
    }
    
    m_fixture->SetUserData(this);
    
    m_body->SetUserData(this);
}

Entity::~Entity()
{
    if (m_fixture)
    {
        m_body->DestroyFixture(m_fixture);
        m_fixture = nullptr;
    }
    
    if (m_body)
    {
        m_worldRef.DestroyBody(m_body);
        m_body = nullptr;
    }
}

void Entity::setStateTime(float stateTime)
{
    m_fStateTime = stateTime;
}

float Entity::getStateTime()
{
    return m_fStateTime;
}

void Entity::setPosition(b2Vec2 position)
{
    m_body->SetTransform(position, m_body->GetAngle());
}

const b2Vec2& Entity::getPosition()
{
    return m_body->GetPosition();
}

void Entity::setVelocity(b2Vec2 velocity)
{
    m_body->SetLinearVelocity(velocity);
}

const b2Vec2& Entity::getVelocity()
{
    return m_body->GetLinearVelocity();
}

void Entity::setColor(Color color)
{
    m_color = color;
}

Color& Entity::getColor()
{
    return m_color;
}

void Entity::setWidth(float width)
{
    m_fWidth = width;
}

const float& Entity::getWidth()
{
    return m_fWidth;
}

void Entity::setHeight(float height)
{
    m_fHeight = height;
}

const float& Entity::getHeight()
{
    return m_fHeight;
}

void Entity::setAngle(float angle)
{
    angle = DEGREES_TO_RADIANS(angle);
    m_body->SetTransform(m_body->GetPosition(), angle);
}

float Entity::getAngle()
{
    return m_body->GetAngle();
}

void Entity::setID(int inID)
{
    m_iID = inID;
}

int Entity::getID()
{
    return m_iID;
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

NW_TYPE_IMPL(Entity);
