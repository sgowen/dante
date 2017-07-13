//
//  Projectile.cpp
//  dante
//
//  Created by Stephen Gowen on 6/4/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Projectile.h"

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
#include "SpacePirate.h"
#include "OverlapTester.h"
#include "NetworkManagerServer.h"
#include "NetworkManagerClient.h"
#include "NGAudioEngine.h"
#include "InstanceManager.h"
#include "Util.h"

#include <math.h>

Projectile::Projectile(b2World& world, bool isServer) : Entity(world, 0, 0, 1.565217391304348f * 0.444444444444444f, 2.0f * 0.544423440453686f),
m_isServer(isServer),
m_iPlayerId(0),
m_state(ProjectileState_Active),
m_isFacingLeft(false)
{
    // Empty
}

void Projectile::update()
{
    if (m_isServer)
    {
        b2Vec2 oldVelocity = getVelocity();
        ProjectileState oldState = m_state;
        bool old_isFacingLeft = m_isFacingLeft;
        
        updateInternal(Timing::getInstance()->getDeltaTime());
        
        if (!areBox2DVectorsEqual(oldVelocity, getVelocity())
            || oldState != m_state
            || old_isFacingLeft != m_isFacingLeft)
        {
            NG_SERVER->setStateDirty(getID(), PRJC_Pose);
        }
    }
    else
    {
        updateInternal(Timing::getInstance()->getDeltaTime());
    }
}

void Projectile::handleContact(Entity* inEntity)
{
    if (inEntity != this
        && !inEntity->isRequestingDeletion())
    {
        if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
        {
            if (!m_isServer)
            {
                return;
            }
            
            float projBottom = getPosition().y - (m_fHeight / 2.0f);
            float targTop = inEntity->getPosition().y + (inEntity->getHeight() / 2.0f);
            float targHead = targTop - inEntity->getHeight() * 0.4f;
            
            m_state = ProjectileState_Exploding;
            m_fStateTime = 0.0f;
            setVelocity(b2Vec2(0.0f, 0.0f));
            
            bool isHeadshot = projBottom > targHead;
            
            SpacePirate* spacePirate = static_cast<SpacePirate*>(inEntity);
            spacePirate->takeDamage(isHeadshot);
            if (spacePirate->getHealth() == 0)
            {
                Robot* robot = InstanceManager::getServerWorld()->getRobotWithPlayerId(getPlayerId());
                robot->awardKill(isHeadshot);
            }
        }
    }
}

uint32_t Projectile::getAllStateMask() const
{
    return PRJC_AllState;
}

void Projectile::read(InputMemoryBitStream& inInputStream)
{
    ProjectileState oldState = m_state;
    
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
        
        b2Vec2 velocity;
        inInputStream.read(velocity);
        setVelocity(velocity);
        
        b2Vec2 position;
        inInputStream.read(position);
        setPosition(position);
        
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
    
    if (oldState == ProjectileState_Active
        && m_state == ProjectileState_Exploding)
    {
        playSound(SOUND_ID_EXPLOSION);
    }
    
    if ((readState & PRJC_PlayerId) != 0)
    {
        // This projectile was just created
        playSound(SOUND_ID_FIRE_ROCKET);
        
        float rtt = NG_CLIENT->getRoundTripTime() / 2;
        
        while (true)
        {
            if (rtt < FRAME_RATE)
            {
                updateInternal(rtt);
                break;
            }
            else
            {
                updateInternal(FRAME_RATE);
                rtt -= FRAME_RATE;
            }
        }
    }
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
        
        inOutputStream.write(getVelocity());
        
        inOutputStream.write(getPosition());
        
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
    
    setVelocity(b2Vec2(m_isFacingLeft ? -20 : 20, getVelocity().y));
    
    b2Vec2 position = inRobot->getPosition();
    position += b2Vec2(m_isFacingLeft ? -0.5f : 0.5f, 0.4f);
    setPosition(position);
    
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
    m_fStateTime += inDeltaTime;
    
    if (!m_isServer)
    {
        return;
    }
    
    if (m_state == ProjectileState_Exploding)
    {
        if (m_fStateTime > 0.5f)
        {
            requestDeletion();
        }
    }
}

void Projectile::playSound(int soundId)
{
    Util::playSound(soundId, getPosition(), m_isServer);
}

RTTI_IMPL(Projectile, Entity);

NETWORK_TYPE_IMPL(Projectile);
