//
//  Entity.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/3/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/entity/Entity.h"

#include "Box2D/Box2D.h"
#include "framework/math/NGRect.h"
#include "framework/network/portable/OutputMemoryBitStream.h"
#include "framework/network/portable/InputMemoryBitStream.h"
#include "framework/network/portable/Move.h"

#include "framework/util/NGSTDUtil.h"
#include "framework/util/Timing.h"
#include "framework/util/macros.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "framework/util/StringUtil.h"
#include "framework/math/MathUtil.h"

Entity::Entity(b2World& world, float x, float y, float width, float height, bool isServer, EntityDef inEntityDef, bool autoInitPhysics) :
_worldRef(world),
_body(NULL),
_fixture(NULL),
_footSensorFixture(NULL),
_stateTime(0.0f),
_color(1.0f, 1.0f, 1.0f, 1.0f),
_x(x),
_y(y),
_width(width),
_height(height),
_angle(0),
_readState(0),
_isServer(isServer),
_isPhysicsOn(false),
_velocityLastKnown(b2Vec2_zero),
_positionLastKnown(b2Vec2_zero),
_angleLastKnown(0.0f),
_numGroundContactsLastKnown(0),
_timeVelocityBecameOutOfSync(0.0f),
_timePositionBecameOutOfSync(0.0f),
_iD(getUniqueEntityID()),
_numGroundContacts(0),
_isRequestingDeletion(false)
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
        interpolateClientSidePrediction(_velocityLastKnown, _positionLastKnown);
    }
    
    _readState = 0;
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
    ++_numGroundContacts;
}

void Entity::handleEndFootContact(Entity* inEntity)
{
    _numGroundContacts = 0;
}

void Entity::setStateTime(float stateTime)
{
    _stateTime = stateTime;
}

float Entity::getStateTime()
{
    return _stateTime;
}

b2Body* Entity::getBody()
{
    return _body;
}

void Entity::setTransform(b2Vec2 position, float angle)
{
    _x = position.x;
    _y = position.y;
    _angle = angle;
    
    if (_isPhysicsOn)
    {
        _body->SetTransform(position, _angle);
    }
}

void Entity::setPosition(b2Vec2 position)
{
    _x = position.x;
    _y = position.y;
    
    if (_isPhysicsOn)
    {
        _body->SetTransform(position, _body->GetAngle());
    }
}

const b2Vec2& Entity::getPosition()
{
    return _isPhysicsOn ? _body->GetPosition() : b2Vec2_zero;
}

void Entity::setVelocity(b2Vec2 velocity)
{
    if (_isPhysicsOn)
    {
        _body->SetLinearVelocity(velocity);
    }
}

const b2Vec2& Entity::getVelocity()
{
    return _isPhysicsOn ? _body->GetLinearVelocity() : b2Vec2_zero;
}

void Entity::setColor(Color color)
{
    _color = color;
}

Color& Entity::getColor()
{
    return _color;
}

void Entity::setWidth(float width)
{
    _width = width;
}

const float& Entity::getWidth()
{
    return _width;
}

void Entity::setHeight(float height)
{
    _height = height;
}

const float& Entity::getHeight()
{
    return _height;
}

void Entity::setAngle(float angle)
{
    if (_isPhysicsOn)
    {
        if (!areFloatsPracticallyEqual(_angle, angle))
        {
            _body->SetTransform(_body->GetPosition(), angle);
        }
    }
    
    _angle = angle;
}

float Entity::getAngle()
{
    return _isPhysicsOn ? _body->GetAngle() : _angle;
}

void Entity::setID(uint32_t inID)
{
    _iD = inID;
}

uint32_t Entity::getID()
{
    return _iD;
}

bool Entity::isGrounded()
{
    return _numGroundContacts > 0;
}

bool Entity::isFalling()
{
    return _numGroundContacts == 0;
}

void Entity::requestDeletion()
{
    _isRequestingDeletion = true;
}

bool Entity::isRequestingDeletion()
{
    return _isRequestingDeletion;
}

#pragma mark protected

void Entity::initPhysics(EntityDef inEntityDef)
{
    deinitPhysics();
    
    if (inEntityDef.isStaticBody)
    {
        // Define the ground body.
        b2BodyDef groundBodyDef;
        groundBodyDef.position.Set(_x, _y);
        
        // Call the body factory which allocates memory for the ground body
        // from a pool and creates the ground box shape (also from a pool).
        // The body is also added to the world.
        _body = _worldRef.CreateBody(&groundBodyDef);
        
        // Define the ground box shape.
        b2PolygonShape groundBox;
        
        // The extents are the half-widths of the box.
        groundBox.SetAsBox(_width / 2.0f, _height / 2.0f);
        
        // Add the ground fixture to the ground body.
        _fixture = _body->CreateFixture(&groundBox, 0.0f);
    }
    else
    {
        // Define the dynamic body. We set its position and call the body factory.
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(_x, _y);
        bodyDef.fixedRotation = inEntityDef.fixedRotation;
        bodyDef.bullet = inEntityDef.bullet;
        _body = _worldRef.CreateBody(&bodyDef);
        
        // Define another box shape for our dynamic body.
        b2PolygonShape dynamicBox;
        dynamicBox.SetAsBox(_width / 2.0f, _height / 2.0f);
        
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
        _fixture = _body->CreateFixture(&fixtureDef);
        
        if (inEntityDef.isCharacter)
        {
            // Add foot sensor fixture
            b2PolygonShape feet;
            feet.SetAsBox(_width / 3.0f, _height / 8.0f, b2Vec2(_x, -_height / 2.0f), 0);
            b2FixtureDef fixtureDefFeet;
            fixtureDefFeet.shape = &feet;
            fixtureDefFeet.isSensor = true;
            _footSensorFixture = _body->CreateFixture(&fixtureDefFeet);
            _footSensorFixture->SetUserData(this);
        }
    }
    
    _fixture->SetUserData(this);
    
    _body->SetUserData(this);
    
    _isPhysicsOn = true;
}

void Entity::deinitPhysics()
{
    _isPhysicsOn = false;
    
    if (_fixture)
    {
        _body->DestroyFixture(_fixture);
        _fixture = NULL;
    }
    
    if (_footSensorFixture)
    {
        _body->DestroyFixture(_footSensorFixture);
        _footSensorFixture = NULL;
    }
    
    if (_body)
    {
        _worldRef.DestroyBody(_body);
        _body = NULL;
    }
}

void Entity::interpolateClientSidePrediction(b2Vec2& inOldVelocity, b2Vec2& inOldPos)
{
    if (interpolateVectorsIfNecessary(inOldVelocity, getVelocity(), _timeVelocityBecameOutOfSync, "velocity"))
    {
        setVelocity(inOldVelocity);
    }
    
    if (interpolateVectorsIfNecessary(inOldPos, getPosition(), _timePositionBecameOutOfSync, "position"))
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
