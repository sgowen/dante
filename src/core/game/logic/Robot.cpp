//
//  Robot.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Robot.h"

#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"
#include "InputState.h"
#include "Move.h"

#include "World.h"
#include "Vector2.h"
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

#include <math.h>

Entity* Robot::staticCreateClient()
{
    return new Robot(false);
}

Entity* Robot::staticCreateServer()
{
    return new Robot(true);
}

void Robot::onDeletion()
{
    if (m_isServer)
    {
        NG_SERVER->unregisterEntity(this);
    }
    else
    {
        if (getPlayerId() == NG_CLIENT->getPlayerId())
        {
            // This robot is the current local player, so let's display something like "Respawning in 5, 4, 3..."
            playSound(SOUND_ID_DEATH);
        }
    }
}

void Robot::update()
{
    if (m_isServer)
    {
        Vector2 oldAcceleration = getAcceleration();
        Vector2 oldVelocity = getVelocity();
        Vector2 oldPosition = getPosition();
        uint8_t oldNumJumps = m_iNumJumps;
        bool old_isFacingLeft = m_isFacingLeft;
        bool old_isGrounded = m_isGrounded;
        bool old_isFalling = m_isFalling;
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
            
            moveList.clear();
        }
        
        handleShooting();
        
        if (!oldAcceleration.isEqualTo(getAcceleration())
            || !oldVelocity.isEqualTo(getVelocity())
            || !oldPosition.isEqualTo(getPosition())
            || oldNumJumps != m_iNumJumps
            || old_isFacingLeft != m_isFacingLeft
            || old_isGrounded != m_isGrounded
            || old_isFalling != m_isFalling
            || old_isShooting != m_isShooting
            || old_isSprinting != m_isSprinting)
        {
            NG_SERVER->setStateDirty(getID(), ROBT_Pose);
        }
    }
    else
    {
        // TODO, allow for multiple client inputs
        if (getPlayerId() == NG_CLIENT->getPlayerId())
        {
            const Move* pendingMove = InputManager::getInstance()->getAndClearPendingMove();
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

uint32_t Robot::getAllStateMask() const
{
    return ROBT_AllState;
}

void Robot::read(InputMemoryBitStream& inInputStream)
{
    Vector2 oldAcceleration = m_acceleration;
    Vector2 oldVelocity = m_velocity;
    Vector2 oldPosition = m_position;
    uint8_t oldNumJumps = m_iNumJumps;
    bool wasSprinting = m_isSprinting;
    
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
        
        inInputStream.read(m_acceleration.getXRef());
        inInputStream.read(m_acceleration.getYRef());
        
        inInputStream.read(m_velocity.getXRef());
        inInputStream.read(m_velocity.getYRef());
        
        inInputStream.read(m_position.getXRef());
        inInputStream.read(m_position.getYRef());
        
        inInputStream.read(m_iNumJumps);
        
        inInputStream.read(m_isFacingLeft);
        inInputStream.read(m_isGrounded);
        inInputStream.read(m_isFalling);
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
        readState |= ROBT_NumKills;
    }
    
    if (getPlayerId() == NG_CLIENT->getPlayerId())
    {
        doClientSidePredictionForLocalRobot(readState);
        
        // if this is a create packet, don't interpolate
        if ((readState & ROBT_PlayerInfo) == 0)
        {
            interpolateClientSidePrediction(oldAcceleration, oldVelocity, oldPosition);
        }
    }
    else
    {
        doClientSidePredictionForRemoteRobot(readState);
        
        // if this is a create packet, don't interpolate
        if ((readState & ROBT_PlayerInfo) == 0)
        {
            interpolateVectorsIfNecessary(oldPosition, getPosition(), m_fTimePositionBecameOutOfSync, "remote position");
        }
        
        if (m_iNumJumps && !oldNumJumps)
        {
            playSound(SOUND_ID_ROBOT_JUMP);
        }
        
        if (m_isSprinting && !wasSprinting)
        {
            playSound(SOUND_ID_ACTIVATE_SPRINT);
        }
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
        
        inOutputStream.write(m_acceleration.getX());
        inOutputStream.write(m_acceleration.getY());
        
        inOutputStream.write(m_velocity.getX());
        inOutputStream.write(m_velocity.getY());
        
        inOutputStream.write(m_position.getX());
        inOutputStream.write(m_position.getY());
        
        inOutputStream.write(m_iNumJumps);
        
        inOutputStream.write((bool)m_isFacingLeft);
        inOutputStream.write((bool)m_isGrounded);
        inOutputStream.write((bool)m_isFalling);
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
        
        writtenState |= ROBT_NumKills;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    return writtenState;
}

void Robot::takeDamage()
{
    if (m_isServer)
    {
        m_iHealth--;
        
        if (m_iHealth <= 0)
        {
            requestDeletion();
            
            ClientProxy* clientProxy = NG_SERVER->getClientProxy(getPlayerId());
            
            Server::staticHandleNewClient(clientProxy);
        }
        
        // tell the world our health dropped
        NG_SERVER->setStateDirty(getID(), ROBT_Health);
    }
}

void Robot::awardKill()
{
    if (m_isServer)
    {
        m_iNumKills++;
        
        // tell the world how bad ass we are
        NG_SERVER->setStateDirty(getID(), ROBT_NumKills);
    }
}

void Robot::setAddressHash(uint64_t addressHash)
{
    m_iAddressHash = addressHash;
}

uint64_t Robot::getAddressHash() const
{
    return m_iAddressHash;
}

void Robot::setPlayerId(uint32_t inPlayerId)
{
    m_iPlayerId = inPlayerId;
}

uint32_t Robot::getPlayerId() const
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
    uint8_t oldNumJumps = m_iNumJumps;
    bool wasSprinting = m_isSprinting;
    
    IInputState* currentState = inMove.getInputState();
    
    float deltaTime = inMove.getDeltaTime();
    
    processInput(deltaTime, currentState);
    
    updateInternal(deltaTime);
    
    if (m_iNumJumps && !oldNumJumps)
    {
        playSound(SOUND_ID_ROBOT_JUMP);
    }
    
    if (m_isSprinting && !wasSprinting)
    {
        playSound(SOUND_ID_ACTIVATE_SPRINT);
    }
}

void Robot::processInput(float inDeltaTime, IInputState* inInputState)
{
    InputState* inputState = static_cast<InputState*>(inInputState);
    
    m_velocity.setX(inputState->getDesiredHorizontalDelta() * m_fSpeed);
    
    m_isFacingLeft = m_velocity.getX() < 0 ? true : m_velocity.getX() > 0 ? false : m_isFacingLeft;
    
    m_isSprinting = inputState->isSprinting();
    
    if (m_isSprinting && m_isGrounded)
    {
        m_velocity.add(inputState->getDesiredHorizontalDelta() * m_fSpeed / 2, 0);
    }
    
    if (inputState->isJumping())
    {
        if (m_isGrounded)
        {
            m_fStateTime = 0;
            m_isGrounded = false;
            m_isFalling = false;
            m_isFirstJumpCompleted = false;
            m_iNumJumps = 1;
            
            m_acceleration.setY(-9.8f);
            m_velocity.setY(m_fJumpSpeed);
        }
        else if (m_iNumJumps == 1 && m_isFirstJumpCompleted)
        {
            m_fStateTime = 0;
            m_iNumJumps = 2;
            
            m_acceleration.setY(-9.8f);
            m_velocity.setY(m_fJumpSpeed - 3);
        }
    }
    else
    {
        if (m_iNumJumps == 1)
        {
            m_isFirstJumpCompleted = true;
            
            if (m_velocity.getY() > (m_fJumpSpeed / 2.0f))
            {
                m_velocity.setY(m_fJumpSpeed / 2.0f);
            }
        }
        
        if (m_iNumJumps == 2 && m_velocity.getY() > ((m_fJumpSpeed - 2) / 2.0f))
        {
            m_velocity.setY((m_fJumpSpeed - 2) / 2.0f);
        }
    }
    
    m_isShooting = inputState->isShooting();
}

void Robot::updateInternal(float inDeltaTime)
{
    Entity::update(inDeltaTime);
    
    processCollisions();
    
    if (m_velocity.getY() < 0
        && !m_isFalling)
    {
        m_isFalling = true;
        m_fStateTime = 0;
    }
}

void Robot::processCollisions()
{
    //right now just bounce off the sides..
    processCollisionsWithScreenWalls();
    
    Vector2 sourcePosition = getPosition();
    
    World* world = m_isServer ? InstanceManager::getServerWorld() : InstanceManager::getClientWorld();
    
    std::vector<Entity*> entities = world->getEntities();
    
    Robot* robot = nullptr;
    
    for (Entity* target : entities)
    {
        robot = nullptr;
        if (target != this
            && !target->isRequestingDeletion()
            && target->getRTTI().derivesFrom(Robot::rtti)
            && (robot = static_cast<Robot*>(target)))
        {
            //simple collision test for spheres- are the radii summed less than the distance?
            Vector2 targetPosition = robot->getPosition();
            float targetRadius = robot->getWidth() / 2;
            
            Vector2 delta = targetPosition - sourcePosition;
            float distSq = delta.lenSquared();
            float collisionDist = (m_fWidth / 2 + targetRadius);
            if (distSq < (collisionDist * collisionDist))
            {
                //okay, you hit something!
                //so, project your position far enough that you're not colliding
                Vector2 dirToTarget = delta;
                dirToTarget.nor();
                Vector2 acceptableDeltaFromSourceToTarget = dirToTarget * collisionDist;
                //important note- we only move this robot. the other robot can take care of moving itself
                setPosition(targetPosition - acceptableDeltaFromSourceToTarget);
                
                Vector2 relVel = m_velocity;
                
                //if other object is a robot, it might have velocity, so there might be relative velocity...
                if (robot)
                {
                    relVel -= robot->m_velocity;
                }
                
                //got vel with dir between objects to figure out if they're moving towards each other
                //and if so, the magnitude of the impulse (since they're both just balls)
                float relVelDotDir = relVel.dot(dirToTarget);
                
                if (relVelDotDir > 0.f)
                {
                    Vector2 impulse = relVelDotDir * dirToTarget;
                    
                    m_velocity -= impulse;
                    m_velocity *= m_fRobotRestitution;
                }
            }
        }
    }
}

void Robot::processCollisionsWithScreenWalls()
{
    Vector2 position = getPosition();
    float x = position.getX();
    
    float boundsY = getMainBounds().getBottom();
    
    float vx = m_velocity.getX();
    float vy = m_velocity.getY();
    
    float radius = m_fWidth / 2;
    
    if (boundsY <= GROUND_TOP && vy < 0)
    {
        m_velocity.setY(0);
        m_acceleration.setY(0);
        position.setY(GROUND_TOP + getMainBounds().getHeight() / 2);
        setPosition(position);
        
        m_iNumJumps = 0;
        m_isGrounded = true;
        m_isFalling = false;
    }
    
    if ((x + radius) >= GAME_WIDTH && vx > 0)
    {
        position.setX(GAME_WIDTH - radius);
        setPosition(position);
    }
    else if (x <= 0 && vx < 0)
    {
        position.setX(0);
        setPosition(position);
    }
}

void Robot::handleShooting()
{
    float time = Timing::getInstance()->getFrameStartTime();
    
    if (m_isShooting && time > m_fTimeOfNextShot)
    {
        //not exact, but okay
        m_fTimeOfNextShot = time + 0.25f;
        
        //fire!
        Projectile* projectile = static_cast<Projectile*>(FWInstanceManager::getServerEntityRegistry()->createEntity(NETWORK_TYPE_Projectile));
        projectile->initFromShooter(this);
    }
}

void Robot::doClientSidePredictionForLocalRobot(uint32_t inReadState)
{
    if ((inReadState & ROBT_Pose) != 0)
    {
        //simulate pose only if we received new pose- might have just gotten thrustDir
        //in which case we don't need to replay moves because we haven't warped backwards
        
        //all processed moves have been removed, so all that are left are unprocessed moves
        //so we must apply them...
        MoveList& moveList = InputManager::getInstance()->getMoveList();
        
        for (const Move& move : moveList)
        {
            float deltaTime = move.getDeltaTime();
            processInput(deltaTime, move.getInputState());
            
            updateInternal(deltaTime);
        }
    }
}

//so what do we want to do here? need to do some kind of interpolation...

void Robot::doClientSidePredictionForRemoteRobot(uint32_t inReadState)
{
    if ((inReadState & ROBT_Pose) != 0)
    {
        // simulate movement for an additional RTT
        float rtt = NG_CLIENT->getRoundTripTime();
        
        // let's break into framerate sized chunks so we don't run through walls and do crazy things...
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

void Robot::interpolateClientSidePrediction(Vector2& inOldAcceleration, Vector2& inOldVelocity, Vector2& inOldPos)
{
    interpolateVectorsIfNecessary(inOldAcceleration, getAcceleration(), m_fTimeAccelerationBecameOutOfSync, "acceleration");
    interpolateVectorsIfNecessary(inOldVelocity, getVelocity(), m_fTimeVelocityBecameOutOfSync, "velocity");
    interpolateVectorsIfNecessary(inOldPos, getPosition(), m_fTimePositionBecameOutOfSync, "position");
}

void Robot::interpolateVectorsIfNecessary(Vector2& inA, Vector2& inB, float& syncTracker, const char* vectorType)
{
    float roundTripTime = NG_CLIENT->getRoundTripTime();
    
    if (!inA.isEqualTo(inB))
    {
        LOG("WARN: Robot move replay ended with incorrect %s! Old %3.8f, %3.8f - New %3.8f, %3.8f", vectorType, inA.getX(), inA.getY(), inB.getX(), inB.getY());
        
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
            inB.set(lerp(inA, inB, roundTripTime / 2));
        }
    }
    else
    {
        syncTracker = 0.0f;
    }
}

void Robot::playSound(int soundId)
{
    Util::playSound(soundId, getPlayerId(), getPosition(), m_isServer);
}

Robot::Robot(bool isServer) : Entity(0, 0, 1.565217391304348f, 2.0f),
m_isServer(isServer),
m_fJumpSpeed(10.0f),
m_fSpeed(7.5f),
m_fTimeOfNextShot(0.0f),
m_fRobotRestitution(0.1f),
m_fTimeAccelerationBecameOutOfSync(0.0f),
m_fTimeVelocityBecameOutOfSync(0.0f),
m_fTimePositionBecameOutOfSync(0.0f),
m_iNumKills(0),
m_iHealth(255),
m_iNumJumps(0),
m_isFacingLeft(false),
m_isGrounded(false),
m_isFalling(false),
m_isShooting(false),
m_isSprinting(false),
m_isFirstJumpCompleted(false),
m_iAddressHash(0),
m_iPlayerId(0)
{
    m_acceleration.setY(-9.8f);
    
    if (m_isServer)
    {
        NG_SERVER->registerEntity(this);
    }
}

RTTI_IMPL(Robot, Entity);

NETWORK_TYPE_IMPL(Robot);
