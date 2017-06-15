//
//  Projectile.cpp
//  dante
//
//  Created by Stephen Gowen on 6/4/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Projectile.h"

#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"
#include "Server.h"

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
#include "NetworkManagerServer.h"
#include "NetworkManagerClient.h"
#include "NGAudioEngine.h"
#include "InstanceManager.h"
#include "Util.h"

#include <math.h>

Entity* Projectile::staticCreateClient()
{
    return new Projectile(nullptr);
}

Entity* Projectile::staticCreateServer()
{
    return new Projectile(Server::getInstance());
}

void Projectile::onDeletion()
{
    if (m_server)
    {
        NetworkManagerServer::getInstance()->unregisterEntity(this);
    }
}

void Projectile::update()
{
    if (m_server)
    {
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
    }
    else
    {
        updateInternal(Timing::getInstance()->getDeltaTime());
    }
}

uint32_t Projectile::getAllStateMask() const
{
    return PRJC_AllState;
}

void Projectile::read(InputMemoryBitStream& inInputStream)
{
    Vector2 oldPosition = m_position;
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
    
    if (m_server)
    {
        std::vector<Entity*> entities = InstanceManager::getServerWorld()->getEntities();
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

void Projectile::playSound(int soundId)
{
    Util::playSound(soundId, getPlayerId(), getPosition(), m_server);
}

Projectile::Projectile(Server* server) : Entity(0, 0, 1.565217391304348f * 0.444444444444444f, 2.0f * 0.544423440453686f),
m_server(server),
m_iPlayerId(0),
m_state(ProjectileState_Active),
m_isFacingLeft(false),
m_fTimePositionBecameOutOfSync(0.0f)
{
    if (m_server)
    {
        NetworkManagerServer::getInstance()->registerEntity(this);
    }
}

RTTI_IMPL(Projectile, Entity);

NETWORK_TYPE_IMPL(Projectile);
