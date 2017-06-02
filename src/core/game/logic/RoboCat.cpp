//
//  RoboCat.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "RoboCat.h"

#include "InputState.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"

#include "World.h"
#include "Vector2.h"
#include "macros.h"
#include "GameConstants.h"

#include <math.h>

uint32_t RoboCat::getAllStateMask() const
{
    return ECRS_AllState;
}

void RoboCat::read(InputMemoryBitStream& inInputStream)
{
    bool stateBit;
    
    m_iReadState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        uint32_t playerId;
        inInputStream.read(playerId);
        setPlayerId(playerId);
        m_iReadState |= ECRS_PlayerId;
    }
    
    Vector2 replicatedLocation;
    Vector2 replicatedVelocity;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_fStateTime);
        
        inInputStream.read(replicatedVelocity.getXRef());
        inInputStream.read(replicatedVelocity.getYRef());
        
        m_velocity.set(replicatedVelocity);
        
        inInputStream.read(replicatedLocation.getXRef());
        inInputStream.read(replicatedLocation.getYRef());
        
        setPosition(replicatedLocation);
        
        inInputStream.read(m_isFacingLeft);
        
        m_iReadState |= ECRS_Pose;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        Color color;
        inInputStream.read(color);
        setColor(color);
        m_iReadState |= ECRS_Color;
    }
}

uint32_t RoboCat::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & ECRS_PlayerId)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(getPlayerId());
        
        writtenState |= ECRS_PlayerId;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & ECRS_Pose)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(m_fStateTime);
        
        Vector2 velocity = m_velocity;
        inOutputStream.write(velocity.getX());
        inOutputStream.write(velocity.getY());
        
        Vector2 location = getPosition();
        inOutputStream.write(location.getX());
        inOutputStream.write(location.getY());
        
        inOutputStream.write(m_isFacingLeft);
        
        writtenState |= ECRS_Pose;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & ECRS_Color)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(getColor());
        
        writtenState |= ECRS_Color;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    return writtenState;
}

void RoboCat::processInput(float inDeltaTime, IInputState* inInputState)
{
    //process our input....
    if (!inInputState)
    {
        return;
    }
    
    InputState* inputState = (InputState*)inInputState;
    
    // moving
    m_velocity.setX(inputState->getDesiredHorizontalDelta() * m_fSpeed);
    
    m_isFacingLeft = m_velocity.getX() < 0 ? true : m_velocity.getX() > 0 ? false : m_isFacingLeft;
    
    // jumping
    if (inputState->getDesiredJumpIntensity() > 0
        && m_isGrounded)
    {
        m_velocity.setY(inputState->getDesiredJumpIntensity() * m_fJumpSpeed);
        m_fStateTime = 0;
        m_isGrounded = false;
        m_isFalling = false;
    }
}

void RoboCat::simulateMovement(float inDeltaTime)
{
    update(inDeltaTime);
    
    processCollisions();
    
    if (m_velocity.getY() < 0
        && !m_isFalling)
    {
        m_isFalling = true;
        m_fStateTime = 0;
    }
}

void RoboCat::processCollisions()
{
    //right now just bounce off the sides..
    processCollisionsWithScreenWalls();
    
    float sourceRadius = 0.5f;
    Vector2 sourceLocation = getPosition();
    
    //now let's iterate through the world and see what we hit...
    //note: since there's a small number of objects in our game, this is fine.
    //but in a real game, brute-force checking collisions against every other object is not efficient.
    //it would be preferable to use a quad tree or some other structure to minimize the
    //number of collisions that need to be tested.
    for (auto goIt = World::sInstance->GetRoboCats().begin(), end = World::sInstance->GetRoboCats().end(); goIt != end; ++goIt)
    {
        PhysicalEntity* target = *goIt;
        if (target != this && !target->isRequestingDeletion())
        {
            //simple collision test for spheres- are the radii summed less than the distance?
            Vector2 targetLocation = target->getPosition();
            float targetRadius = 0.5f;
            
            Vector2 delta = targetLocation - sourceLocation;
            float distSq = delta.lenSquared();
            float collisionDist = (sourceRadius + targetRadius);
            if (distSq < (collisionDist * collisionDist))
            {
                //okay, you hit something!
                //so, project your location far enough that you're not colliding
                Vector2 dirToTarget = delta;
                dirToTarget.nor();
                Vector2 acceptableDeltaFromSourceToTarget = dirToTarget * collisionDist;
                //important note- we only move this cat. the other cat can take care of moving itself
                setPosition(targetLocation - acceptableDeltaFromSourceToTarget);
                
                Vector2 relVel = m_velocity;
                
                //if other object is a cat, it might have velocity, so there might be relative velocity...
                RoboCat* targetCat = target->getRTTI().derivesFrom(RoboCat::rtti) ? (RoboCat*)target : nullptr;
                if (targetCat)
                {
                    relVel -= targetCat->m_velocity;
                }
                
                //got vel with dir between objects to figure out if they're moving towards each other
                //and if so, the magnitude of the impulse (since they're both just balls)
                float relVelDotDir = relVel.dot(dirToTarget);
                
                if (relVelDotDir > 0.f)
                {
                    Vector2 impulse = relVelDotDir * dirToTarget;
                    
                    if (targetCat)
                    {
                        m_velocity -= impulse;
                        m_velocity *= m_fCatRestitution;
                    }
                    else
                    {
                        m_velocity -= impulse * 2.f;
                        m_velocity *= m_fWallRestitution;
                    }
                }
            }
        }
    }
}

void RoboCat::processCollisionsWithScreenWalls()
{
    Vector2 location = getPosition();
    float x = location.getX();
    float y = location.getY();
    
    float vx = m_velocity.getX();
    float vy = m_velocity.getY();
    
    float radius = 0.5f;
    
    //if the cat collides against a wall, the quick solution is to push it off
    if ((y + radius) >= CAM_HEIGHT && vy > 0)
    {
        m_velocity.setY(-vy * m_fWallRestitution);
        location.setY(CAM_HEIGHT - radius);
        setPosition(location);
        m_isGrounded = false;
        m_isFalling = true;
    }
    else if (y <= 2.3f && vy < 0)
    {
        m_acceleration.setY(-9.8f);
        m_velocity.setY(-vy * m_fWallRestitution);
        location.setY(2.3f);
        setPosition(location);
        m_isGrounded = true;
        m_isFalling = false;
    }
    
    if ((x + radius) >= CAM_WIDTH && vx > 0)
    {
        m_velocity.setX(-vx * m_fWallRestitution);
        location.setX(CAM_WIDTH - radius);
        setPosition(location);
    }
    else if (x <= (0) && vx < 0)
    {
        m_velocity.setX(-vx * m_fWallRestitution);
        location.setX(0);
        setPosition(location);
    }
}

void RoboCat::setPlayerId(uint32_t inPlayerId)
{
    m_iPlayerId = inPlayerId;
}

uint32_t RoboCat::getPlayerId() const
{
    return m_iPlayerId;
}

void RoboCat::setIndexInWorld(int inIndex)
{
    m_iIndexInWorld = inIndex;
}

int RoboCat::getIndexInWorld() const
{
    return m_iIndexInWorld;
}

bool RoboCat::isFacingLeft()
{
    return m_isFacingLeft;
}

bool RoboCat::isGrounded()
{
    return m_isGrounded;
}

RoboCat::RoboCat() : PhysicalEntity(0, 0, 1.565217391304348f, 2.0f),
m_fJumpSpeed(8.0f),
m_fSpeed(7.5f),
m_fWallRestitution(0.1f),
m_fCatRestitution(0.1f),
m_isFacingLeft(false),
m_isGrounded(false),
m_isFalling(false),
m_iPlayerId(0),
m_iReadState(0),
m_iIndexInWorld(-1)
{
    m_acceleration.setY(-9.8f);
}

RTTI_IMPL(RoboCat, PhysicalEntity);

NETWORK_TYPE_IMPL(RoboCat);
