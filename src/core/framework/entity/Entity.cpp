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
_groundSensorFixture(NULL),
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
    bodyDef.type = _entityDef.isStaticBody ? b2_staticBody : b2_dynamicBody;
    bodyDef.fixedRotation = _entityDef.fixedRotation;
    bodyDef.bullet = _entityDef.bullet;
    
    b2PolygonShape shape;
    shape.SetAsBox(inEntityDef.width / 2, inEntityDef.height / 2);
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.isSensor = _entityDef.isSensor;
    fixtureDef.density = _entityDef.density;
    fixtureDef.friction = _entityDef.friction;
    fixtureDef.restitution = _entityDef.restitution;
    
    _body = _worldRef.CreateBody(&bodyDef);
    _body->SetUserData(this);
    
    _fixture = _body->CreateFixture(&fixtureDef);
    _fixture->SetUserData(this);
    
    if (_entityDef.isCharacter)
    {
        b2PolygonShape groundContact;
        groundContact.SetAsBox(inEntityDef.width * 0.48f, inEntityDef.height / 8, b2Vec2(0, -inEntityDef.height / 2), 0);
        b2FixtureDef fixtureDefGroundContact;
        fixtureDefGroundContact.shape = &groundContact;
        fixtureDefGroundContact.isSensor = true;
        _groundSensorFixture = _body->CreateFixture(&fixtureDefGroundContact);
        _groundSensorFixture->SetUserData(this);
    }
}

Entity::~Entity()
{
    deinitPhysics();
    
    delete _controller;
}

void Entity::recallLastReadState()
{
    // TODO
}

void Entity::postRead()
{
    // Only interpolate when new pose has been read in
    if (needsMoveReplay())
    {
        interpolateClientSidePrediction(_velocityLastKnown, _positionLastKnown);
    }
}

void Entity::deinitPhysics()
{
    if (_fixture)
    {
        _body->DestroyFixture(_fixture);
        _fixture = NULL;
    }
    
    if (_groundSensorFixture)
    {
        _body->DestroyFixture(_groundSensorFixture);
        _groundSensorFixture = NULL;
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

