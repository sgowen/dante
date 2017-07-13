//
//  SpacePirate.cpp
//  dante
//
//  Created by Stephen Gowen on 6/4/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SpacePirate.h"

#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"
#include "Robot.h"

#include "World.h"
#include "Vector2.h"
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

#include <math.h>

Entity* SpacePirate::staticCreateClient()
{
    return new SpacePirate(false);
}

Entity* SpacePirate::staticCreateServer()
{
    return new SpacePirate(true);
}

void SpacePirate::onDeletion()
{
    if (m_isServer)
    {
        NG_SERVER->deregisterEntity(this);
    }
}

void SpacePirate::update()
{
    Timing* timing = m_isServer ? Timing::getInstance() : Timing::getInstance();
    
    if (m_isServer)
    {
        Vector2 oldAcceleration = getAcceleration();
        Vector2 oldVelocity = getVelocity();
        bool old_isFacingLeft = m_isFacingLeft;
        bool old_isGrounded = m_isGrounded;
        bool old_isFalling = m_isFalling;
        
        processAI();
        
        updateInternal(timing->getDeltaTime());
        
        if (!oldAcceleration.isEqualTo(getAcceleration())
            || !oldVelocity.isEqualTo(getVelocity())
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
        
        inInputStream.read(m_acceleration.getXRef());
        inInputStream.read(m_acceleration.getYRef());
        
        inInputStream.read(m_velocity.getXRef());
        inInputStream.read(m_velocity.getYRef());
        
        inInputStream.read(m_position.getXRef());
        inInputStream.read(m_position.getYRef());
        
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
        
        inOutputStream.write(m_acceleration.getX());
        inOutputStream.write(m_acceleration.getY());
        
        inOutputStream.write(m_velocity.getX());
        inOutputStream.write(m_velocity.getY());
        
        inOutputStream.write(m_position.getX());
        inOutputStream.write(m_position.getY());
        
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
    setPosition(Vector2(x, y));
    m_fSpeed = speed;
    
    m_fWidth *= scale;
    m_fHeight *= scale;
    
    resetBounds(m_fWidth, m_fHeight);
    
    m_iHealth = health;
    m_fStartingHealth = m_iHealth;
}

void SpacePirate::takeDamage(bool isHeadshot)
{
    if (!m_isServer)
    {
        return;
    }
    
    m_iHealth -= isHeadshot ? 8 : 1;
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

void SpacePirate::processAI()
{
    if (!m_isServer)
    {
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
            
            m_acceleration.setY(-9.8f);
            m_velocity.setY(m_fJumpSpeed);
            
            // tell the world our health dropped
            NG_SERVER->setStateDirty(getID(), SPCP_Pose);
        }
    }
}

void SpacePirate::updateInternal(float inDeltaTime)
{
    Entity::update(inDeltaTime);
    
    processCollisions();
}

void SpacePirate::processCollisions()
{
    processCollisionsWithScreenWalls();
    
    if (!m_isServer)
    {
        return;
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
            float dist = robot->getPosition().dist(getPosition());
            if (dist < shortestDistance)
            {
                shortestDistance = dist;
                m_isFacingLeft = robot->getPosition().getX() < getPosition().getX();
                m_velocity.setX(m_isFacingLeft ? -m_fSpeed : m_fSpeed);
                targetFound = true;
            }
            
            if (OverlapTester::doNGRectsOverlap(getMainBounds(), target->getMainBounds()))
            {
                // Deal Damage to player robot
                robot->takeDamage();
            }
        }
    }
    
    if (!targetFound)
    {
        m_isFacingLeft = true;
        m_velocity.setX(0);
    }
}

void SpacePirate::processCollisionsWithScreenWalls()
{
    Vector2 position = getPosition();
    float x = position.getX();
    
    float boundsY = getMainBounds().getBottom();
    
    float vx = m_velocity.getX();
    float vy = m_velocity.getY();
    
    float radius = m_fWidth / 2;
    
    if (boundsY <= GROUND_TOP && vy < 0)
    {
        m_velocity.setY(0);
        m_acceleration.setY(0);
        position.setY(GROUND_TOP + getMainBounds().getHeight() / 2);
        setPosition(position);
        m_isGrounded = true;
        m_isFalling = false;
        m_isJumping = false;
    }
    
    if ((x + radius) >= GAME_WIDTH && vx > 0)
    {
        position.setX(GAME_WIDTH - radius);
        setPosition(position);
    }
    else if (x <= 0 && vx < 0)
    {
        position.setX(0);
        setPosition(position);
    }
}

SpacePirate::SpacePirate(bool isServer) : Entity(0, 0, 2.0f, 2.347826086956522f),
m_isServer(isServer),
m_fSpeed(0.0),
m_iHealth(8),
m_fStartingHealth(8),
m_isFacingLeft(false),
m_isGrounded(false),
m_isFalling(false),
m_isJumping(false),
m_fRobotRestitution(0.1f),
m_fTimeForNextJump(0.0f),
m_fJumpSpeed(7.0f)
{
    m_acceleration.setY(-9.8f);
    
    m_fTimeForNextJump = Timing::getInstance()->getFrameStartTime();
    m_fTimeForNextJump += (rand() % 100) * 0.1f + 1.0f;
    
    if (m_isServer)
    {
        NG_SERVER->registerEntity(this);
    }
}

RTTI_IMPL(SpacePirate, Entity);

NETWORK_TYPE_IMPL(SpacePirate);
