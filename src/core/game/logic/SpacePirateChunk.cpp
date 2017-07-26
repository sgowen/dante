//
//  SpacePirateChunk.cpp
//  dante
//
//  Created by Stephen Gowen on 7/24/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SpacePirateChunk.h"

#include "Box2D/Box2D.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"

#include "Ground.h"

#include "Robot.h"
#include "Projectile.h"
#include "SpacePirate.h"
#include "NetworkManagerServer.h"
#include "MathUtil.h"
#include "Timing.h"

#include <math.h>

SpacePirateChunk::SpacePirateChunk(b2World& world, bool isServer) : Entity(world, 0.0f, 0.0f, 1.0f, 1.0f, constructEntityDef()), m_isServer(isServer), m_iType(Space_Pirate_Chunk_Top_Left), m_isFacingLeft(false)
{
    // Empty
}

EntityDef SpacePirateChunk::constructEntityDef()
{
    EntityDef ret = EntityDef();
    
    ret.isStaticBody = false;
    ret.fixedRotation = false;
    ret.restitution = 0.25f;
    
    return ret;
}

void SpacePirateChunk::update()
{
    m_fStateTime += Timing::getInstance()->getDeltaTime();
    
    if (m_fStateTime > 5)
    {
        requestDeletion();
    }
    else if (m_fStateTime > 3)
    {
        m_color.alpha = (5 - m_fStateTime) / 2.0f;
        m_color.alpha = clamp(m_color.alpha, 1, 0);
    }
    
    if (getPosition().y < DEAD_ZONE_BOTTOM
        || getPosition().x < DEAD_ZONE_LEFT
        || getPosition().x > DEAD_ZONE_RIGHT)
    {
        requestDeletion();
    }
    
    if (m_isServer)
    {
        if (!areBox2DVectorsEqual(m_velocityOld, getVelocity())
            || !areBox2DVectorsEqual(m_positionOld, getPosition()))
        {
            NG_SERVER->setStateDirty(getID(), SPCH_Pose);
        }
        
        m_velocityOld = b2Vec2(getVelocity().x, getVelocity().y);
        m_positionOld = b2Vec2(getPosition().x, getPosition().y);
    }
}

bool SpacePirateChunk::shouldCollide(Entity *inEntity)
{
    return !inEntity->getRTTI().derivesFrom(Robot::rtti);
}

void SpacePirateChunk::handleContact(Entity* inEntity)
{
    // Empty
}

uint32_t SpacePirateChunk::getAllStateMask() const
{
    return SPCH_AllState;
}

void SpacePirateChunk::read(InputMemoryBitStream& inInputStream)
{
    bool stateBit;
    
    uint32_t readState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {        
        b2Vec2 velocity;
        inInputStream.read(velocity);
        setVelocity(velocity);
        
        b2Vec2 position;
        inInputStream.read(position);
        setPosition(position);
        
        inInputStream.read(m_fWidth);
        inInputStream.read(m_fHeight);
        
        inInputStream.read(m_color);
        
        inInputStream.read(m_iType);
        inInputStream.read(m_isFacingLeft);
        
        readState |= SPCH_Pose;
    }
}

uint32_t SpacePirateChunk::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & SPCH_Pose)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(getVelocity());
        
        inOutputStream.write(getPosition());
        
        inOutputStream.write(m_fWidth);
        inOutputStream.write(m_fHeight);
        
        inOutputStream.write(m_color);
        
        inOutputStream.write(m_iType);
        inOutputStream.write((bool)m_isFacingLeft);
        
        writtenState |= SPCH_Pose;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    return writtenState;
}

void SpacePirateChunk::initFromSpacePirate(SpacePirate* spacePirate, b2Vec2 force, int type)
{
    float x = spacePirate->getPosition().x;
    float y = spacePirate->getPosition().y;
    float w = spacePirate->getWidth();
    float h = spacePirate->getHeight();
    bool flipX = spacePirate->isFacingLeft();
    m_isFacingLeft = flipX;
    m_iType = type;
    
    switch (type)
    {
        case Space_Pirate_Chunk_Top_Left:
        {
            x += flipX ? (w / 4) : (-w / 4);
            y += h / 4;
        }
            break;
        case Space_Pirate_Chunk_Top_Right:
        {
            x += flipX ? (-w / 4) : (w / 4);
            y += h / 4;
        }
            break;
        case Space_Pirate_Chunk_Bottom_Left:
        {
            x += flipX ? (w / 4) : (-w / 4);
            y -= h / 4;
        }
            break;
        case Space_Pirate_Chunk_Bottom_Right:
        {
            x += flipX ? (-w / 4) : (w / 4);
            y -= h / 4;
        }
            break;
        default:
            break;
    }
    
    Color spColor = spacePirate->getColor();
    m_color = Color(spColor.red, spColor.green, spColor.blue, spColor.alpha);
    
    setPosition(b2Vec2(x, y));
    
    m_fWidth = w / 2;
    m_fHeight = h / 2;
    
    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(m_fWidth / 2.0f, m_fHeight / 2.0f);
    
    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    
    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = 1.0f;
    
    // Override the default friction.
    fixtureDef.friction = 0.3f;
    
    m_body->DestroyFixture(m_fixture);
    
    // Add the shape to the body.
    m_fixture = m_body->CreateFixture(&fixtureDef);
    
    m_fixture->SetUserData(this);
    
    getBody()->ApplyLinearImpulseToCenter(force, true);
}

int SpacePirateChunk::getType()
{
    return m_iType;
}

bool SpacePirateChunk::isFacingLeft()
{
    return m_isFacingLeft;
}

RTTI_IMPL(SpacePirateChunk, Entity);

NW_TYPE_IMPL(SpacePirateChunk);
