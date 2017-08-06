//
//  SpacePirate.cpp
//  dante
//
//  Created by Stephen Gowen on 6/4/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SpacePirate.h"

#include "Box2D/Box2D.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"
#include "Robot.h"
#include "Ground.h"
#include "Crate.h"

#include "World.h"
#include "macros.h"
#include "GameConstants.h"
#include "Timing.h"
#include "StringUtil.h"
#include "Move.h"
#include "MathUtil.h"
#include "NGRect.h"
#include "OverlapTester.h"
#include "NetworkManagerServer.h"
#include "NetworkManagerClient.h"
#include "InstanceManager.h"
#include "Projectile.h"
#include "Util.h"
#include "EntityRegistry.h"
#include "FWInstanceManager.h"
#include "SpacePirateChunk.h"

#include <math.h>

SpacePirate::SpacePirate(b2World& world, bool isServer) : Entity(world, 0, 0, 2.0f, 2.347826086956522f, isServer, constructEntityDef()),
m_fSpeed(0.0),
m_iHealth(8),
m_fStartingHealth(8),
m_isFacingLeft(false),
m_isJumping(false),
m_fTimeForNextJump(0.0f),
m_fJumpSpeed(7.0f),
m_iHealthLeftLastKnown(8)
{
    m_fTimeForNextJump = Timing::getInstance()->getFrameStartTime();
    m_fTimeForNextJump += (rand() % 100) * 0.1f + 1.0f;
}

EntityDef SpacePirate::constructEntityDef()
{
    EntityDef ret = EntityDef();
    
    ret.isStaticBody = false;
    ret.restitution = 0.1f;
    ret.isCharacter = true;
    ret.density = 32.0f;
    
    return ret;
}

void SpacePirate::update()
{
    updateInternal(FRAME_RATE);
    
    if (m_isServer)
    {
        if (!areBox2DVectorsEqual(m_velocityLastKnown, getVelocity())
            || !areBox2DVectorsEqual(m_positionLastKnown, getPosition()))
        {
            NG_SERVER->setStateDirty(getID(), SPCP_Pose);
        }
        
        if (m_iHealthLeftLastKnown != m_iHealth)
        {
            NG_SERVER->setStateDirty(getID(), SPCP_Health);
        }
    }
    
    m_velocityLastKnown = b2Vec2(getVelocity().x, getVelocity().y);
    m_positionLastKnown = b2Vec2(getPosition().x, getPosition().y);
    m_iHealthLeftLastKnown = m_iHealth;
}

bool SpacePirate::shouldCollide(Entity *inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == m_footSensorFixture)
    {
        return inEntity->getRTTI().derivesFrom(Ground::rtti)
        || inEntity->getRTTI().derivesFrom(Crate::rtti);
    }
    
    return inEntity->getRTTI().derivesFrom(Robot::rtti)
    || inEntity->getRTTI().derivesFrom(Projectile::rtti)
    || inEntity->getRTTI().derivesFrom(Ground::rtti)
    || inEntity->getRTTI().derivesFrom(Crate::rtti);
}

void SpacePirate::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == m_footSensorFixture)
    {
        m_isJumping = false;
        
        handleBeginFootContact(inEntity);
        
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(Robot::rtti))
    {
        Robot* robot = static_cast<Robot*>(inEntity);
        
        handleBeginContactWithRobot(robot);
    }
    else if (inEntity->getRTTI().derivesFrom(Projectile::rtti))
    {
        Projectile* projectile = static_cast<Projectile*>(inEntity);
        
        projectile->handleBeginContactWithSpacePirate(this);
    }
}

void SpacePirate::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == m_footSensorFixture)
    {
        if (inEntity->getRTTI().derivesFrom(Ground::rtti)
            || inEntity->getRTTI().derivesFrom(Crate::rtti))
        {
            handleEndFootContact(inEntity);
        }
        
        return;
    }
}

uint32_t SpacePirate::getAllStateMask() const
{
    return SPCP_AllState;
}

void SpacePirate::read(InputMemoryBitStream& inInputStream)
{
    bool stateBit;
    
    uint32_t readState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_color);
        
        inInputStream.read(m_fWidth);
        inInputStream.read(m_fHeight);
        readState |= SPCP_Info;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        b2Vec2 velocity;
        inInputStream.read(velocity);
        setVelocity(velocity);
        
        b2Vec2 position;
        inInputStream.read(position);
        setPosition(position);
        
        inInputStream.read(m_isFacingLeft);
        inInputStream.read(m_isJumping);
        
        readState |= SPCP_Pose;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_iHealth);
        readState |= SPCP_Health;
    }
}

uint32_t SpacePirate::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & SPCP_Info)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(m_color);
        
        inOutputStream.write(m_fWidth);
        inOutputStream.write(m_fHeight);
        
        writtenState |= SPCP_Info;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & SPCP_Pose)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(getVelocity());
        
        inOutputStream.write(getPosition());
        
        inOutputStream.write((bool)m_isFacingLeft);
        inOutputStream.write((bool)m_isJumping);
        
        writtenState |= SPCP_Pose;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & SPCP_Health)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(m_iHealth);
        
        writtenState |= SPCP_Health;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    return writtenState;
}

void SpacePirate::init(float x, float y, float speed, int scale, uint8_t health)
{
    setPosition(b2Vec2(x, y));
    
    m_fSpeed = speed;
    
    m_fWidth *= scale;
    m_fHeight *= scale;
    
    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(m_fWidth / 2.0f, m_fHeight / 2.0f);
    
    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    
    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = 1.0f;
    
    // Override the default friction.
    fixtureDef.friction = 0.3f;
    
    m_body->DestroyFixture(m_fixture);
    
    // Add the shape to the body.
    m_fixture = m_body->CreateFixture(&fixtureDef);
    
    m_fixture->SetUserData(this);
    
    m_iHealth = health;
    m_fStartingHealth = m_iHealth;
}

void SpacePirate::handleBeginContactWithRobot(Robot* robot)
{
    // Deal Damage to player robot
    robot->takeDamage();
}

void SpacePirate::takeDamage(b2Vec2 force, bool isHeadshot)
{
    m_lastForce.Set(force.x, force.y);
    m_iHealth -= isHeadshot ? 8 : 1;
    if (m_iHealth > m_fStartingHealth)
    {
        // We are using unsigned values for health
        m_iHealth = 0;
    }
}

uint8_t SpacePirate::getHealth()
{
    return m_iHealth;
}

float SpacePirate::getSpeed()
{
    return m_fSpeed;
}

bool SpacePirate::isFacingLeft()
{
    return m_isFacingLeft;
}

void SpacePirate::updateInternal(float inDeltaTime)
{
    m_fStateTime += inDeltaTime;
    
    if (isGrounded())
    {
        setAngle(0);
    }
    
    if (getVelocity().y < 0 && !isFalling())
    {
        m_fStateTime = 0;
    }
    
    if (getPosition().y < DEAD_ZONE_BOTTOM)
    {
        requestDeletion();
        
        return;
    }
    
    if (!m_isServer)
    {
        return;
    }
    
    if (m_iHealth == 0 && !isRequestingDeletion())
    {
        for (int i = 0; i < 4; ++i)
        {
            SpacePirateChunk* chunk = static_cast<SpacePirateChunk*>(SERVER_ENTITY_REG->createEntity(NW_TYPE_SpacePirateChunk));
            chunk->initFromSpacePirate(this, m_lastForce, i);
        }
    }
    
    if (m_iHealth == 0)
    {
        requestDeletion();
    }
    
    if (Timing::getInstance()->getFrameStartTime() > m_fTimeForNextJump)
    {
        if (isGrounded())
        {
            m_fTimeForNextJump = Timing::getInstance()->getFrameStartTime();
            m_fTimeForNextJump += (rand() % 100) * 0.1f + 1.0f;
            
            m_fStateTime = 0;
            m_isJumping = true;
            
            setVelocity(b2Vec2(getVelocity().x, m_fJumpSpeed));
        }
    }
    
    bool targetFound = false;
    
    float shortestDistance = GAME_WIDTH;
    std::vector<Entity*> players = InstanceManager::getServerWorld()->getPlayers();
    for (Entity* entity : players)
    {
        Robot* robot = static_cast<Robot*>(entity);
        
        if (!robot->isRequestingDeletion())
        {
            float dist = b2Distance(robot->getPosition(), getPosition());
            if (dist < shortestDistance)
            {
                shortestDistance = dist;
                m_isFacingLeft = robot->getPosition().x < getPosition().x;
                
                setVelocity(b2Vec2(m_isFacingLeft ? -m_fSpeed : m_fSpeed, getVelocity().y));
                
                targetFound = true;
            }
        }
    }
    
    if (!targetFound)
    {
        m_isFacingLeft = true;
        
        setVelocity(b2Vec2(0, getVelocity().y));
    }
}

RTTI_IMPL(SpacePirate, Entity);

NW_TYPE_IMPL(SpacePirate);
