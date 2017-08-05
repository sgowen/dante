//
//  Robot.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Robot.h"

#include "Box2D/Box2D.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"
#include "InputState.h"
#include "Move.h"
#include "Ground.h"
#include "Crate.h"

#include "World.h"
#include "macros.h"
#include "GameConstants.h"
#include "Timing.h"
#include "StringUtil.h"
#include "MathUtil.h"
#include "NGRect.h"
#include "Projectile.h"
#include "EntityRegistry.h"
#include "NetworkManagerServer.h"
#include "ClientProxy.h"
#include "NetworkManagerClient.h"
#include "InputManager.h"
#include "NGAudioEngine.h"
#include "InstanceManager.h"
#include "FWInstanceManager.h"
#include "Util.h"
#include "Server.h"
#include "Ground.h"
#include "SpacePirate.h"
#include "SpacePirateChunk.h"

#include <math.h>

Robot::Robot(b2World& world, bool isServer) : Entity(world, 0, 0, 1.565217391304348f, 2.0f, constructEntityDef()),
m_iAddressHash(0),
m_iPlayerId(0),
m_playerName("Robot"),
m_iNumJumps(0),
m_iHealth(25),
m_iNumKills(0),
m_wasLastKillHeadshot(false),
m_isFacingLeft(false),
m_isShooting(false),
m_isSprinting(false),
m_iReadState(0),
m_fSpeed(7.5f),
m_fJumpSpeed(11.0f),
m_fTimeOfNextShot(0.0f),
m_isServer(isServer),
m_isFirstJumpCompleted(false),
m_velocityLastKnown(),
m_positionLastKnown(),
m_iNumJumpsLastKnown(0),
m_iHealthLastKnown(25),
m_iNumKillsLastKnown(0),
m_wasLastKillHeadshotLastKnown(false),
m_isFacingLeftLastKnown(false),
m_isShootingLastKnown(false),
m_isSprintingLastKnown(false)
{
    // Empty
}

Robot::~Robot()
{
    // Empty
}

EntityDef Robot::constructEntityDef()
{
    EntityDef ret = EntityDef();
    
    ret.isStaticBody = false;
    ret.restitution = 0.1f;
    ret.isCharacter = true;
    
    return ret;
}

void Robot::update()
{
    if (m_isServer)
    {
        handleShooting();
        
        if (!areBox2DVectorsEqual(m_velocityLastKnown, getVelocity())
            || !areBox2DVectorsEqual(m_positionLastKnown, getPosition())
            || m_iNumJumpsLastKnown != m_iNumJumps
            || m_iHealthLastKnown != m_iHealth
            || m_iNumKillsLastKnown != m_iNumKills
            || m_wasLastKillHeadshotLastKnown != m_wasLastKillHeadshot
            || m_isFacingLeftLastKnown != m_isFacingLeft
            || m_isShootingLastKnown != m_isShooting
            || m_isSprintingLastKnown != m_isSprinting)
        {
            NG_SERVER->setStateDirty(getID(), ROBT_Pose);
        }
    }
    else
    {
        if (NG_CLIENT->isPlayerIdLocal(getPlayerId()))
        {
            if (m_iNumKills > m_iNumKillsLastKnown && m_wasLastKillHeadshot)
            {
                playSound(SOUND_ID_HEADSHOT);
            }
        }
        else
        {
            playNetworkBoundSounds(m_iNumJumpsLastKnown, m_isSprintingLastKnown);
        }
    }
    
    if (m_iHealth == 0 && !isRequestingDeletion())
    {
        playSound(SOUND_ID_DEATH);
        
        // TODO, this is NOT the right way to handle the player dying
        
        requestDeletion();
        
        if (m_isServer)
        {
            Server::sHandleNewClient(m_iPlayerId, m_playerName);
        }
    }
    
    m_velocityLastKnown = b2Vec2(getVelocity().x, getVelocity().y);
    m_positionLastKnown = b2Vec2(getPosition().x, getPosition().y);
    m_iNumJumpsLastKnown = m_iNumJumps;
    m_iHealthLastKnown = m_iHealth;
    m_iNumKillsLastKnown = m_iNumKills;
    m_wasLastKillHeadshotLastKnown = m_wasLastKillHeadshot;
    m_isFacingLeftLastKnown = m_isFacingLeft;
    m_isShootingLastKnown = m_isShooting;
    m_isSprintingLastKnown = m_isSprinting;
}

bool Robot::shouldCollide(Entity *inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == m_footSensorFixture)
    {
        return inEntity->getRTTI().derivesFrom(Ground::rtti)
        || inEntity->getRTTI().derivesFrom(Crate::rtti);
    }
    
    return inEntity->getRTTI().derivesFrom(SpacePirate::rtti)
    || inEntity->getRTTI().derivesFrom(Ground::rtti)
    || inEntity->getRTTI().derivesFrom(Crate::rtti);
}

void Robot::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inFixtureA == m_footSensorFixture)
    {
        m_iNumJumps = 0;
        
        handleBeginFootContact(inEntity);
        
        return;
    }
    
    if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
    {
        (static_cast<SpacePirate*>(inEntity))->handleBeginContactWithRobot(this);
    }
}

void Robot::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
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
}

uint32_t Robot::getAllStateMask() const
{
    return ROBT_AllState;
}

void Robot::read(InputMemoryBitStream& inInputStream)
{
    bool stateBit;
    
    m_iReadState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_iAddressHash);
        inInputStream.read(m_iPlayerId);
        inInputStream.read(m_playerName);
        inInputStream.read(m_color);
        
        m_iReadState |= ROBT_PlayerInfo;
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
        
        inInputStream.read(m_iNumJumps);
        
        inInputStream.read(m_iHealth);
        
        inInputStream.read(m_iNumKills);
        inInputStream.read(m_wasLastKillHeadshot);
        
        inInputStream.read(m_isFacingLeft);
        inInputStream.read(m_isShooting);
        inInputStream.read(m_isSprinting);
        
        m_iReadState |= ROBT_Pose;
    }
}

uint32_t Robot::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & ROBT_PlayerInfo)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(m_iAddressHash);
        inOutputStream.write(m_iPlayerId);
        inOutputStream.write(m_playerName);
        inOutputStream.write(m_color);
        
        writtenState |= ROBT_PlayerInfo;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & ROBT_Pose)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(m_fStateTime);
        
        inOutputStream.write(getVelocity());
        
        inOutputStream.write(getPosition());
        
        inOutputStream.write(m_iNumJumps);
        
        inOutputStream.write(m_iHealth);
        
        inOutputStream.write(m_iNumKills);
        inOutputStream.write((bool)m_wasLastKillHeadshot);
        
        inOutputStream.write((bool)m_isFacingLeft);
        inOutputStream.write((bool)m_isShooting);
        inOutputStream.write((bool)m_isSprinting);
        
        writtenState |= ROBT_Pose;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    return writtenState;
}

void Robot::postRead()
{
    if (NG_CLIENT->isPlayerIdLocal(getPlayerId()))
    {
        // Only interpolate when new pose has been read in
        if ((m_iReadState & ROBT_Pose) != 0)
        {
            interpolateClientSidePrediction(m_velocityLastKnown, m_positionLastKnown);
        }
    }
    
    m_iReadState = 0;
}

void Robot::processInput(IInputState* inInputState, bool isPending)
{
    InputState* is = static_cast<InputState*>(inInputState);
    uint8_t playerId = getPlayerId();
    InputState::GameInputState* inputState = is->getGameInputStateForPlayerId(playerId);
    if (inputState == nullptr)
    {
        return;
    }
    
    int numJumpsLastKnown = m_iNumJumps;
    bool isSprintingLastKnown = m_isSprinting;
    
    b2Vec2 velocity = b2Vec2(getVelocity().x, getVelocity().y);
    
    velocity.Set(inputState->getDesiredRightAmount() * m_fSpeed, getVelocity().y);
    
    m_isFacingLeft = velocity.x < 0 ? true : velocity.x > 0 ? false : m_isFacingLeft;
    
    m_isSprinting = inputState->isSprinting();
    
    if (m_isSprinting)
    {
        velocity += b2Vec2(inputState->getDesiredRightAmount() * m_fSpeed / 2, 0);
    }
    
    if (inputState->isJumping())
    {
        if (isGrounded() && areFloatsPracticallyEqual(getAngle(), 0))
        {
            m_fStateTime = 0;
            m_isFirstJumpCompleted = false;
            m_iNumJumps = 1;
            
            velocity.Set(velocity.x, m_fJumpSpeed);
        }
        else if (m_iNumJumps == 1 && m_isFirstJumpCompleted)
        {
            m_fStateTime = 0;
            m_iNumJumps = 2;
            
            velocity.Set(velocity.x, m_fJumpSpeed - 3);
        }
    }
    else
    {
        if (m_iNumJumps == 1)
        {
            m_isFirstJumpCompleted = true;
            
            if (velocity.y > (m_fJumpSpeed / 2.0f))
            {
                velocity.Set(velocity.x, m_fJumpSpeed / 2.0f);
            }
        }
        
        if (m_iNumJumps == 2 && velocity.y > ((m_fJumpSpeed - 2) / 2.0f))
        {
            velocity.Set(velocity.x, (m_fJumpSpeed - 2) / 2.0f);
        }
    }
    
    m_isShooting = inputState->isShooting();
    
    setVelocity(velocity);
    
    if (isPending)
    {
        playNetworkBoundSounds(numJumpsLastKnown, isSprintingLastKnown);
    }
}

void Robot::updateInternal(float inDeltaTime)
{
    m_fStateTime += inDeltaTime;
    
    if (isGrounded())
    {
        setAngle(0);
    }
    
    if (getVelocity().y < 0 && !isFalling() && m_iNumJumps > 0)
    {
        m_fStateTime = 0;
    }
    
    if (getPosition().y < DEAD_ZONE_BOTTOM)
    {
        m_iHealth = 0;
    }
}

void Robot::takeDamage()
{
    if (m_iHealth > 0)
    {
        m_iHealth--;
    }
}

void Robot::awardKill(bool isHeadshot)
{
    if (!m_isServer)
    {
        return;
    }
    
    m_iNumKills++;
    
    m_wasLastKillHeadshot = isHeadshot;
}

void Robot::setAddressHash(uint64_t addressHash)
{
    m_iAddressHash = addressHash;
}

uint64_t Robot::getAddressHash() const
{
    return m_iAddressHash;
}

void Robot::setPlayerId(uint8_t inPlayerId)
{
    m_iPlayerId = inPlayerId;
}

uint8_t Robot::getPlayerId() const
{
    return m_iPlayerId;
}

void Robot::setPlayerName(std::string playerName)
{
    m_playerName = playerName;
}

std::string& Robot::getPlayerName()
{
    return m_playerName;
}

uint32_t Robot::getNumKills()
{
    return m_iNumKills;
}

uint8_t Robot::getHealth()
{
    return m_iHealth;
}

bool Robot::isFacingLeft()
{
    return m_isFacingLeft;
}

bool Robot::isShooting()
{
    return m_isShooting;
}

bool Robot::isSprinting()
{
    return m_isSprinting;
}

bool Robot::needsMoveReplay()
{
    return m_iReadState > 0;
}

void Robot::handleShooting()
{
    if (!m_isServer)
    {
        return;
    }
    
    float time = Timing::getInstance()->getFrameStartTime();
    
    if (m_isShooting && time > m_fTimeOfNextShot)
    {
        // not exact, but okay
        m_fTimeOfNextShot = time + FRAME_RATE * 4;
        
        // fire!
        Projectile* projectile = static_cast<Projectile*>(SERVER_ENTITY_REG->createEntity(NW_TYPE_Projectile));
        projectile->initFromShooter(this);
    }
}

void Robot::playNetworkBoundSounds(int numJumpsLastKnown, bool isSprintingLastKnown)
{
    if (numJumpsLastKnown != m_iNumJumps && m_iNumJumps > 0)
    {
        playSound(SOUND_ID_ROBOT_JUMP);
    }
    
    if (!isSprintingLastKnown && m_isSprinting)
    {
        playSound(SOUND_ID_ACTIVATE_SPRINT);
    }
}

void Robot::playSound(int soundId)
{
    Util::playSound(soundId, getPosition(), m_isServer);
}

RTTI_IMPL(Robot, Entity);

NW_TYPE_IMPL(Robot);
