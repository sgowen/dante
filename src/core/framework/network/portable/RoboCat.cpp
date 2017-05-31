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

//zoom hardcoded at 100...if we want to lock players on screen, this could be calculated from zoom
const float WORLD_HEIGHT = 7.2f;
const float WORLD_WIDTH = 12.8f;

RoboCat::RoboCat() : GameObject(),
m_fMaxRotationSpeed(5.f),
m_fMaxLinearSpeed(50.f),
mVelocity(0.0f, 0.0f),
m_fWallRestitution(0.1f),
m_fCatRestitution(0.1f),
m_fThrustDir(0.f),
m_iPlayerId(0),
m_iHealth(10)
{
    SetCollisionRadius(0.5f);
}

void RoboCat::ProcessInput(float inDeltaTime, const InputState& inInputState)
{
    //process our input....
    
    //turning...
    float newRotation = GetRotation() + inInputState.GetDesiredHorizontalDelta() * m_fMaxRotationSpeed * inDeltaTime;
    SetRotation(newRotation);
    
    //moving...
    float inputForwardDelta = inInputState.GetDesiredVerticalDelta();
    m_fThrustDir = inputForwardDelta;
}

void RoboCat::AdjustVelocityByThrust(float inDeltaTime)
{
    //just set the velocity based on the thrust direction -- no thrust will lead to 0 velocity
    //simulating acceleration makes the client prediction a bit more complex
    Vector2 forwardVector = GetForwardVector();
    mVelocity = forwardVector * (m_fThrustDir * inDeltaTime * m_fMaxLinearSpeed);
}

void RoboCat::SimulateMovement(float inDeltaTime)
{
    //simulate us...
    AdjustVelocityByThrust(inDeltaTime);
    
    SetLocation(GetLocation() + mVelocity * inDeltaTime);
    
    ProcessCollisions();
}

void RoboCat::Update()
{
    // Empty
}

void RoboCat::ProcessCollisions()
{
    //right now just bounce off the sides..
    ProcessCollisionsWithScreenWalls();
    
    float sourceRadius = GetCollisionRadius();
    Vector2 sourceLocation = GetLocation();
    
    //now let's iterate through the world and see what we hit...
    //note: since there's a small number of objects in our game, this is fine.
    //but in a real game, brute-force checking collisions against every other object is not efficient.
    //it would be preferable to use a quad tree or some other structure to minimize the
    //number of collisions that need to be tested.
    for (auto goIt = World::sInstance->GetGameObjects().begin(), end = World::sInstance->GetGameObjects().end(); goIt != end; ++goIt)
    {
        GameObject* target = *goIt;
        if (target != this && !target->DoesWantToDie())
        {
            //simple collision test for spheres- are the radii summed less than the distance?
            Vector2 targetLocation = target->GetLocation();
            float targetRadius = target->GetCollisionRadius();
            
            Vector2 delta = targetLocation - sourceLocation;
            float distSq = delta.lenSquared();
            float collisionDist = (sourceRadius + targetRadius);
            if (distSq < (collisionDist * collisionDist))
            {
                //first, tell the other guy there was a collision with a cat, so it can do something...
                
                if (target->HandleCollisionWithCat(this))
                {
                    //okay, you hit something!
                    //so, project your location far enough that you're not colliding
                    Vector2 dirToTarget = delta;
                    dirToTarget.nor();
                    Vector2 acceptableDeltaFromSourceToTarget = dirToTarget * collisionDist;
                    //important note- we only move this cat. the other cat can take care of moving itself
                    SetLocation(targetLocation - acceptableDeltaFromSourceToTarget);
                    
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
}

void RoboCat::ProcessCollisionsWithScreenWalls()
{
    Vector2 location = GetLocation();
    float x = location.getX();
    float y = location.getY();
    
    float vx = mVelocity.getX();
    float vy = mVelocity.getY();
    
    float radius = GetCollisionRadius();
    
    //if the cat collides against a wall, the quick solution is to push it off
    if ((y + radius) >= WORLD_HEIGHT && vy > 0)
    {
        mVelocity.setY(-vy * m_fWallRestitution);
        location.setY(WORLD_HEIGHT - radius);
        SetLocation(location);
    }
    else if (y <= (0) && vy < 0)
    {
        mVelocity.setY(-vy * m_fWallRestitution);
        location.setY(0);
        SetLocation(location);
    }
    
    if ((x + radius) >= WORLD_WIDTH && vx > 0)
    {
        mVelocity.setX(-vx * m_fWallRestitution);
        location.setX(WORLD_WIDTH - radius);
        SetLocation(location);
    }
    else if (x <= (0) && vx < 0)
    {
        mVelocity.setX(-vx * m_fWallRestitution);
        location.setX(0);
        SetLocation(location);
    }
}

RTTI_IMPL(RoboCat, GameObject);

NETWORK_TYPE_IMPL(RoboCat);
