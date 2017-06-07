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
#include "SpacePirate.h"
#include "OverlapTester.h"

#ifdef NG_SERVER
#include "NetworkManagerServer.h"
#elif NG_CLIENT
#include "NetworkManagerClient.h"
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
    Vector2 oldVelocity = getVelocity();
    ProjectileState oldState = m_state;
    bool old_isFacingLeft = m_isFacingLeft;
    
    updateInternal(Timing::getInstance()->getDeltaTime());
    
    if (!oldVelocity.isEqualTo(getVelocity())
        || oldState != m_state
        || old_isFacingLeft != m_isFacingLeft)
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
    Vector2 oldPosition = m_position;
    ProjectileState oldState = m_state;
#endif
    
    bool stateBit;
    
    uint32_t readState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_iPlayerId);
        readState |= PRJC_PlayerId;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_fStateTime);
        
        inInputStream.read(m_velocity.getXRef());
        inInputStream.read(m_velocity.getYRef());
        
        inInputStream.read(m_position.getXRef());
        inInputStream.read(m_position.getYRef());
        
        inInputStream.read(m_state);
        
        inInputStream.read(m_isFacingLeft);
        
        readState |= PRJC_Pose;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_color);
        readState |= PRJC_Color;
    }
    
#ifdef NG_CLIENT
    if (getPlayerId() == NetworkManagerClient::getInstance()->getPlayerId())
    {
        // if this is a create packet, don't interpolate
        if ((readState & PRJC_PlayerId) == 0)
        {
            interpolateClientSidePrediction(oldPosition);
        }
    }
    else
    {
        if ((readState & PRJC_PlayerId) == 0)
        {
            interpolateClientSidePrediction(oldPosition);
        }
    }
    
    if (oldState == ProjectileState_Active
        && m_state == ProjectileState_Exploding)
    {
        playSound(SOUND_ID_EXPLOSION);
    }
    
    if ((readState & PRJC_PlayerId) != 0)
    {
        // This projectile was just created
        playSound(SOUND_ID_FIRE_ROCKET);
    }
#endif
}

uint32_t Projectile::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & PRJC_PlayerId)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(m_iPlayerId);
        
        writtenState |= PRJC_PlayerId;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & PRJC_Pose)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(m_fStateTime);
        
        inOutputStream.write(m_velocity.getX());
        inOutputStream.write(m_velocity.getY());
        
        inOutputStream.write(m_position.getX());
        inOutputStream.write(m_position.getY());
        
        inOutputStream.write(m_state);
        
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
        inOutputStream.write(m_color);
        
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
    m_iPlayerId = inRobot->getPlayerId();
    m_isFacingLeft = inRobot->isFacingLeft();
    m_velocity.setX(m_isFacingLeft ? -20 : 20);
    m_position.set(inRobot->getPosition());
    m_position.add(m_isFacingLeft ? -0.5f : 0.5f, 0.4f);
    
    setColor(inRobot->getColor());
}

Projectile::ProjectileState Projectile::getState()
{
    return m_state;
}

void Projectile::setPlayerId(uint32_t inPlayerId)
{
    m_iPlayerId = inPlayerId;
}

uint32_t Projectile::getPlayerId() const
{
    return m_iPlayerId;
}

bool Projectile::isFacingLeft()
{
    return m_isFacingLeft;
}

void Projectile::updateInternal(float inDeltaTime)
{
    Entity::update(inDeltaTime);
    
    if (m_state == ProjectileState_Active)
    {
        processCollisions();
    }
    else if (m_state == ProjectileState_Exploding)
    {
        if (m_fStateTime > 0.5f)
        {
            requestDeletion();
        }
    }
}

void Projectile::processCollisions()
{
    processCollisionsWithScreenWalls();
    
#ifdef NG_SERVER
    std::vector<Entity*> entities = World::getInstance()->getEntities();
    for (Entity* target : entities)
    {
        if (target != this && !target->isRequestingDeletion() && target->getRTTI().derivesFrom(SpacePirate::rtti))
        {
            if (OverlapTester::doNGRectsOverlap(getMainBounds(), target->getMainBounds()))
            {
                m_state = ProjectileState_Exploding;
                m_fStateTime = 0.0f;
                m_velocity.set(Vector2::Zero);
                
                SpacePirate* spacePirate = static_cast<SpacePirate*>(target);
                spacePirate->takeDamage();
            }
        }
    }
#endif
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
void Projectile::interpolateClientSidePrediction(Vector2& inOldPos)
{
    if (interpolateVectorsIfNecessary(inOldPos, getPosition(), m_fTimePositionBecameOutOfSync))
    {
        LOG("Robot ERROR! Move Replay Position");
    }
}

bool Projectile::interpolateVectorsIfNecessary(Vector2& inA, Vector2& inB, float& syncTracker)
{
    float roundTripTime = NetworkManagerClient::getInstance()->getRoundTripTime();
    
    if (!inA.isEqualTo(inB))
    {
        LOG("Robot ERROR! Move replay ended with incorrect vector! Old %3.8f, %3.8f - New %3.8f, %3.8f", inA.getX(), inA.getY(), inB.getX(), inB.getY());
        
        //have we been out of sync, or did we just become out of sync?
        float time = Timing::getInstance()->getFrameStartTime();
        if (syncTracker == 0.0f)
        {
            syncTracker = time;
        }
        
        //now interpolate to the correct value...
        float durationOutOfSync = time - syncTracker;
        if (durationOutOfSync < roundTripTime)
        {
            inB.set(lerp(inA, inB, 0.1f));
        }
        
        return true;
    }
    
    //we're in sync
    syncTracker = 0.0f;
    
    return false;
}
#endif

void Projectile::playSound(int soundId)
{
#ifdef NG_CLIENT
    float volume = 1;
    
    if (getPlayerId() != NetworkManagerClient::getInstance()->getPlayerId())
    {
        Robot* playerRobot = World::staticGetRobotWithPlayerId(NetworkManagerClient::getInstance()->getPlayerId());
        if (playerRobot)
        {
            float distance = playerRobot->getPosition().dist(getPosition());
            float factor = distance / 4.0f;
            volume = 1.0f / (factor * factor);
        }
    }
    
    NG_AUDIO_ENGINE->playSound(soundId, volume);
#endif
}

Projectile::Projectile() : Entity(0, 0, 1.565217391304348f * 0.444444444444444f, 2.0f * 0.544423440453686f),
m_iPlayerId(0),
m_state(ProjectileState_Active),
m_isFacingLeft(false),
m_fTimePositionBecameOutOfSync(0.0f)
{
    // Empty
}

RTTI_IMPL(Projectile, Entity);

NETWORK_TYPE_IMPL(Projectile);