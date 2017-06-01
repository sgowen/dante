//
//  RoboCat.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "RoboCat.h"

#include "NetworkManager.h"
#include "World.h"
#include "Vector2.h"
#include "macros.h"

//zoom hardcoded at 100...if we want to lock players on screen, this could be calculated from zoom
const float WORLD_HEIGHT = 7.2f;
const float WORLD_WIDTH = 12.8f;

RoboCat::RoboCat() : NWPhysicalEntity(),
m_fMaxRotationSpeed(5.f),
m_fMaxLinearSpeed(50.f),
mVelocity(0.0f, 0.0f),
m_fWallRestitution(0.1f),
m_fCatRestitution(0.1f),
m_fThrustDir(0.f),
m_iPlayerId(0),
m_iIndexInWorld(-1)
{
    // Empty
}

void RoboCat::ProcessInput(float inDeltaTime, const InputState& inInputState)
{
    //process our input....
    
    //turning...
    float newRotation = getAngle() + inInputState.getDesiredHorizontalDelta() * m_fMaxRotationSpeed * inDeltaTime;
    setAngle(newRotation);
    
    //moving...
    float inputForwardDelta = inInputState.getDesiredVerticalDelta();
    m_fThrustDir = inputForwardDelta;
}

void RoboCat::AdjustVelocityByThrust(float inDeltaTime)
{
    //just set the velocity based on the thrust direction -- no thrust will lead to 0 velocity
    //simulating acceleration makes the client prediction a bit more complex
    Vector2 forwardVector = Vector2(sinf(m_fAngle), -cosf(m_fAngle));
    mVelocity = forwardVector * (m_fThrustDir * inDeltaTime * m_fMaxLinearSpeed);
}

void RoboCat::SimulateMovement(float inDeltaTime)
{
    //simulate us...
    AdjustVelocityByThrust(inDeltaTime);
    
    setPosition(getPosition() + mVelocity * inDeltaTime);
    
    ProcessCollisions();
}

void RoboCat::ProcessCollisions()
{
    //right now just bounce off the sides..
    ProcessCollisionsWithScreenWalls();
    
    float sourceRadius = 0.5f;
    Vector2 sourceLocation = getPosition();
    
    //now let's iterate through the world and see what we hit...
    //note: since there's a small number of objects in our game, this is fine.
    //but in a real game, brute-force checking collisions against every other object is not efficient.
    //it would be preferable to use a quad tree or some other structure to minimize the
    //number of collisions that need to be tested.
    for (auto goIt = World::sInstance->GetRoboCats().begin(), end = World::sInstance->GetRoboCats().end(); goIt != end; ++goIt)
    {
        NWPhysicalEntity* target = *goIt;
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
                
                Vector2 relVel = mVelocity;
                
                //if other object is a cat, it might have velocity, so there might be relative velocity...
                RoboCat* targetCat = target->getRTTI().derivesFrom(RoboCat::rtti) ? (RoboCat*)target : nullptr;
                if (targetCat)
                {
                    relVel -= targetCat->mVelocity;
                }
                
                //got vel with dir between objects to figure out if they're moving towards each other
                //and if so, the magnitude of the impulse (since they're both just balls)
                float relVelDotDir = relVel.dot(dirToTarget);
                
                if (relVelDotDir > 0.f)
                {
                    Vector2 impulse = relVelDotDir * dirToTarget;
                    
                    if (targetCat)
                    {
                        mVelocity -= impulse;
                        mVelocity *= m_fCatRestitution;
                    }
                    else
                    {
                        mVelocity -= impulse * 2.f;
                        mVelocity *= m_fWallRestitution;
                    }
                }
            }
        }
    }
}

void RoboCat::ProcessCollisionsWithScreenWalls()
{
    Vector2 location = getPosition();
    float x = location.getX();
    float y = location.getY();
    
    float vx = mVelocity.getX();
    float vy = mVelocity.getY();
    
    float radius = 0.5f;
    
    //if the cat collides against a wall, the quick solution is to push it off
    if ((y + radius) >= WORLD_HEIGHT && vy > 0)
    {
        mVelocity.setY(-vy * m_fWallRestitution);
        location.setY(WORLD_HEIGHT - radius);
        setPosition(location);
    }
    else if (y <= (0) && vy < 0)
    {
        mVelocity.setY(-vy * m_fWallRestitution);
        location.setY(0);
        setPosition(location);
    }
    
    if ((x + radius) >= WORLD_WIDTH && vx > 0)
    {
        mVelocity.setX(-vx * m_fWallRestitution);
        location.setX(WORLD_WIDTH - radius);
        setPosition(location);
    }
    else if (x <= (0) && vx < 0)
    {
        mVelocity.setX(-vx * m_fWallRestitution);
        location.setX(0);
        setPosition(location);
    }
}

void RoboCat::setIndexInWorld(int inIndex)
{
    m_iIndexInWorld = inIndex;
}

int RoboCat::getIndexInWorld() const
{
    return m_iIndexInWorld;
}

RTTI_IMPL(RoboCat, NWPhysicalEntity);

NETWORK_TYPE_IMPL(RoboCat);
