//
//  SpacePirate.cpp
//  dante
//
//  Created by Stephen Gowen on 6/4/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SpacePirate.h"

#include "Box2D/Box2D.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"
#include "Robot.h"
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
#include "OverlapTester.h"
#include "NetworkManagerServer.h"
#include "NetworkManagerClient.h"
#include "InstanceManager.h"
#include "Projectile.h"
#include "Util.h"
#include "EntityRegistry.h"
#include "FWInstanceManager.h"
#include "SpacePirateChunk.h"

#include <math.h>

SpacePirate::SpacePirate(b2World& world, bool isServer) : Entity(world, 0, 0, 2.0f, 2.347826086956522f, isServer, constructEntityDef()),
m_fSpeed(0.0),
m_iHealth(8),
m_fStartingHealth(8),
m_isFacingLeft(false),
m_isJumping(false),
m_fJumpCooldown(0.0f),
m_fJumpSpeed(7.0f),
m_iHealthLeftLastKnown(8)
{
    m_fJumpCooldown = Timing::getInstance()->getFrameStartTime();
    m_fJumpCooldown += (rand() % 100) * 0.1f + 1.0f;
}

EntityDef SpacePirate::constructEntityDef()
{
    EntityDef ret = EntityDef();
    
    ret.isStaticBody = false;
    ret.restitution = 0.1f;
    ret.isCharacter = true;
    ret.density = 32.0f;
    
    return ret;
}

void SpacePirate::update()
{
    m_fStateTime += FRAME_RATE;
    
    if (m_isServer)
    {
        if (getPosition().y < DEAD_ZONE_BOTTOM)
        {
            m_iHealth = 0;
        }
        
        if (m_iHealth == 0 && !isRequestingDeletion())
        {
            requestDeletion();
            
            for (int i = 0; i < 4; ++i)
            {
                SpacePirateChunk* chunk = static_cast<SpacePirateChunk*>(SERVER_ENTITY_REG->createEntity(NW_TYPE_SpacePirateChunk));
                chunk->initFromSpacePirate(this, m_lastForce, i);
            }
        }
        
        if (isGrounded())
        {
            m_fJumpCooldown -= FRAME_RATE;
            
            if (m_fJumpCooldown < 0)
            {
                m_fJumpCooldown += (rand() % 100) * 0.1f + 1.0f;
                
                m_isJumping = true;
                
                setVelocity(b2Vec2(getVelocity().x, m_fJumpSpeed));
            }
        }
        
        bool targetFound = false;
        
        float shortestDistance = GAME_WIDTH * 3;
        std::vector<Entity*> players = InstanceManager::getServerWorld()->getPlayers();
        for (Entity* entity : players)
        {
            Robot* robot = static_cast<Robot*>(entity);
            
            if (!robot->isRequestingDeletion())
            {
                float dist = b2Distance(robot->getPosition(), getPosition());
                if (dist < shortestDistance)
                {
                    shortestDistance = dist;
                    m_isFacingLeft = robot->getPosition().x < getPosition().x;
                    
                    setVelocity(b2Vec2(m_isFacingLeft ? -m_fSpeed : m_fSpeed, getVelocity().y));
                    
                    targetFound = true;
                }
            }
        }
        
        if (!targetFound)
        {
            m_isFacingLeft = true;
            
            setVelocity(b2Vec2(0, getVelocity().y));
        }
        
        NG_SERVER->setStateDirty(getID(), SPCP_Pose);
        
        if (m_iHealthLeftLastKnown != m_iHealth)
        {
            NG_SERVER->setStateDirty(getID(), SPCP_Health);
        }
    }
    
    m_velocityLastKnown = b2Vec2(getVelocity().x, getVelocity().y);
    m_positionLastKnown = b2Vec2(getPosition().x, getPosition().y);
    m_iHealthLeftLastKnown = m_iHealth;
}

bool SpacePirate::shouldCollide(Entity *inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == m_footSensorFixture)
    {
        return inEntity->getRTTI().derivesFrom(Ground::rtti)
        || inEntity->getRTTI().derivesFrom(Crate::rtti);
    }
    
    return inEntity->getRTTI().derivesFrom(Robot::rtti)
    || inEntity->getRTTI().derivesFrom(Projectile::rtti)
    || inEntity->getRTTI().derivesFrom(Ground::rtti)
    || inEntity->getRTTI().derivesFrom(Crate::rtti);
}

void SpacePirate::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == m_footSensorFixture)
    {
        m_isJumping = false;
        
        handleBeginFootContact(inEntity);
        
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
    {
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(Robot::rtti)
        || inEntity->getRTTI().derivesFrom(Projectile::rtti))
    {
        inEntity->handleBeginContact(this, inFixtureB, inFixtureA);
    }
}

void SpacePirate::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == m_footSensorFixture)
    {
        if (inEntity->getRTTI().derivesFrom(Ground::rtti)
            || inEntity->getRTTI().derivesFrom(Crate::rtti))
        {
            handleEndFootContact(inEntity);
        }
        
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
    {
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(Robot::rtti)
        || inEntity->getRTTI().derivesFrom(Projectile::rtti))
    {
        inEntity->handleEndContact(this, inFixtureB, inFixtureA);
    }
}

uint32_t SpacePirate::getAllStateMask() const
{
    return SPCP_AllState;
}

void SpacePirate::read(InputMemoryBitStream& inInputStream)
{
    float oldWidth = m_fWidth;
    
    bool stateBit;
    
    m_iReadState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_color);
        
        inInputStream.read(m_fWidth);
        inInputStream.read(m_fHeight);
        m_iReadState |= SPCP_Info;
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
        
        inInputStream.read(m_isFacingLeft);
        inInputStream.read(m_isJumping);
        
        m_iReadState |= SPCP_Pose;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_iHealth);
        m_iReadState |= SPCP_Health;
    }
    
    if (!areFloatsPracticallyEqual(oldWidth, m_fWidth))
    {
        initPhysics(constructEntityDef());
    }
}

uint32_t SpacePirate::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & SPCP_Info)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(m_color);
        
        inOutputStream.write(m_fWidth);
        inOutputStream.write(m_fHeight);
        
        writtenState |= SPCP_Info;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & SPCP_Pose)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(m_fStateTime);
        
        inOutputStream.write(getVelocity());
        
        inOutputStream.write(getPosition());
        
        inOutputStream.write((bool)m_isFacingLeft);
        inOutputStream.write((bool)m_isJumping);
        
        writtenState |= SPCP_Pose;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & SPCP_Health)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(m_iHealth);
        
        writtenState |= SPCP_Health;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    return writtenState;
}

bool SpacePirate::needsMoveReplay()
{
    return (m_iReadState & SPCP_Pose) != 0;
}

void SpacePirate::init(float x, float y, float speed, int scale, uint8_t health)
{
    setPosition(b2Vec2(x, y));
    
    m_fSpeed = speed;
    
    m_fWidth *= scale;
    m_fHeight *= scale;
    
    m_iHealth = health;
    m_fStartingHealth = m_iHealth;
    
    initPhysics(constructEntityDef());
}

void SpacePirate::takeDamage(b2Vec2 force, bool isHeadshot)
{
    m_lastForce.Set(force.x, force.y);
    m_iHealth -= isHeadshot ? 8 : 1;
    if (m_iHealth > m_fStartingHealth)
    {
        // We are using unsigned values for health
        m_iHealth = 0;
    }
}

uint8_t SpacePirate::getHealth()
{
    return m_iHealth;
}

float SpacePirate::getSpeed()
{
    return m_fSpeed;
}

bool SpacePirate::isFacingLeft()
{
    return m_isFacingLeft;
}

RTTI_IMPL(SpacePirate, Entity);

NW_TYPE_IMPL(SpacePirate);
