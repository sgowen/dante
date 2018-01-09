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
#include <framework/util/FlagUtil.h>
#include <framework/network/server/NetworkManagerServer.h>

NGRTTI_IMPL_NOPARENT(Entity);

Entity::Entity(EntityDef& inEntityDef, b2World& world, bool isServer) :
_entityDef(inEntityDef),
_controller(EntityMapper::getInstance()->createEntityController(inEntityDef.controller, this)),
_worldRef(world),
_isServer(isServer),
_body(NULL),
_fixture(NULL),
_groundSensorFixture(NULL),
_pose(),
_poseCache(_pose),
_readState(0),
_state(0),
_ID(0),
_isRequestingDeletion(false)
{
    b2BodyDef bodyDef;
    bodyDef.position.Set(0, 0);
    bodyDef.type = _entityDef.staticBody ? b2_staticBody : b2_dynamicBody;
    bodyDef.fixedRotation = _entityDef.fixedRotation;
    bodyDef.bullet = _entityDef.bullet;
    
    b2PolygonShape shape;
    shape.SetAsBox(inEntityDef.width / 2, inEntityDef.height / 2);
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.isSensor = _entityDef.sensor;
    fixtureDef.density = _entityDef.density;
    fixtureDef.friction = _entityDef.friction;
    fixtureDef.restitution = _entityDef.restitution;
    
    _body = _worldRef.CreateBody(&bodyDef);
    _body->SetUserData(this);
    
    _fixture = _body->CreateFixture(&fixtureDef);
    _fixture->SetUserData(this);
    
    if (_entityDef.character)
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

void Entity::update()
{
    if (_entityDef.stateSensitive)
    {
        _pose.stateTime += FRAME_RATE;
    }
    
    if (getPosition().y < -5)
    {
        requestDeletion();
    }
    
    _state = _controller->update();
    
    updatePoseFromBody();
    
    if (_isServer)
    {
        if (_poseCache != _pose)
        {
            _poseCache = _pose;
            NG_SERVER->setStateDirty(getID(), ReadStateFlag_Pose);
        }
    }
}

bool Entity::shouldCollide(Entity *inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == _groundSensorFixture)
    {
        return inEntity->getEntityDef().type != _entityDef.type;
    }
    
    return _controller->shouldCollide(inEntity, inFixtureA, inFixtureB);
}

void Entity::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == _groundSensorFixture)
    {
        handleBeginFootContact(inEntity);
    }
    
    _controller->handleBeginContact(inEntity, inFixtureA, inFixtureB);
}

void Entity::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == _groundSensorFixture)
    {
        if (inEntity->getEntityDef().type != _entityDef.type)
        {
            handleEndFootContact(inEntity);
        }
    }
    
    _controller->handleEndContact(inEntity, inFixtureA, inFixtureB);
}

void Entity::read(InputMemoryBitStream& inInputStream)
{
    _readState = 0;
    
    bool stateBit;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        if (_entityDef.stateSensitive)
        {
            inInputStream.read(_pose.stateTime);
            inInputStream.read(_pose.state);
        }
        
        inInputStream.read(_pose.velocity);
        inInputStream.read(_pose.position);
        
        if (!_entityDef.fixedRotation)
        {
            inInputStream.read(_pose.angle);
        }
        
        if (_entityDef.character)
        {
            inInputStream.read<uint8_t, 2>(_pose.numGroundContacts);
        }
        
        inInputStream.read(_pose.isFacingLeft);
        
        updateBodyFromPose();
        
        setFlag(_readState, ReadStateFlag_Pose);
        _poseCache = _pose;
    }
    
    _controller->read(inInputStream, _readState);
}

void Entity::recallLastReadState()
{
    if (isFlagSet(_readState, ReadStateFlag_Pose))
    {
        _pose = _poseCache;
        
        updateBodyFromPose();
    }
    
    _controller->recallLastReadState(_readState);
}

uint16_t Entity::write(OutputMemoryBitStream& inOutputStream, uint16_t inDirtyState)
{
    uint16_t writtenState = 0;
    
    bool pose = isFlagSet(inDirtyState, ReadStateFlag_Pose);
    inOutputStream.write(pose);
    if (pose)
    {
        if (_entityDef.stateSensitive)
        {
            inOutputStream.write(_pose.stateTime);
            inOutputStream.write(_pose.state);
        }
        
        inOutputStream.write(_pose.velocity);
        inOutputStream.write(_pose.position);
        
        if (!_entityDef.fixedRotation)
        {
            inOutputStream.write(_pose.angle);
        }
        
        if (_entityDef.character)
        {
            inOutputStream.write<uint8_t, 2>(_pose.numGroundContacts);
        }
        
        inOutputStream.write(_pose.isFacingLeft);
        
        setFlag(writtenState, ReadStateFlag_Pose);
    }
    
    return _controller->write(inOutputStream, writtenState, inDirtyState);
}

void Entity::deinitPhysics()
{
    if (!_body)
    {
        // Physics already deinitialized
        return;
    }
    
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
    ++_pose.numGroundContacts;
    
    if (_pose.numGroundContacts > 3)
    {
        _pose.numGroundContacts = 0;
    }
}

void Entity::handleEndFootContact(Entity* inEntity)
{
    if (_pose.numGroundContacts == 1)
    {
        _pose.numGroundContacts = 0;
    }
    else
    {
        --_pose.numGroundContacts;
    }
}

EntityDef& Entity::getEntityDef()
{
    return _entityDef;
}

EntityController* Entity::getEntityController()
{
    return _controller;
}

void Entity::setStateTime(float stateTime)
{
    _pose.stateTime = stateTime;
}

float Entity::getStateTime()
{
    return _pose.stateTime;
}

b2Body* Entity::getBody()
{
    return _body;
}

void Entity::setPosition(b2Vec2 position)
{
    _pose.position = position;
    _body->SetTransform(_pose.position, _body->GetAngle());
}

const b2Vec2& Entity::getPosition()
{
    _pose.position = _body->GetPosition();
    return _pose.position;
}

void Entity::setVelocity(b2Vec2 velocity)
{
    _pose.velocity = velocity;
    _body->SetLinearVelocity(_pose.velocity);
}

const b2Vec2& Entity::getVelocity()
{
    _pose.velocity = _body->GetLinearVelocity();
    return _pose.velocity;
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
    _pose.angle = angle;
    _body->SetTransform(_body->GetPosition(), _pose.angle);
}

float Entity::getAngle()
{
    _pose.angle = _body->GetAngle();
    return _pose.angle;
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
    return _pose.numGroundContacts > 0;
}

bool Entity::isFalling()
{
    return _pose.numGroundContacts == 0;
}

void Entity::requestDeletion()
{
    _isRequestingDeletion = true;
}

bool Entity::isRequestingDeletion()
{
    return _isRequestingDeletion;
}

bool Entity::isServer()
{
    return _isServer;
}

bool Entity::isFacingLeft()
{
    return _pose.isFacingLeft;
}

std::string& Entity::getMapping()
{
    std::map<std::string, std::string>& mappings = _entityDef.mappings;
    std::map<uint8_t, std::string>& stateMappings = _controller->getStateMappings();
    auto q = stateMappings.find(_state);
    
    assert(q != stateMappings.end());
    
    std::string& key = q->second;
    
    auto q2 = mappings.find(key);
    
    assert(q2 != mappings.end());
    
    std::string& mapping = q2->second;
    
    return mapping;
}

Entity::Pose& Entity::getPose()
{
    return _pose;
}

void Entity::updatePoseFromBody()
{
    _pose.velocity = getVelocity();
    _pose.position = getPosition();
    
    if (!_entityDef.fixedRotation)
    {
        _pose.angle = getAngle();
    }
}

void Entity::updateBodyFromPose()
{
    setVelocity(_pose.velocity);
    setPosition(_pose.position);
    
    if (!_entityDef.fixedRotation)
    {
        setAngle(_pose.angle);
    }
}
