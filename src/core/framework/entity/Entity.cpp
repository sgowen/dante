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
#include <framework/network/server/NetworkManagerServer.h>

NGRTTI_IMPL_NOPARENT(Entity);

Entity::Entity(EntityDef& inEntityDef, int x, int y, bool isServer) :
_entityDef(inEntityDef),
_controller(EntityMapper::getInstance()->createEntityController(inEntityDef.controller, this)),
_isServer(isServer),
_body(NULL),
_groundSensorFixture(NULL),
_pose(x, y),
_poseCache(_pose),
_poseInterpolateCache(_pose),
_readState(0),
_state(0),
_ID(0),
_deadZoneY(-_entityDef.height / 2.0f),
_isRequestingDeletion(false)
{
    // Empty
}

Entity::~Entity()
{
    delete _controller;
}

void Entity::update()
{
    updatePoseFromBody();
    
    if (_entityDef.stateSensitive)
    {
        ++_pose.stateTime;
    }
    
    if (_isServer)
    {
        if (getPosition().y < _deadZoneY)
        {
            requestDeletion();
        }
    }
    
    _state = _controller->update();
    
    if (_isServer)
    {
        if (_poseCache != _pose)
        {
            _poseCache = _pose;
            NG_SERVER->setStateDirty(getID(), ReadStateFlag_Pose);
        }
    }
}

void Entity::interpolate(double alpha)
{
    if (_body->GetType() == b2_dynamicBody && _body->IsActive())
    {
        float x = _pose.position.x * alpha + _poseInterpolateCache.position.x * (1.0f - alpha);
        float y = _pose.position.y * alpha + _poseInterpolateCache.position.y * (1.0f - alpha);
        float a = _pose.angle * alpha + _poseInterpolateCache.angle * (1.0f - alpha);
        
        _poseInterpolateCache.position = _pose.position;
        _poseInterpolateCache.angle = _pose.angle;
        
        _pose.position.Set(x, y);
        _pose.angle = a;
    }
}

void Entity::postRender()
{
    if (_body->GetType() == b2_dynamicBody && _body->IsActive())
    {
        _pose.position = _poseInterpolateCache.position;
        _pose.angle = _poseInterpolateCache.angle;
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
        _pose.numGroundContacts = clamp(_pose.numGroundContacts + 1, 3, 0);
    }
    
    _controller->handleBeginContact(inEntity, inFixtureA, inFixtureB);
}

void Entity::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == _groundSensorFixture)
    {
        _pose.numGroundContacts = clamp(_pose.numGroundContacts - 1, 3, 0);
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
        
        bool fixedRotation = _entityDef.bodyFlags & BodyFlag_FixedRotation;
        if (!fixedRotation)
        {
            inInputStream.read(_pose.angle);
        }
        
        _poseInterpolateCache.position = _pose.position;
        _poseInterpolateCache.angle = _pose.angle;
        
        if (_groundSensorFixture)
        {
            inInputStream.read<uint8_t, 2>(_pose.numGroundContacts);
        }
        
        inInputStream.read(_pose.isFacingLeft);
        
        updateBodyFromPose();
        
        _readState |= ReadStateFlag_Pose;
    }
    
    _controller->read(inInputStream, _readState);
    
    if (_readState & ReadStateFlag_Pose)
    {
        _poseCache = _pose;
    }
}

void Entity::recallLastReadState()
{
    if (_readState & ReadStateFlag_Pose)
    {
        _pose = _poseCache;
        
        updateBodyFromPose();
    }
    
    _controller->recallLastReadState(_readState);
}

uint16_t Entity::write(OutputMemoryBitStream& inOutputStream, uint16_t inDirtyState)
{
    uint16_t writtenState = 0;
    
    bool pose = inDirtyState & ReadStateFlag_Pose;
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
        
        bool fixedRotation = _entityDef.bodyFlags & BodyFlag_FixedRotation;
        if (!fixedRotation)
        {
            inOutputStream.write(_pose.angle);
        }
        
        if (_groundSensorFixture)
        {
            inOutputStream.write<uint8_t, 2>(_pose.numGroundContacts);
        }
        
        inOutputStream.write(_pose.isFacingLeft);
        
        writtenState |= ReadStateFlag_Pose;
    }
    
    return _controller->write(inOutputStream, writtenState, inDirtyState);
}

void Entity::initPhysics(b2World& world)
{
    assert(!_body);
    
    b2BodyDef bodyDef;
    bodyDef.position.Set(_pose.position.x, _pose.position.y);
    bodyDef.type = _entityDef.bodyFlags & BodyFlag_Static ? b2_staticBody : b2_dynamicBody;
    bodyDef.fixedRotation = _entityDef.bodyFlags & BodyFlag_FixedRotation;
    bodyDef.bullet = _entityDef.bodyFlags & BodyFlag_Bullet;
    _body = world.CreateBody(&bodyDef);
    _body->SetUserData(this);
    
    for (std::vector<FixtureDef>::iterator i = _entityDef.fixtures.begin(); i != _entityDef.fixtures.end(); ++i)
    {
        FixtureDef def = *i;
        b2Shape* shape;
        if (def.flags & FixtureFlag_Circle)
        {
            b2CircleShape circleShape;
            
            circleShape.m_p.Set(def.center.x * _entityDef.width, def.center.y * _entityDef.height);
            circleShape.m_radius = def.vertices[0].x * _entityDef.width;
            
            shape = &circleShape;
        }
        else
        {
            b2PolygonShape polygonShape;
            if (def.flags & FixtureFlag_Box)
            {
                float wFactor = _entityDef.width * def.vertices[0].x;
                float hFactor = _entityDef.height * def.vertices[0].y;
                def.center.Set(def.center.x * _entityDef.width, def.center.y * _entityDef.height);
                
                polygonShape.SetAsBox(wFactor, hFactor, def.center, 0);
            }
            else
            {
                for (std::vector<b2Vec2>::iterator i = def.vertices.begin(); i != def.vertices.end(); ++i)
                {
                    b2Vec2& vertex = (*i);
                    vertex.Set(vertex.x * _entityDef.width, vertex.y * _entityDef.height);
                }
                
                int count = static_cast<int>(def.vertices.size());
                polygonShape.Set(&def.vertices[0], count);
            }
            shape = &polygonShape;
        }
        
        b2FixtureDef fixtureDef;
        fixtureDef.shape = shape;
        fixtureDef.isSensor = def.flags & FixtureFlag_Sensor;
        fixtureDef.density = def.density;
        fixtureDef.friction = def.friction;
        fixtureDef.restitution = def.restitution;
        b2Fixture* fixture = _body->CreateFixture(&fixtureDef);
        fixture->SetUserData(this);
        
        if (def.flags & FixtureFlag_GroundContact)
        {
            _groundSensorFixture = fixture;
        }
        
        _fixtures.push_back(fixture);
    }
}

void Entity::deinitPhysics()
{
    assert(_body);
    
    for (std::vector<b2Fixture*>::iterator i = _fixtures.begin(); i != _fixtures.end(); )
    {
        _body->DestroyFixture(*i);
        
        i = _fixtures.erase(i);
    }
    
    _groundSensorFixture = NULL;
    
    b2World* world = _body->GetWorld();
    world->DestroyBody(_body);
    _body = NULL;
}

void Entity::updatePoseFromBody()
{
    if (_body)
    {
        _pose.velocity = _body->GetLinearVelocity();
        _pose.position = _body->GetPosition();
        _pose.angle = _body->GetAngle();
        
        _poseInterpolateCache.position = _pose.position;
        _poseInterpolateCache.angle = _pose.angle;
    }
}

void Entity::updateBodyFromPose()
{
    if (_body)
    {
        if (!isCloseEnough(_pose.velocity, _body->GetLinearVelocity()))
        {
            _body->SetLinearVelocity(_pose.velocity);
        }
        
        if (!isCloseEnough(_pose.position, _body->GetPosition()) ||
            !isCloseEnough(_pose.angle, _body->GetAngle()))
        {
            _body->SetTransform(_pose.position, _pose.angle);
        }
    }
}

EntityDef& Entity::getEntityDef()
{
    return _entityDef;
}

EntityController* Entity::getController()
{
    return _controller;
}

uint16_t Entity::getStateTime()
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
    
    updateBodyFromPose();
}

const b2Vec2& Entity::getPosition()
{
    return _pose.position;
}

void Entity::setVelocity(b2Vec2 velocity)
{
    _pose.velocity = velocity;
    
    updateBodyFromPose();
}

const b2Vec2& Entity::getVelocity()
{
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
}

float Entity::getAngle()
{
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

std::string& Entity::getTextureMapping()
{
    auto q = _entityDef.textureMappings.find(_state);
    
    assert(q != _entityDef.textureMappings.end());
    
    return q->second;
}

int Entity::getSoundMapping(int state)
{
    auto q = _entityDef.soundMappings.find(state);
    
    if (q != _entityDef.soundMappings.end())
    {
        return q->second;
    }
    
    // No sound for this state
    return 0;
}

Entity::Pose& Entity::getPose()
{
    return _pose;
}

Entity::Pose& Entity::getPoseCache()
{
    return _poseCache;
}
