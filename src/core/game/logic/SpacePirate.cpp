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
#include "Ground.h"
#include "Projectile.h"

#include <math.h>

SpacePirate::SpacePirate(b2World& world, bool isServer) : Entity(world, 0, 0, 2.0f, 2.347826086956522f),
m_isServer(isServer),
m_fSpeed(0.0),
m_iHealth(8),
m_fStartingHealth(8),
m_isFacingLeft(false),
m_isGrounded(false),
m_isFalling(false),
m_isJumping(false),
m_fTimeForNextJump(0.0f),
m_fJumpSpeed(7.0f)
{
    m_fTimeForNextJump = Timing::getInstance()->getFrameStartTime();
    m_fTimeForNextJump += (rand() % 100) * 0.1f + 1.0f;
}

void SpacePirate::update()
{
    Timing* timing = m_isServer ? Timing::getInstance() : Timing::getInstance();
    
    if (m_isServer)
    {
        b2Vec2 oldVelocity = b2Vec2(getVelocity().x, getVelocity().y);
        bool old_isFacingLeft = m_isFacingLeft;
        bool old_isGrounded = m_isGrounded;
        bool old_isFalling = m_isFalling;
        
        updateInternal(timing->getDeltaTime());
        
        if (!areBox2DVectorsEqual(oldVelocity, getVelocity())
            || old_isFacingLeft != m_isFacingLeft
            || old_isGrounded != m_isGrounded
            || old_isFalling != m_isFalling)
        {
            NG_SERVER->setStateDirty(getID(), SPCP_Pose);
        }
    }
    else
    {
        updateInternal(timing->getDeltaTime());
    }
}

void SpacePirate::handleContact(Entity* inEntity)
{
    if (inEntity != this
        && !inEntity->isRequestingDeletion())
    {
        if (inEntity->getRTTI().derivesFrom(Robot::rtti))
        {
            Robot* robot = static_cast<Robot*>(inEntity);
            
            handleContactWithRobot(robot);
        }
        else if (inEntity->getRTTI().derivesFrom(Ground::rtti))
        {
            handleContactWithGround(nullptr);
        }
        else if (inEntity->getRTTI().derivesFrom(Projectile::rtti))
        {
            Projectile* projectile = static_cast<Projectile*>(inEntity);
            
            projectile->handleContactWithSpacePirate(this);
        }
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
        inInputStream.read(m_fStateTime);
        
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
        inInputStream.read(m_color);
        readState |= SPCP_Color;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_iHealth);
        readState |= SPCP_Health;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_fWidth);
        inInputStream.read(m_fHeight);
        readState |= SPCP_Size;
    }
}

uint32_t SpacePirate::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & SPCP_Pose)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(m_fStateTime);
        
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
    
    if (inDirtyState & SPCP_Color)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(m_color);
        
        writtenState |= SPCP_Color;
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
    
    if (inDirtyState & SPCP_Size)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(m_fWidth);
        inOutputStream.write(m_fHeight);
        
        writtenState |= SPCP_Size;
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
    m_body->CreateFixture(&fixtureDef);
    
    m_iHealth = health;
    m_fStartingHealth = m_iHealth;
}

void SpacePirate::handleContactWithRobot(Robot* robot)
{
    // Deal Damage to player robot
    robot->takeDamage();
}

void SpacePirate::handleContactWithGround(Ground* ground)
{
    m_isGrounded = true;
    m_isFalling = false;
    m_isJumping = false;
    
    NG_SERVER->setStateDirty(getID(), SPCP_Pose);
}

void SpacePirate::takeDamage(bool isHeadshot)
{
    if (!m_isServer)
    {
        return;
    }
    
    m_iHealth -= isHeadshot ? 32 : 1;
    if (m_iHealth > m_fStartingHealth)
    {
        // We are using unsigned values for health
        m_iHealth = 0;
    }
    
    if (m_iHealth == 0)
    {
        requestDeletion();
    }
    
    // tell the world our health dropped
    NG_SERVER->setStateDirty(getID(), SPCP_Health);
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
    
    if (m_isGrounded)
    {
        setAngle(0);
    }
    
    if (getVelocity().y < 0 && !m_isFalling)
    {
        m_isFalling = true;
        m_fStateTime = 0;
    }
    
    if (!m_isServer)
    {
        return;
    }
    
    if (getPosition().y < -5)
    {
        requestDeletion();
        
        return;
    }
    
    if (Timing::getInstance()->getFrameStartTime() > m_fTimeForNextJump)
    {
        if (m_isGrounded)
        {
            m_fTimeForNextJump = Timing::getInstance()->getFrameStartTime();
            m_fTimeForNextJump += (rand() % 100) * 0.1f + 1.0f;
            
            m_fStateTime = 0;
            m_isGrounded = false;
            m_isFalling = false;
            m_isJumping = true;
            
            setVelocity(b2Vec2(getVelocity().x, m_fJumpSpeed));
            
            // tell the world we just jumped
            NG_SERVER->setStateDirty(getID(), SPCP_Pose);
        }
    }
    
    bool targetFound = false;
    float shortestDistance = GAME_WIDTH;
    Robot* robot = nullptr;
    std::vector<Entity*> entities = InstanceManager::getServerWorld()->getEntities();
    for (Entity* target : entities)
    {
        robot = nullptr;
        if (target != this
            && !target->isRequestingDeletion()
            && target->getRTTI().derivesFrom(Robot::rtti)
            && (robot = static_cast<Robot*>(target)))
        {
            float dist = b2Distance(robot->getPosition(), getPosition());
            if (dist < shortestDistance)
            {
                shortestDistance = dist;
                m_isFacingLeft = robot->getPosition().x < getPosition().x;
                
                setVelocity(b2Vec2(m_isFacingLeft ? -m_fSpeed : m_fSpeed, getVelocity().y));
                
                targetFound = true;
                
                NG_SERVER->setStateDirty(getID(), SPCP_Pose);
            }
        }
    }
    
    if (!targetFound)
    {
        m_isFacingLeft = true;
        
        setVelocity(b2Vec2(0, getVelocity().y));
        
        NG_SERVER->setStateDirty(getID(), SPCP_Pose);
    }
}

RTTI_IMPL(SpacePirate, Entity);

NETWORK_TYPE_IMPL(SpacePirate);
