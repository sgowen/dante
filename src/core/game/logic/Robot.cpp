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

#include <math.h>

Robot::Robot(b2World& world, bool isServer) : Entity(world, 0, 0, 1.565217391304348f, 2.0f),
m_iAddressHash(0),
m_iPlayerId(0),
m_playerName("Robot"),
m_iNumJumps(0),
m_isFacingLeft(false),
m_isShooting(false),
m_isSprinting(false),
m_iHealth(25),
m_iNumKills(0),
m_wasLastKillHeadshot(false),
m_fSpeed(7.5f),
m_fJumpSpeed(11.0f),
m_fTimeOfNextShot(0.0f),
m_fRobotRestitution(0.1f),
m_fTimeAccelerationBecameOutOfSync(0.0f),
m_fTimeVelocityBecameOutOfSync(0.0f),
m_fTimePositionBecameOutOfSync(0.0f),
m_isServer(isServer),
m_isGrounded(false),
m_isFalling(false),
m_isFirstJumpCompleted(false)
{
    // Empty
}

void Robot::update()
{
    if (m_isServer)
    {
        b2Vec2 oldVelocity = b2Vec2(getVelocity().x, getVelocity().y);
        b2Vec2 oldPosition = b2Vec2(getPosition().x, getPosition().y);
        uint8_t oldNumJumps = m_iNumJumps;
        bool old_m_isGrounded = m_isGrounded;
        bool old_m_isFalling = m_isFalling;
        bool old_isFacingLeft = m_isFacingLeft;
        bool old_isShooting = m_isShooting;
        bool old_isSprinting = m_isSprinting;
        
        // is there a move we haven't processed yet?
        ClientProxy* client = NG_SERVER->getClientProxy(getPlayerId());
        if (client)
        {
            MoveList& moveList = client->getUnprocessedMoveList();
            for (const Move& unprocessedMove : moveList)
            {
                processMove(unprocessedMove);
            }
        }
        
        handleShooting();
        
        if (!areBox2DVectorsEqual(oldVelocity, getVelocity())
            || !areBox2DVectorsEqual(oldPosition, getPosition())
            || oldNumJumps != m_iNumJumps
            || old_m_isGrounded != m_isGrounded
            || old_m_isFalling != m_isFalling
            || old_isFacingLeft != m_isFacingLeft
            || old_isShooting != m_isShooting
            || old_isSprinting != m_isSprinting)
        {
            NG_SERVER->setStateDirty(getID(), ROBT_Pose);
        }
    }
    else
    {
        if (NG_CLIENT->isPlayerIdLocal(getPlayerId()))
        {
            const Move* pendingMove = InputManager::getInstance()->getPendingMove();
            if (pendingMove)
            {
                processMove(*pendingMove);
            }
        }
        else
        {
            updateInternal(Timing::getInstance()->getDeltaTime());
        }
    }
}

void Robot::handleContact(Entity* inEntity)
{
    if (inEntity != this
        && !inEntity->isRequestingDeletion())
    {
        if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
        {
            (static_cast<SpacePirate*>(inEntity))->handleContactWithRobot(this);
        }
        else if (inEntity->getRTTI().derivesFrom(Ground::rtti))
        {
            handleContactWithGround(nullptr);
        }
    }
}

uint32_t Robot::getAllStateMask() const
{
    return ROBT_AllState;
}

void Robot::read(InputMemoryBitStream& inInputStream)
{
    b2Vec2 oldVelocity = b2Vec2(getVelocity().x, getVelocity().y);
    b2Vec2 oldPosition = b2Vec2(getPosition().x, getPosition().y);
    uint8_t old_m_iNumJumps = m_iNumJumps;
    bool old_m_isSprinting = m_isSprinting;
    uint32_t old_m_iNumKills = m_iNumKills;
    
    bool stateBit;
    
    uint32_t readState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_iAddressHash);
        inInputStream.read(m_iPlayerId);
        inInputStream.read(m_playerName);
        readState |= ROBT_PlayerInfo;
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
        
        inInputStream.read(m_isGrounded);
        inInputStream.read(m_isFalling);
        inInputStream.read(m_isFacingLeft);
        inInputStream.read(m_isShooting);
        inInputStream.read(m_isSprinting);
        
        readState |= ROBT_Pose;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_color);
        readState |= ROBT_Color;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_iHealth);
        readState |= ROBT_Health;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_iNumKills);
        inInputStream.read(m_wasLastKillHeadshot);
        readState |= ROBT_NumKills;
    }
    
    if (NG_CLIENT->isPlayerIdLocal(getPlayerId()))
    {
        doClientSidePredictionForLocalRobot(readState);
        
        // if this is a create packet, don't interpolate
        if ((readState & ROBT_PlayerInfo) == 0)
        {
            interpolateClientSidePrediction(oldVelocity, oldPosition);
        }
        else
        {
            playSound(SOUND_ID_DEATH);
        }
        
        if (m_wasLastKillHeadshot && m_iNumKills != old_m_iNumKills)
        {
            NGAudioEngine::getInstance()->playSound(SOUND_ID_HEADSHOT);
        }
    }
    else
    {
        doClientSidePredictionForRemoteRobot(readState);
        
        playNetworkBoundSounds(old_m_iNumJumps, old_m_isSprinting);
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
        
        inOutputStream.write((bool)m_isGrounded);
        inOutputStream.write((bool)m_isFalling);
        inOutputStream.write((bool)m_isFacingLeft);
        inOutputStream.write((bool)m_isShooting);
        inOutputStream.write((bool)m_isSprinting);
        
        writtenState |= ROBT_Pose;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & ROBT_Color)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(m_color);
        
        writtenState |= ROBT_Color;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & ROBT_Health)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(m_iHealth);
        
        writtenState |= ROBT_Health;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & ROBT_NumKills)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(m_iNumKills);
        inOutputStream.write((bool)m_wasLastKillHeadshot);
        
        writtenState |= ROBT_NumKills;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    return writtenState;
}

void Robot::handleContactWithGround(Ground* ground)
{
    m_iNumJumps = 0;
    m_isGrounded = true;
    m_isFalling = false;
}

void Robot::takeDamage()
{
    if (!m_isServer)
    {
        return;
    }
    
    if (m_iHealth > 0)
    {
        m_iHealth--;
    }
    
    // tell the world our health dropped
    NG_SERVER->setStateDirty(getID(), ROBT_Health);
}

void Robot::awardKill(bool isHeadshot)
{
    if (!m_isServer)
    {
        return;
    }
    
    m_iNumKills++;
    
    m_wasLastKillHeadshot = isHeadshot;
    
    // tell the world how bad ass we are
    NG_SERVER->setStateDirty(getID(), ROBT_NumKills);
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

bool Robot::isGrounded()
{
    return m_isGrounded;
}

bool Robot::isShooting()
{
    return m_isShooting;
}

bool Robot::isSprinting()
{
    return m_isSprinting;
}

void Robot::processMove(const Move& inMove)
{
    uint8_t old_m_iNumJumps = m_iNumJumps;
    bool old_m_isSprinting = m_isSprinting;
    
    IInputState* currentState = inMove.getInputState();
    
    float deltaTime = inMove.getDeltaTime();
    
    processInput(deltaTime, currentState);
    
    updateInternal(deltaTime);
    
    playNetworkBoundSounds(old_m_iNumJumps, old_m_isSprinting);
}

void Robot::processInput(float inDeltaTime, IInputState* inInputState)
{
    InputState* is = static_cast<InputState*>(inInputState);
    uint8_t playerId = getPlayerId();
    InputState::GameInputState* inputState = is->getGameInputStateForPlayerId(playerId);
    if (inputState == nullptr)
    {
        return;
    }
    
    b2Vec2 velocity = b2Vec2(inputState->getDesiredRightAmount() * m_fSpeed, getVelocity().y);
    
    m_isFacingLeft = velocity.x < 0 ? true : velocity.x > 0 ? false : m_isFacingLeft;
    
    m_isSprinting = inputState->isSprinting();
    
    if (m_isSprinting && m_isGrounded)
    {
        velocity += b2Vec2(inputState->getDesiredRightAmount() * m_fSpeed / 2, 0);
    }
    
    if (inputState->isJumping())
    {
        if (m_isGrounded && areFloatsPracticallyEqual(getAngle(), 0))
        {
            m_fStateTime = 0;
            m_isGrounded = false;
            m_isFalling = false;
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
}

void Robot::updateInternal(float inDeltaTime)
{
    m_fStateTime += inDeltaTime;
    
    if (m_isGrounded)
    {
        setAngle(0);
    }
    
    if (getVelocity().y < 0 && !m_isFalling)
    {
        m_isFalling = true;
        m_fStateTime = 0;
    }
    
    if (!m_isServer)
    {
        return;
    }
    
    if (getPosition().y < -5)
    {
        if (m_iHealth > 0)
        {
            m_iHealth = 0;
        }
    }
    
    if (m_iHealth == 0)
    {
        // TODO, this is NOT the right way to handle the player dying
        
        m_iHealth = 255;
        
        requestDeletion();
        
        Server::sHandleNewClient(m_iPlayerId, m_playerName);
    }
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
        m_fTimeOfNextShot = time + 0.05f;
        
        // fire!
        Projectile* projectile = static_cast<Projectile*>(SERVER_ENTITY_REG->createEntity(NETWORK_TYPE_Projectile));
        projectile->initFromShooter(this);
    }
}

void Robot::doClientSidePredictionForLocalRobot(uint32_t inReadState)
{
    if ((inReadState & ROBT_Pose) != 0)
    {
        // simulate pose only if we received new pose
        // all processed moves have been removed, so all that are left are unprocessed moves
        // so we must apply them...
        MoveList& moveList = InputManager::getInstance()->getMoveList();
        
        for (const Move& move : moveList)
        {
            float deltaTime = move.getDeltaTime();
            processInput(deltaTime, move.getInputState());
            
            updateInternal(deltaTime);
        }
    }
}

void Robot::doClientSidePredictionForRemoteRobot(uint32_t inReadState)
{
    // so what do we want to do here? need to do some kind of interpolation...
    
    if ((inReadState & ROBT_Pose) != 0)
    {
        // simulate movement for an additional RTT/2
        float rtt = NG_CLIENT->getRoundTripTime() / 2;
        
        // break into framerate sized chunks so we don't run through walls and do crazy things...
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

void Robot::interpolateClientSidePrediction(b2Vec2& inOldVelocity, b2Vec2& inOldPos)
{
    if (interpolateVectorsIfNecessary(inOldVelocity, getVelocity(), m_fTimeVelocityBecameOutOfSync, "velocity"))
    {
        setVelocity(inOldVelocity);
    }
    
    if (interpolateVectorsIfNecessary(inOldPos, getPosition(), m_fTimePositionBecameOutOfSync, "position"))
    {
        setPosition(inOldPos);
    }
}

bool Robot::interpolateVectorsIfNecessary(b2Vec2& inOld, const b2Vec2& inNew, float& syncTracker, const char* vectorType)
{
    float rtt = NG_CLIENT->getRoundTripTime();
    rtt /= 2; // Let's just measure the time from server to us
    
    if (!areBox2DVectorsEqual(inOld, inNew))
    {
        LOG("WARN: Robot move replay ended with incorrect %s! Old %3.8f, %3.8f - New %3.8f, %3.8f", vectorType, inOld.x, inOld.y, inNew.x, inNew.y);
        
        //have we been out of sync, or did we just become out of sync?
        float time = Timing::getInstance()->getFrameStartTime();
        if (syncTracker == 0.0f)
        {
            syncTracker = time;
        }
        
        //now interpolate to the correct value...
        float durationOutOfSync = time - syncTracker;
        if (durationOutOfSync < rtt)
        {
            b2Vec2 vec = lerpBox2DVector(inOld, inNew, rtt);
            inOld.Set(vec.x, vec.y);
            
            return true;
        }
    }
    else
    {
        syncTracker = 0.0f;
    }
    
    return false;
}

void Robot::playNetworkBoundSounds(uint8_t old_m_iNumJumps, bool old_m_isSprinting)
{
    if (old_m_iNumJumps != m_iNumJumps && m_iNumJumps > 0)
    {
        playSound(SOUND_ID_ROBOT_JUMP);
    }
    
    if (!old_m_isSprinting && m_isSprinting)
    {
        playSound(SOUND_ID_ACTIVATE_SPRINT);
    }
}

void Robot::playSound(int soundId)
{
    Util::playSound(soundId, getPosition(), m_isServer);
}

RTTI_IMPL(Robot, Entity);

NETWORK_TYPE_IMPL(Robot);
