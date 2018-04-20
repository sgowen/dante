//
//  Entity.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 8/3/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/entity/Entity.h>

#include <framework/entity/EntityController.h>
#include <framework/entity/EntityNetworkController.h>
#include <Box2D/Box2D.h>

#include <framework/entity/EntityMapper.h>

IMPL_RTTI_NOPARENT(Entity);

Entity::Entity(EntityDef inEntityDef) :
_entityDef(inEntityDef),
_controller(EntityMapper::getInstance()->createEntityController(inEntityDef, this)),
_networkController(EntityMapper::getInstance()->createEntityNetworkController(inEntityDef, this)),
_body(NULL),
_groundSensorFixture(NULL),
_pose(inEntityDef.x, inEntityDef.y),
_poseNetworkCache(_pose),
_poseInterpolateCache(_pose),
_ID(inEntityDef.ID),
_deadZoneY(-_entityDef.height / 2.0f),
_isRequestingDeletion(false),
_isBodyFacingLeft(false)
{
    // Empty
}

Entity::~Entity()
{
    delete _controller;
    delete _networkController;
}

void Entity::update()
{
    updatePoseFromBody();
    
    if (getPosition().y < _deadZoneY)
    {
        requestDeletion();
    }
    
    if (_entityDef.stateSensitive)
    {
        ++_state.stateTime;
    }
    
    _controller->update();
}

void Entity::interpolate(double alpha)
{
    float x = _pose.position.x * alpha + _poseInterpolateCache.position.x * (1.0f - alpha);
    float y = _pose.position.y * alpha + _poseInterpolateCache.position.y * (1.0f - alpha);
    float a = _pose.angle * alpha + _poseInterpolateCache.angle * (1.0f - alpha);
    
    _poseInterpolateCache.position = _pose.position;
    _poseInterpolateCache.angle = _pose.angle;
    
    _pose.position.Set(x, y);
    _pose.angle = a;
}

void Entity::endInterpolation()
{
    _pose.position = _poseInterpolateCache.position;
    _pose.angle = _poseInterpolateCache.angle;
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
    if (inFixtureA == _groundSensorFixture &&
        !inFixtureB->IsSensor())
    {
        _pose.numGroundContacts = clamp(_pose.numGroundContacts + 1, 0, 15);
    }
    
    _controller->handleBeginContact(inEntity, inFixtureA, inFixtureB);
}

void Entity::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == _groundSensorFixture &&
        !inFixtureB->IsSensor())
    {
        _pose.numGroundContacts = clamp(_pose.numGroundContacts - 1, 0, 15);
    }
    
    _controller->handleEndContact(inEntity, inFixtureA, inFixtureB);
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
    
    createFixtures();
}

void Entity::deinitPhysics()
{
    assert(_body);
    
    destroyFixtures();
    
    b2World* world = _body->GetWorld();
    world->DestroyBody(_body);
    _body = NULL;
}

void Entity::updatePoseFromBody()
{
    if (!_body)
    {
        return;
    }
    
    _pose.velocity = _body->GetLinearVelocity();
    _pose.position = _body->GetPosition();
    _pose.angle = _body->GetAngle();
}

void Entity::updateBodyFromPose()
{
    if (!_body)
    {
        return;
    }
    
    if (!isCloseEnough(_pose.velocity, _body->GetLinearVelocity()))
    {
        _body->SetLinearVelocity(_pose.velocity);
    }
    
    if (!isCloseEnough(_pose.position, _body->GetPosition()) ||
        !isCloseEnough(_pose.angle, _body->GetAngle()))
    {
        _body->SetTransform(_pose.position, _pose.angle);
    }
    
    if (_isBodyFacingLeft != _pose.isFacingLeft)
    {
        destroyFixtures();
        _isBodyFacingLeft = _pose.isFacingLeft;
        createFixtures();
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

EntityNetworkController* Entity::getNetworkController()
{
    return _networkController;
}

uint16_t Entity::getStateTime()
{
    return _state.stateTime;
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

const uint32_t Entity::getID()
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

bool Entity::isFacingLeft()
{
    return _pose.isFacingLeft;
}

std::string& Entity::getTextureMapping()
{
    return getTextureMapping(_state.state);
}

std::string& Entity::getTextureMapping(uint8_t state)
{
    auto q = _entityDef.textureMappings.find(state);
    
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

bool Entity::isFixedRotation() const
{
    return _entityDef.bodyFlags & BodyFlag_FixedRotation;
}

Entity::Pose& Entity::getPose()
{
    return _pose;
}

Entity::Pose& Entity::getPoseNetworkCache()
{
    return _poseNetworkCache;
}

Entity::State& Entity::getState()
{
    return _state;
}

Entity::State& Entity::getStateNetworkCache()
{
    return _stateNetworkCache;
}

void Entity::createFixtures()
{
    for (std::vector<FixtureDef>::iterator i = _entityDef.fixtures.begin(); i != _entityDef.fixtures.end(); ++i)
    {
        FixtureDef def = *i;
        if (_isBodyFacingLeft)
        {
            def.center.x = -def.center.x;
        }
        
        b2Shape* shape;
        b2PolygonShape polygonShape;
        b2CircleShape circleShape;
        if (def.flags & FixtureFlag_Circle)
        {
            circleShape.m_p.Set(def.center.x * _entityDef.width, def.center.y * _entityDef.height);
            circleShape.m_radius = def.vertices[0].x * _entityDef.width;
            
            shape = &circleShape;
        }
        else
        {
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
    
    _controller->onFixturesCreated(_fixtures);
}

void Entity::destroyFixtures()
{
    for (std::vector<b2Fixture*>::iterator i = _fixtures.begin(); i != _fixtures.end(); )
    {
        _body->DestroyFixture(*i);
        
        i = _fixtures.erase(i);
    }
    
    _groundSensorFixture = NULL;
}
