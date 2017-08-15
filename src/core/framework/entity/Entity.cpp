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
#include "Move.h"

#include "NGSTDUtil.h"
#include "Timing.h"
#include "macros.h"
#include "NetworkManagerClient.h"
#include "StringUtil.h"
#include "MathUtil.h"

Entity::Entity(b2World& world, float x, float y, float width, float height, bool isServer, EntityDef inEntityDef, bool autoInitPhysics) :
m_worldRef(world),
m_body(nullptr),
m_fixture(nullptr),
m_footSensorFixture(nullptr),
m_fStateTime(0.0f),
m_color(1.0f, 1.0f, 1.0f, 1.0f),
m_fX(x),
m_fY(y),
m_fWidth(width),
m_fHeight(height),
m_fAngle(0),
m_iReadState(0),
m_isServer(isServer),
m_isPhysicsOn(false),
m_velocityLastKnown(b2Vec2_zero),
m_positionLastKnown(b2Vec2_zero),
m_fAngleLastKnown(0.0f),
m_iNumGroundContactsLastKnown(0),
m_fTimeVelocityBecameOutOfSync(0.0f),
m_fTimePositionBecameOutOfSync(0.0f),
m_iID(getUniqueEntityID()),
m_iNumGroundContacts(0),
m_isRequestingDeletion(false)
{
    if (autoInitPhysics)
    {
        initPhysics(inEntityDef);
    }
}

Entity::~Entity()
{
    onDeletion();
}

void Entity::recallIfNecessary(Move* move)
{
    if (!needsMoveReplay())
    {
        move->recallEntityCache(this);
    }
}

void Entity::postRead()
{
    // Only interpolate when new pose has been read in
    if (needsMoveReplay())
    {
        interpolateClientSidePrediction(m_velocityLastKnown, m_positionLastKnown);
    }
    
    m_iReadState = 0;
}

void Entity::cacheToMove(const Move& move)
{
    move.cacheEntity(this);
}

void Entity::onDeletion()
{
    deinitPhysics();
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
    m_fX = position.x;
    m_fY = position.y;
    
    if (m_isPhysicsOn)
    {
        m_body->SetTransform(position, m_body->GetAngle());
    }
}

const b2Vec2& Entity::getPosition()
{
    return m_isPhysicsOn ? m_body->GetPosition() : b2Vec2_zero;
}

void Entity::setVelocity(b2Vec2 velocity)
{
    if (m_isPhysicsOn)
    {
        m_body->SetLinearVelocity(velocity);
    }
}

const b2Vec2& Entity::getVelocity()
{
    return m_isPhysicsOn ? m_body->GetLinearVelocity() : b2Vec2_zero;
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
    if (!areFloatsPracticallyEqual(m_fAngle, angle))
    {
        if (m_isPhysicsOn)
        {
            angle = DEGREES_TO_RADIANS(angle);
            m_body->SetTransform(m_body->GetPosition(), angle);
        }
    }
    
    m_fAngle = angle;
}

float Entity::getAngle()
{
    return m_isPhysicsOn ? m_body->GetAngle() : m_fAngle;
}

void Entity::setID(uint32_t inID)
{
    m_iID = inID;
}

uint32_t Entity::getID()
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

void Entity::initPhysics(EntityDef inEntityDef)
{
    deinitPhysics();
    
    if (inEntityDef.isStaticBody)
    {
        // Define the ground body.
        b2BodyDef groundBodyDef;
        groundBodyDef.position.Set(m_fX, m_fY);
        
        // Call the body factory which allocates memory for the ground body
        // from a pool and creates the ground box shape (also from a pool).
        // The body is also added to the world.
        m_body = m_worldRef.CreateBody(&groundBodyDef);
        
        // Define the ground box shape.
        b2PolygonShape groundBox;
        
        // The extents are the half-widths of the box.
        groundBox.SetAsBox(m_fWidth / 2.0f, m_fHeight / 2.0f);
        
        // Add the ground fixture to the ground body.
        m_fixture = m_body->CreateFixture(&groundBox, 0.0f);
    }
    else
    {
        // Define the dynamic body. We set its position and call the body factory.
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(m_fX, m_fY);
        bodyDef.fixedRotation = inEntityDef.fixedRotation;
        bodyDef.bullet = inEntityDef.bullet;
        m_body = m_worldRef.CreateBody(&bodyDef);
        
        // Define another box shape for our dynamic body.
        b2PolygonShape dynamicBox;
        dynamicBox.SetAsBox(m_fWidth / 2.0f, m_fHeight / 2.0f);
        
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
            feet.SetAsBox(m_fWidth / 3.0f, m_fHeight / 8.0f, b2Vec2(m_fX, -m_fHeight / 2.0f), 0);
            b2FixtureDef fixtureDefFeet;
            fixtureDefFeet.shape = &feet;
            fixtureDefFeet.isSensor = true;
            m_footSensorFixture = m_body->CreateFixture(&fixtureDefFeet);
            m_footSensorFixture->SetUserData(this);
        }
    }
    
    m_fixture->SetUserData(this);
    
    m_body->SetUserData(this);
    
    m_isPhysicsOn = true;
}

void Entity::deinitPhysics()
{
    m_isPhysicsOn = false;
    
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
    if (!areBox2DVectorsCloseEnough(inOld, inNew))
    {
        LOG("WARN: %s move replay ended with incorrect %s! Old %3.8f, %3.8f - New %3.8f, %3.8f", getRTTI().getClassName().c_str(), vectorType, inOld.x, inOld.y, inNew.x, inNew.y);
        
        // have we been out of sync, or did we just become out of sync?
        float time = Timing::getInstance()->getFrameStartTime();
        if (syncTracker == 0.0f)
        {
            syncTracker = time;
        }
        
        float rtt = NG_CLIENT->getRoundTripTime();
        
        // now interpolate to the correct value...
        float durationOutOfSync = time - syncTracker;
        if (durationOutOfSync < rtt)
        {
            b2Vec2 vec = lerpBox2DVector(inOld, inNew, 0.1f);
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

uint32_t Entity::getUniqueEntityID()
{
    static uint32_t entityID = 0;
    
    return entityID++;
}

RTTI_IMPL_NOPARENT(Entity);

NW_TYPE_IMPL(Entity);
