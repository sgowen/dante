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
#include "SpacePirate.h"
#include "SpacePirateChunk.h"
#include "Ground.h"
#include "Crate.h"

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

Projectile::Projectile(b2World& world, bool isServer) : Entity(world, 0, 0, 1.565217391304348f * 0.444444444444444f, 2.0f * 0.544423440453686f, isServer, constructEntityDef(), false),
m_iPlayerId(0),
m_isFacingLeft(false),
m_state(ProjectileState_Waiting),
m_stateLastKnown(ProjectileState_Waiting)
{
    // Empty
}

EntityDef Projectile::constructEntityDef()
{
    EntityDef ret = EntityDef();
    
    ret.isStaticBody = false;
    ret.bullet = true;
    ret.restitution = 0.5f;
    ret.density = 1.0f;
    
    return ret;
}

void Projectile::update()
{
    m_fStateTime += FRAME_RATE;
    
    if (m_state == ProjectileState_Active)
    {
        if (m_fStateTime > 0.25f)
        {
            explode();
        }
        else if (getPosition().y < DEAD_ZONE_BOTTOM
            || getPosition().x < DEAD_ZONE_LEFT
            || getPosition().x > DEAD_ZONE_RIGHT)
        {
            explode();
        }
    }
    else if (m_state == ProjectileState_Exploding)
    {
        if (m_fStateTime > 0.5f)
        {
            m_fStateTime = 0.0f;
            m_state = ProjectileState_Waiting;
            
            deinitPhysics();
        }
    }
    
    if (m_isServer)
    {
        if (m_isPhysicsOn)
        {
            NG_SERVER->setStateDirty(getID(), PRJC_Pose);
        }
    }
    else
    {
        if (m_stateLastKnown == ProjectileState_Waiting
            && m_state == ProjectileState_Active)
        {
            m_fStateTime = 0;
            
            initPhysics(constructEntityDef());
            
            // This projectile was just created
            Util::playSound(SOUND_ID_FIRE_ROCKET, getPosition(), m_isServer);
        }
        else if (m_stateLastKnown == ProjectileState_Active
            && m_state == ProjectileState_Exploding)
        {
            float stateTime = m_fStateTime;
            explode();
            m_fStateTime = stateTime;
            
            Util::playSound(SOUND_ID_EXPLOSION, getPosition(), m_isServer);
        }
        else if (m_stateLastKnown == ProjectileState_Exploding
                 && m_state == ProjectileState_Waiting)
        {
            deinitPhysics();
        }
    }
    
    if (m_isPhysicsOn)
    {
        m_velocityLastKnown = b2Vec2(getVelocity().x, getVelocity().y);
        m_positionLastKnown = b2Vec2(getPosition().x, getPosition().y);
    }
    
    m_stateLastKnown = m_state;
}

bool Projectile::shouldCollide(Entity *inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (m_state != ProjectileState_Active)
    {
        return false;
    }
    
    return inEntity->getRTTI().derivesFrom(SpacePirate::rtti)
    || inEntity->getRTTI().derivesFrom(SpacePirateChunk::rtti)
    || inEntity->getRTTI().derivesFrom(Crate::rtti)
    || inEntity->getRTTI().derivesFrom(Ground::rtti);
}

void Projectile::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (m_state != ProjectileState_Active)
    {
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
    {
        handleBeginContactWithSpacePirate(static_cast<SpacePirate*>(inEntity));
    }
    else if (inEntity->getRTTI().derivesFrom(SpacePirateChunk::rtti))
    {
        handleBeginContactWithSpacePirateChunk(static_cast<SpacePirateChunk*>(inEntity));
    }
    else if (inEntity->getRTTI().derivesFrom(Crate::rtti))
    {
        handleBeginContactWithCrate(static_cast<Crate*>(inEntity));
    }
    else if (inEntity->getRTTI().derivesFrom(Ground::rtti))
    {
        handleBeginContactWithGround(nullptr);
    }
}

void Projectile::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (m_state != ProjectileState_Active)
    {
        return;
    }
}

uint32_t Projectile::getAllStateMask() const
{
    return PRJC_AllState;
}

void Projectile::read(InputMemoryBitStream& inInputStream)
{
    bool stateBit;
    
    m_iReadState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_iPlayerId);
        inInputStream.read(m_color);
        m_iReadState |= PRJC_PlayerInfo;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        uint8_t state;
        inInputStream.read(state);
        m_state = (ProjectileState) state;
        
        inInputStream.read(m_fStateTime);
        
        inInputStream.read(m_isFacingLeft);
        
        b2Vec2 velocity;
        inInputStream.read(velocity);
        setVelocity(velocity);
        
        b2Vec2 position;
        inInputStream.read(position);
        setPosition(position);
        
        m_iReadState |= PRJC_Pose;
    }
}

uint32_t Projectile::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & PRJC_PlayerInfo)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(m_iPlayerId);
        inOutputStream.write(m_color);
        
        writtenState |= PRJC_PlayerInfo;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & PRJC_Pose)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write((uint8_t)m_state);
        
        inOutputStream.write(m_fStateTime);
        
        inOutputStream.write((bool)m_isFacingLeft);
        
        inOutputStream.write(getVelocity());
        
        inOutputStream.write(getPosition());
        
        writtenState |= PRJC_Pose;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    return writtenState;
}

bool Projectile::needsMoveReplay()
{
    return false;
}

void Projectile::initFromShooter(Robot* inRobot)
{
    m_iPlayerId = inRobot->getPlayerId();
    
    setColor(inRobot->getColor());
}

void Projectile::fire(Robot* inRobot)
{
    m_fStateTime = 0;
    m_state = ProjectileState_Active;
    
    m_isFacingLeft = inRobot->isFacingLeft();
    
    b2Vec2 position = inRobot->getPosition();
    position += b2Vec2(m_isFacingLeft ? -inRobot->getWidth() : inRobot->getWidth(), inRobot->getHeight() / 5);
    setPosition(position);
    
    initPhysics(constructEntityDef());
    
    setVelocity(b2Vec2(m_isFacingLeft ? -80 : 80, 0));
}

void Projectile::handleBeginContactWithSpacePirate(SpacePirate* inEntity)
{
    if (!m_isServer)
    {
        return;
    }
    
    float projBottom = getPosition().y - (m_fHeight / 2.0f);
    float targTop = inEntity->getPosition().y + (inEntity->getHeight() / 2.0f);
    float targHead = targTop - inEntity->getHeight() * 0.4f;
    
    explode();
    
    bool isHeadshot = projBottom > targHead;
    
    inEntity->takeDamage(b2Vec2(getVelocity().x, getVelocity().y), isHeadshot);
    if (inEntity->getHealth() == 0)
    {
        World* world = m_isServer ? InstanceManager::getServerWorld() : InstanceManager::getClientWorld();
        Robot* robot = world->getRobotWithPlayerId(getPlayerId());
        if (robot)
        {
            robot->awardKill(isHeadshot);
        }
    }
}

void Projectile::handleBeginContactWithSpacePirateChunk(SpacePirateChunk* inEntity)
{
    explode();
}

void Projectile::handleBeginContactWithCrate(Crate* inEntity)
{
    explode();
}

void Projectile::handleBeginContactWithGround(Ground* inEntity)
{
    explode();
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

void Projectile::explode()
{
    if (m_state == ProjectileState_Exploding)
    {
        return;
    }
    
    m_state = ProjectileState_Exploding;
    m_fStateTime = 0.0f;
}

RTTI_IMPL(Projectile, Entity);

NW_TYPE_IMPL(Projectile);
