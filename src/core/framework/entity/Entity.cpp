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
#include "framework/network/portable/OutputMemoryBitStream.h"
#include "framework/network/portable/InputMemoryBitStream.h"
#include "framework/network/portable/Move.h"
#include <framework/entity/EntityController.h>

#include "framework/util/NGSTDUtil.h"
#include "framework/util/Timing.h"
#include "framework/util/macros.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "framework/util/StringUtil.h"
#include "framework/math/MathUtil.h"
#include <framework/entity/EntityMapper.h>

NGRTTI_IMPL_NOPARENT(Entity);

NW_TYPE_IMPL(Entity);

Entity::Entity(EntityDef inEntityDef, b2World& world, bool isServer) :
_entityDef(inEntityDef),
_controller(EntityMapper::getInstance()->createEntityController(inEntityDef.controller, this)),
_worldRef(world),
_body(NULL),
_fixture(NULL),
_footSensorFixture(NULL),
_stateTime(0.0f),
_color(1.0f, 1.0f, 1.0f, 1.0f),
_readState(0),
_isServer(isServer),
_velocityLastKnown(b2Vec2_zero),
_positionLastKnown(b2Vec2_zero),
_angleLastKnown(0.0f),
_numGroundContactsLastKnown(0),
_timeVelocityBecameOutOfSync(0.0f),
_timePositionBecameOutOfSync(0.0f),
_ID(0),
_numGroundContacts(0),
_isRequestingDeletion(false)
{
    b2BodyDef bodyDef;
    bodyDef.position.Set(0, 0);
    
    if (_entityDef.isStaticBody)
    {
        // Call the body factory which allocates memory for the ground body
        // from a pool and creates the ground box shape (also from a pool).
        // The body is also added to the world.
        _body = _worldRef.CreateBody(&bodyDef);
        
        // Define the ground box shape.
        b2PolygonShape groundBox;
        
        // The extents are the half-widths of the box.
        groundBox.SetAsBox(inEntityDef.width / 2, inEntityDef.height / 2);
        
        b2FixtureDef def;
        def.shape = &groundBox;
        def.density = 0.0f;
        def.friction = _entityDef.friction;
        
        // Add the ground fixture to the ground body.
        _fixture = _body->CreateFixture(&def);
    }
    else
    {
        bodyDef.type = b2_dynamicBody;
        bodyDef.fixedRotation = _entityDef.fixedRotation;
        bodyDef.bullet = _entityDef.bullet;
        _body = _worldRef.CreateBody(&bodyDef);
        
        // Define another box shape for our dynamic body.
        b2PolygonShape dynamicBox;
        dynamicBox.SetAsBox(inEntityDef.width / 2, inEntityDef.height / 2);
        
        // Define the dynamic body fixture.
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &dynamicBox;
        fixtureDef.isSensor = _entityDef.isSensor;
        
        // Set the box density to be non-zero, so it will be dynamic.
        fixtureDef.density = _entityDef.density;
        
        // Override the default friction.
        fixtureDef.friction = _entityDef.friction;
        
        fixtureDef.restitution = _entityDef.restitution;
        
        // Add the shape to the body.
        _fixture = _body->CreateFixture(&fixtureDef);
        
        if (_entityDef.isCharacter)
        {
            // Add foot sensor fixture
            b2PolygonShape feet;
            feet.SetAsBox(inEntityDef.width * 0.48f, inEntityDef.height / 8, b2Vec2(0, -inEntityDef.height / 2), 0);
            b2FixtureDef fixtureDefFeet;
            fixtureDefFeet.shape = &feet;
            fixtureDefFeet.isSensor = true;
            _footSensorFixture = _body->CreateFixture(&fixtureDefFeet);
            _footSensorFixture->SetUserData(this);
        }
    }
    
    _fixture->SetUserData(this);
    
    _body->SetUserData(this);
}

Entity::~Entity()
{
    deinitPhysics();
    
    delete _controller;
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

void Entity::deinitPhysics()
{
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

void Entity::handleBeginFootContact(Entity* inEntity)
{
    ++_numGroundContacts;
    
    if (_numGroundContacts > 3)
    {
        _numGroundContacts = 0;
    }
}

void Entity::handleEndFootContact(Entity* inEntity)
{
    if (_numGroundContacts == 1)
    {
        _numGroundContacts = 0;
    }
    else
    {
        --_numGroundContacts;
    }
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
    _body->SetTransform(position, angle);
}

void Entity::setPosition(b2Vec2 position)
{
    _body->SetTransform(position, _body->GetAngle());
}

const b2Vec2& Entity::getPosition()
{
    return _body->GetPosition();
}

void Entity::setVelocity(b2Vec2 velocity)
{
    _body->SetLinearVelocity(velocity);
}

const b2Vec2& Entity::getVelocity()
{
    return _body->GetLinearVelocity();
}

void Entity::setColor(Color color)
{
    _color = color;
}

Color& Entity::getColor()
{
    return _color;
}

float Entity::getWidth()
{
    return _entityDef.width;
}

float Entity::getHeight()
{
    return _entityDef.height;
}

void Entity::setAngle(float angle)
{
    _body->SetTransform(_body->GetPosition(), angle);
}

float Entity::getAngle()
{
    return _body->GetAngle();
}

void Entity::setID(uint32_t inID)
{
    _ID = inID;
}

uint32_t Entity::getID()
{
    return _ID;
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

