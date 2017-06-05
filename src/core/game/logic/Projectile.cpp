//
//  Projectile.cpp
//  dante
//
//  Created by Stephen Gowen on 6/4/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Projectile.h"

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
#include "ClientProxy.h"
#elif NG_CLIENT
#include "NetworkManagerClient.h"
#include "InputManager.h"
#include "NGAudioEngine.h"
#endif

#include <math.h>

Entity* Projectile::create()
{
    Projectile* ret = new Projectile();
    
#ifdef NG_SERVER
    NetworkManagerServer::getInstance()->registerEntity(ret);
#endif
    
    return ret;
}

void Projectile::onDeletion()
{
#ifdef NG_SERVER
    NetworkManagerServer::getInstance()->unregisterEntity(this);
#endif
}

void Projectile::update()
{
#ifdef NG_SERVER
    Vector2 oldAcceleration = getAcceleration();
    Vector2 oldVelocity = getVelocity();
    Vector2 oldPosition = getPosition();
    
    updateInternal(Timing::getInstance()->getDeltaTime());
    
    if (!oldAcceleration.isEqualTo(getAcceleration())
        || !oldVelocity.isEqualTo(getVelocity())
        || !oldPosition.isEqualTo(getPosition()))
    {
        NetworkManagerServer::getInstance()->setStateDirty(getID(), PRJC_Pose);
    }
#elif NG_CLIENT
    updateInternal(Timing::getInstance()->getDeltaTime());
#endif
}

uint32_t Projectile::getAllStateMask() const
{
    return PRJC_AllState;
}

void Projectile::read(InputMemoryBitStream& inInputStream)
{
#ifdef NG_CLIENT
    float oldStateTime = m_fStateTime;
    Vector2 oldVelocity = getVelocity();
    Vector2 oldPosition = getPosition();
#endif
    
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
        
        readState |= PRJC_Pose;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        Color color;
        inInputStream.read(color);
        setColor(color);
        readState |= PRJC_Color;
    }
    
#ifdef NG_CLIENT
    // if this is a create packet, don't interpolate
    if ((readState & PRJC_Color) == 0)
    {
        interpolateClientSidePrediction(oldStateTime, oldVelocity, oldPosition);
    }
#endif
}

uint32_t Projectile::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & PRJC_Pose)
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
        
        writtenState |= PRJC_Pose;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & PRJC_Color)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(getColor());
        
        writtenState |= PRJC_Color;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    return writtenState;
}

void Projectile::initFromShooter(Robot* inRobot)
{
    m_owner = inRobot;
    m_isFacingLeft = inRobot->isFacingLeft();
    m_velocity.setX(m_isFacingLeft ? -12 : 12);
    m_position.set(inRobot->getPosition());
    m_position.add(m_isFacingLeft ? -0.5f : 0.5f, 0.4f);
}

bool Projectile::isFacingLeft()
{
    return m_isFacingLeft;
}

void Projectile::updateInternal(float inDeltaTime)
{
    Entity::update(inDeltaTime);
    
    processCollisions();
}

void Projectile::processCollisions()
{
    processCollisionsWithScreenWalls();
    
    for (auto goIt = World::getInstance()->getEntities().begin(), end = World::getInstance()->getEntities().end(); goIt != end; ++goIt)
    {
        Entity* target = *goIt;
        if (target != this && target != m_owner && !target->isRequestingDeletion() && target->getRTTI().derivesFrom(Robot::rtti))
        {
            if (OverlapTester::doNGRectsOverlap(getMainBounds(), target->getMainBounds()))
            {
                //requestDeletion();
            }
        }
    }
}

void Projectile::processCollisionsWithScreenWalls()
{
    NGRect& bounds = getMainBounds();
    if (bounds.getBottom() > CAM_HEIGHT
        || bounds.getTop() < 0
        || bounds.getLeft() > CAM_WIDTH
        || bounds.getRight() < 0)
    {
        requestDeletion();
    }
}

#ifdef NG_CLIENT
void Projectile::interpolateClientSidePrediction(float& inOldStateTime, Vector2& inOldVelocity, Vector2& inOldPosition)
{
    float roundTripTime = NetworkManagerClient::getInstance()->getRoundTripTime();
    
    if (!areFloatsPracticallyEqual(inOldStateTime, m_fStateTime))
    {
        m_fStateTime = inOldStateTime + 0.1f * (m_fStateTime - inOldStateTime);
    }
    
    if (!inOldVelocity.isEqualTo(getVelocity()))
    {
        LOG("ERROR! Move replay ended with incorrect velocity! Old %3.8f, %3.8f - New %3.8f, %3.8f", inOldVelocity.getX(), inOldVelocity.getY(), getVelocity().getX(), getVelocity().getY());
        
        //have we been out of sync, or did we just become out of sync?
        float time = Timing::getInstance()->getFrameStartTime();
        if (m_fTimeVelocityBecameOutOfSync == 0.0f)
        {
            m_fTimeVelocityBecameOutOfSync = time;
        }
        
        //now interpolate to the correct value...
        float durationOutOfSync = time - m_fTimeVelocityBecameOutOfSync;
        if (durationOutOfSync < roundTripTime)
        {
            m_velocity.set(lerp(inOldVelocity, getVelocity(), 0.1f));
        }
        //otherwise, fine...
    }
    else
    {
        //we're in sync
        m_fTimeVelocityBecameOutOfSync = 0.0f;
    }
    
    if (!inOldPosition.isEqualTo(getPosition()))
    {
        LOG("ERROR! Move replay ended with incorrect position! Old %3.8f, %3.8f - New %3.8f, %3.8f", inOldPosition.getX(), inOldPosition.getY(), getPosition().getX(), getPosition().getY());
        
        //have we been out of sync, or did we just become out of sync?
        float time = Timing::getInstance()->getFrameStartTime();
        if (m_fTimePositionBecameOutOfSync == 0.0f)
        {
            m_fTimePositionBecameOutOfSync = time;
        }
        
        float durationOutOfSync = time - m_fTimePositionBecameOutOfSync;
        if (durationOutOfSync < roundTripTime)
        {
            setPosition(lerp(inOldPosition, getPosition(), 0.1f));
        }
    }
    else
    {
        //we're in sync
        m_fTimePositionBecameOutOfSync = 0.0f;
    }
}
#endif

Projectile::Projectile() : Entity(0, 0, 1.565217391304348f * 0.444444444444444f, 2.0f * 0.544423440453686f),
m_owner(nullptr),
m_fTimeVelocityBecameOutOfSync(0.0f),
m_fTimePositionBecameOutOfSync(0.0f),
m_isFacingLeft(false)
{
    // Empty
}

RTTI_IMPL(Projectile, Entity);

NETWORK_TYPE_IMPL(Projectile);
