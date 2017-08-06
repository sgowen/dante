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
#include "NetworkManagerClient.h"
#include "StringUtil.h"
#include "MathUtil.h"

Entity::Entity(b2World& world, float x, float y, float width, float height, bool isServer, EntityDef inEntityDef) :
m_worldRef(world),
m_body(nullptr),
m_fixture(nullptr),
m_footSensorFixture(nullptr),
m_fStateTime(0.0f),
m_color(1.0f, 1.0f, 1.0f, 1.0f),
m_fWidth(width),
m_fHeight(height),
m_isServer(isServer),
m_velocityLastKnown(),
m_positionLastKnown(),
m_fTimeVelocityBecameOutOfSync(0.0f),
m_fTimePositionBecameOutOfSync(0.0f),
m_iID(getUniqueEntityID()),
m_iNumGroundContacts(0),
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
        fixtureDef.density = inEntityDef.density;
        
        // Override the default friction.
        fixtureDef.friction = 0.3f;
        
        fixtureDef.restitution = inEntityDef.restitution;
        
        // Add the shape to the body.
        m_fixture = m_body->CreateFixture(&fixtureDef);
        
        if (inEntityDef.isCharacter)
        {
            // Add foot sensor fixture
            b2PolygonShape feet;
            feet.SetAsBox(width / 3.0f, height / 8.0f, b2Vec2(x, -height / 2.0f), 0);
            b2FixtureDef fixtureDefFeet;
            fixtureDefFeet.shape = &feet;
            fixtureDefFeet.isSensor = true;
            m_footSensorFixture = m_body->CreateFixture(&fixtureDefFeet);
            m_footSensorFixture->SetUserData(this);
        }
    }
    
    m_fixture->SetUserData(this);
    
    m_body->SetUserData(this);
}

Entity::~Entity()
{
    onDeletion();
}

void Entity::onDeletion()
{
    if (m_fixture)
    {
        m_body->DestroyFixture(m_fixture);
        m_fixture = nullptr;
    }
    
    if (m_footSensorFixture)
    {
        m_body->DestroyFixture(m_footSensorFixture);
        m_footSensorFixture = nullptr;
    }
    
    if (m_body)
    {
        m_worldRef.DestroyBody(m_body);
        m_body = nullptr;
    }
}

void Entity::handleBeginFootContact(Entity* inEntity)
{
    ++m_iNumGroundContacts;
}

void Entity::handleEndFootContact(Entity* inEntity)
{
    --m_iNumGroundContacts;
}

void Entity::setStateTime(float stateTime)
{
    m_fStateTime = stateTime;
}

float Entity::getStateTime()
{
    return m_fStateTime;
}

b2Body* Entity::getBody()
{
    return m_body;
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

bool Entity::isGrounded()
{
    return m_iNumGroundContacts > 0;
}

bool Entity::isFalling()
{
    return m_iNumGroundContacts == 0;
}

void Entity::requestDeletion()
{
    m_isRequestingDeletion = true;
}

bool Entity::isRequestingDeletion()
{
    return m_isRequestingDeletion;
}

#pragma mark protected

void Entity::interpolateClientSidePrediction(b2Vec2& inOldVelocity, b2Vec2& inOldPos)
{
    if (interpolateVectorsIfNecessary(inOldVelocity, getVelocity(), m_fTimeVelocityBecameOutOfSync, "velocity"))
    {
        setVelocity(inOldVelocity);
    }
    
    if (interpolateVectorsIfNecessary(inOldPos, getPosition(), m_fTimePositionBecameOutOfSync, "position"))
    {
        setPosition(inOldPos);
    }
}

bool Entity::interpolateVectorsIfNecessary(b2Vec2& inOld, const b2Vec2& inNew, float& syncTracker, const char* vectorType)
{
    float rtt = NG_CLIENT->getRoundTripTime();
    rtt /= 2; // Let's just measure the time from server to us
    
    if (!areBox2DVectorsEqual(inOld, inNew))
    {
        LOG("WARN: Robot move replay ended with incorrect %s! Old %3.8f, %3.8f - New %3.8f, %3.8f", vectorType, inOld.x, inOld.y, inNew.x, inNew.y);
        
        // have we been out of sync, or did we just become out of sync?
        float time = Timing::getInstance()->getFrameStartTime();
        if (syncTracker == 0.0f)
        {
            syncTracker = time;
        }
        
        // now interpolate to the correct value...
        float durationOutOfSync = time - syncTracker;
        if (durationOutOfSync < rtt)
        {
            b2Vec2 vec = lerpBox2DVector(inOld, inNew, rtt);
            inOld.Set(vec.x, vec.y);
            
            return true;
        }
    }
    else
    {
        syncTracker = 0.0f;
    }
    
    return false;
}

#pragma mark private

int Entity::getUniqueEntityID()
{
    static int entityID = 0;
    
    return entityID++;
}

RTTI_IMPL_NOPARENT(Entity);

NW_TYPE_IMPL(Entity);
