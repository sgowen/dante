//
//  SpacePirate.cpp
//  dante
//
//  Created by Stephen Gowen on 6/4/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SpacePirate.h"

#include "InputState.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"

#include "World.h"
#include "Vector2.h"
#include "macros.h"
#include "GameConstants.h"
#include "Timing.h"
#include "StringUtil.h"
#include "Move.h"
#include "MathUtil.h"
#include "NGRect.h"
#include "Robot.h"
#include "OverlapTester.h"

#ifdef NG_SERVER
#include "NetworkManagerServer.h"
#endif

#include <math.h>

Entity* SpacePirate::create()
{
    SpacePirate* ret = new SpacePirate();
    
#ifdef NG_SERVER
    NetworkManagerServer::getInstance()->registerEntity(ret);
#endif
    
    return ret;
}

void SpacePirate::onDeletion()
{
#ifdef NG_SERVER
    NetworkManagerServer::getInstance()->unregisterEntity(this);
#endif
}

void SpacePirate::update()
{
#ifdef NG_SERVER
    Vector2 oldAcceleration = getAcceleration();
    Vector2 oldVelocity = getVelocity();
    bool old_isFacingLeft = m_isFacingLeft;
    bool old_isGrounded = m_isGrounded;
    bool old_isFalling = m_isFalling;
    
    updateInternal(Timing::getInstance()->getDeltaTime());
    
    if (!oldAcceleration.isEqualTo(getAcceleration())
        || !oldVelocity.isEqualTo(getVelocity())
        || old_isFacingLeft != m_isFacingLeft
        || old_isGrounded != m_isGrounded
        || old_isFalling != m_isFalling)
    {
        NetworkManagerServer::getInstance()->setStateDirty(getID(), SPCP_Pose);
    }
#elif NG_CLIENT
    updateInternal(Timing::getInstance()->getDeltaTime());
#endif
}

uint32_t SpacePirate::getAllStateMask() const
{
    return SPCP_AllState;
}

void SpacePirate::read(InputMemoryBitStream& inInputStream)
{    
    bool stateBit;
    
    uint32_t readState = 0;
    
    Vector2 replicatedAcceleration;
    Vector2 replicatedVelocity;
    Vector2 replicatedPosition;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_fStateTime);
        
        inInputStream.read(replicatedVelocity.getXRef());
        inInputStream.read(replicatedVelocity.getYRef());
        
        m_velocity.set(replicatedVelocity);
        
        inInputStream.read(replicatedPosition.getXRef());
        inInputStream.read(replicatedPosition.getYRef());
        
        setPosition(replicatedPosition);
        
        inInputStream.read(m_isFacingLeft);
        
        readState |= SPCP_Pose;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        Color color;
        inInputStream.read(color);
        setColor(color);
        readState |= SPCP_Color;
    }
}

uint32_t SpacePirate::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & SPCP_Pose)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(m_fStateTime);
        
        Vector2 velocity = m_velocity;
        inOutputStream.write(velocity.getX());
        inOutputStream.write(velocity.getY());
        
        Vector2 position = getPosition();
        inOutputStream.write(position.getX());
        inOutputStream.write(position.getY());
        
        inOutputStream.write((bool)m_isFacingLeft);
        
        writtenState |= SPCP_Pose;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & SPCP_Color)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(getColor());
        
        writtenState |= SPCP_Color;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    return writtenState;
}

void SpacePirate::init(float x, float y, float speed)
{
    setPosition(Vector2(x, y));
    m_fSpeed = speed;
}

#ifdef NG_SERVER
void SpacePirate::takeDamage()
{
    m_iHealth--;
    
    if (m_iHealth <= 0)
    {
        requestDeletion();
    }
    
    // tell the world our health dropped
    NetworkManagerServer::getInstance()->setStateDirty(getID(), SPCP_Health);
}
#endif

bool SpacePirate::isFacingLeft()
{
    return m_isFacingLeft;
}

void SpacePirate::updateInternal(float inDeltaTime)
{
    Entity::update(inDeltaTime);
    
    processCollisions();
}

void SpacePirate::processCollisions()
{
    processCollisionsWithScreenWalls();
    
#ifdef NG_SERVER
    float shortestDistance = CAM_WIDTH;
    for (auto goIt = World::getInstance()->getEntities().begin(), end = World::getInstance()->getEntities().end(); goIt != end; ++goIt)
    {
        Entity* target = *goIt;
        if (target != this && !target->isRequestingDeletion() && target->getRTTI().derivesFrom(Robot::rtti))
        {
            Robot* robot = static_cast<Robot*>(target);
            float dist = robot->getPosition().dist(getPosition());
            if (dist < shortestDistance)
            {
                shortestDistance = dist;
                m_isFacingLeft = robot->getPosition().getX() < getPosition().getX();
                m_velocity.setX(m_isFacingLeft ? -m_fSpeed : m_fSpeed);
            }
            
            if (OverlapTester::doNGRectsOverlap(getMainBounds(), target->getMainBounds()))
            {
                // Deal Damage to player robot
                robot->takeDamage();
            }
        }
    }
#endif
}

void SpacePirate::processCollisionsWithScreenWalls()
{
    Vector2 position = getPosition();
    float x = position.getX();
    float y = position.getY();
    
    float boundsY = getMainBounds().getBottom();
    
    float vx = m_velocity.getX();
    float vy = m_velocity.getY();
    
    float radius = m_fWidth / 2;
    
    //if the robot collides against a wall, the quick solution is to push it off
    if ((y + radius) >= CAM_HEIGHT && vy > 0)
    {
        m_velocity.setY(-vy * m_fWallRestitution);
        position.setY(CAM_HEIGHT - radius);
        setPosition(position);
        m_isGrounded = false;
        m_isFalling = true;
    }
    else if (boundsY <= 1.3f && vy < 0)
    {
        m_velocity.setY(0);
        m_acceleration.setY(0);
        position.setY(1.3f + getMainBounds().getHeight() / 2);
        setPosition(position);
        m_isGrounded = true;
        m_isFalling = false;
    }
    
    if ((x + radius) >= CAM_WIDTH && vx > 0)
    {
        m_velocity.setX(-vx * m_fWallRestitution);
        position.setX(CAM_WIDTH - radius);
        setPosition(position);
    }
    else if (x <= (0) && vx < 0)
    {
        m_velocity.setX(-vx * m_fWallRestitution);
        position.setX(0);
        setPosition(position);
    }
}

SpacePirate::SpacePirate() : Entity(0, 0, 1.565217391304348f * 1.277777777777778f, 2.0f * 1.173913043478261f),
m_fSpeed(0.0),
m_iHealth(10),
m_isFacingLeft(false),
m_isGrounded(false),
m_isFalling(false),
m_fWallRestitution(0.1f),
m_fRobotRestitution(0.1f)
{
    m_acceleration.setY(-9.8f);
}

RTTI_IMPL(SpacePirate, Entity);

NETWORK_TYPE_IMPL(SpacePirate);
