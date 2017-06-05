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
#include "NGAudioEngine.h"
#endif

#include <math.h>

Entity* Projectile::create()
{
    Projectile* ret = new Projectile();
    
    ret->playSound(SOUND_ID_FIRE_ROCKET);
    
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
    ProjectileState oldState = m_state;
    
    bool stateBit;
    
    uint32_t readState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        uint32_t playerId;
        inInputStream.read(playerId);
        m_iPlayerId = playerId;
        readState |= PRJC_PlayerId;
    }
    
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
        
        inInputStream.read(m_state);
        
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
    if (oldState == ProjectileState_Active
        && m_state == ProjectileState_Exploding)
    {
        playSound(SOUND_ID_EXPLOSION);
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
        
        Vector2 velocity = m_velocity;
        inOutputStream.write(velocity.getX());
        inOutputStream.write(velocity.getY());
        
        Vector2 position = getPosition();
        inOutputStream.write(position.getX());
        inOutputStream.write(position.getY());
        
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
    m_iPlayerId = inRobot->getPlayerId();
    m_isFacingLeft = inRobot->isFacingLeft();
    m_velocity.setX(m_isFacingLeft ? -20 : 20);
    m_position.set(inRobot->getPosition());
    m_position.add(m_isFacingLeft ? -0.5f : 0.5f, 0.4f);
    
    static Color White(1.0f, 1.0f, 1.0f, 1);
    static Color Red(1.0f, 0.0f, 0.0f, 1);
    static Color Blue(0.0f, 0.0f, 1.0f, 1);
    static Color Green(0.0f, 1.0f, 0.0f, 1);
    
    switch (m_iPlayerId)
    {
        case 1:
            setColor(White);
            break;
        case 2:
            setColor(Red);
            break;
        case 3:
            setColor(Green);
            break;
        case 4:
            setColor(Blue);
            break;
        default:
            break;
    }
}

Projectile::ProjectileState Projectile::getState()
{
    return m_state;
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

void Projectile::playSound(int soundId)
{
#ifdef NG_CLIENT
    float volume = 1;
    
    NG_AUDIO_ENGINE->playSound(soundId, volume);
#endif
}

Projectile::Projectile() : Entity(0, 0, 1.565217391304348f * 0.444444444444444f, 2.0f * 0.544423440453686f),
m_iPlayerId(0),
m_state(ProjectileState_Active),
m_isFacingLeft(false)
{
    // Empty
}

RTTI_IMPL(Projectile, Entity);

NETWORK_TYPE_IMPL(Projectile);
