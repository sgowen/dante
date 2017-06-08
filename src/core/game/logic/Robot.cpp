//
//  Robot.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Robot.h"

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
#include "Projectile.h"
#include "EntityRegistry.h"

#ifdef NG_SERVER
#include "NetworkManagerServer.h"
#include "ClientProxy.h"
#include "Server.h"
#elif NG_CLIENT
#include "NetworkManagerClient.h"
#include "InputManager.h"
#include "NGAudioEngine.h"
#endif

#include <math.h>

Entity* Robot::create()
{
    Robot* ret = new Robot();
    
#ifdef NG_SERVER
    NetworkManagerServer::getInstance()->registerEntity(ret);
#endif
    
    return ret;
}

void Robot::onDeletion()
{
#ifdef NG_SERVER
    NetworkManagerServer::getInstance()->unregisterEntity(this);
#elif NG_CLIENT
    if (getPlayerId() == NetworkManagerClient::getInstance()->getPlayerId())
    {
        // This robot is the current local player, so let's display something like "Respawning in 5, 4, 3..."
        playSound(SOUND_ID_DEATH);
    }
#endif
}

void Robot::update()
{
#ifdef NG_SERVER
    Vector2 oldAcceleration = getAcceleration();
    Vector2 oldVelocity = getVelocity();
    Vector2 oldPosition = getPosition();
    bool old_isFacingLeft = m_isFacingLeft;
    bool old_isGrounded = m_isGrounded;
    bool old_isFalling = m_isFalling;
    bool old_isShooting = m_isShooting;
    bool old_isJumping = m_isJumping;
    bool old_isSprinting = m_isSprinting;
    
    // is there a move we haven't processed yet?
    ClientProxy* client = NetworkManagerServer::getInstance()->getClientProxy(getPlayerId());
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
        || old_isFacingLeft != m_isFacingLeft
        || old_isGrounded != m_isGrounded
        || old_isFalling != m_isFalling
        || old_isShooting != m_isShooting
        || old_isJumping != m_isJumping
        || old_isSprinting != m_isSprinting)
    {
        NetworkManagerServer::getInstance()->setStateDirty(getID(), ROBT_Pose);
    }
#elif NG_CLIENT
    // TODO, allow for multiple client inputs
    if (getPlayerId() == NetworkManagerClient::getInstance()->getPlayerId())
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
#endif
}

uint32_t Robot::getAllStateMask() const
{
    return ROBT_AllState;
}

void Robot::read(InputMemoryBitStream& inInputStream)
{
#ifdef NG_CLIENT
    float oldStateTime = m_fStateTime;
    Vector2 oldAcceleration = m_acceleration;
    Vector2 oldVelocity = m_velocity;
    Vector2 oldPosition = m_position;
    bool wasJumping = m_isJumping;
    bool wasSprinting = m_isSprinting;
#endif
    
    bool stateBit;
    
    uint32_t readState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_iPlayerId);
        readState |= ROBT_PlayerId;
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
        
        inInputStream.read(m_isFacingLeft);
        inInputStream.read(m_isGrounded);
        inInputStream.read(m_isFalling);
        inInputStream.read(m_isShooting);
        inInputStream.read(m_isJumping);
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
        inInputStream.read(m_iHealth, 4); // Support up to 15 health points, for now...
        readState |= ROBT_Health;
    }
    
#ifdef NG_CLIENT
    if (getPlayerId() == NetworkManagerClient::getInstance()->getPlayerId())
    {
        doClientSidePredictionAfterReplicationForLocalRobot(readState);
        
        // if this is a create packet, don't interpolate
        if ((readState & ROBT_PlayerId) == 0)
        {
            interpolateClientSidePrediction(oldStateTime, oldAcceleration, oldVelocity, oldPosition);
        }
    }
    else
    {
        doClientSidePredictionAfterReplicationForRemoteRobot(readState);
        
        if ((readState & ROBT_PlayerId) == 0)
        {
            interpolateClientSidePrediction(oldStateTime, oldPosition);
        }
        
        if (m_isJumping && !wasJumping)
        {
            playSound(SOUND_ID_ROBOT_JUMP);
        }
        
        if (m_isSprinting && !wasSprinting)
        {
            playSound(SOUND_ID_ACTIVATE_SPRINT);
        }
    }
#endif
}

uint32_t Robot::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & ROBT_PlayerId)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(m_iPlayerId);
        
        writtenState |= ROBT_PlayerId;
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
        
        inOutputStream.write((bool)m_isFacingLeft);
        inOutputStream.write((bool)m_isGrounded);
        inOutputStream.write((bool)m_isFalling);
        inOutputStream.write((bool)m_isShooting);
        inOutputStream.write((bool)m_isJumping);
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
        inOutputStream.write(m_iHealth, 4); // Support up to 15 health points, for now...
        
        writtenState |= ROBT_Health;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    return writtenState;
}

void Robot::takeDamage()
{
#ifdef NG_SERVER
    m_iHealth--;
    
    if (m_iHealth <= 0)
    {
        requestDeletion();
        
        ClientProxy* clientProxy = NetworkManagerServer::getInstance()->getClientProxy(getPlayerId());
        
        Server::staticHandleNewClient(clientProxy);
    }
    
    // tell the world our health dropped
    NetworkManagerServer::getInstance()->setStateDirty(getID(), ROBT_Health);
#endif
}

void Robot::setPlayerId(uint32_t inPlayerId)
{
    m_iPlayerId = inPlayerId;
}

uint32_t Robot::getPlayerId() const
{
    return m_iPlayerId;
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
    bool wasJumping = m_isJumping;
    bool wasSprinting = m_isSprinting;
    
    IInputState* currentState = inMove.getInputState();
    
    float deltaTime = inMove.getDeltaTime();
    
    processInput(deltaTime, currentState);
    
    updateInternal(deltaTime);
    
    if (m_isJumping && !wasJumping)
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
    
    if (inputState->getDesiredJumpIntensity() > 0
        && m_isGrounded)
    {
        m_velocity.setY(inputState->getDesiredJumpIntensity() * m_fJumpSpeed);
        m_acceleration.setY(-9.8f);
        m_fStateTime = 0;
        m_isGrounded = false;
        m_isFalling = false;
        m_isJumping = true;
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
    
    std::vector<Entity*> entities = World::getInstance()->getEntities();
    for (Entity* target : entities)
    {
        if (target != this && !target->isRequestingDeletion() && target->getRTTI().derivesFrom(Robot::rtti))
        {
            //simple collision test for spheres- are the radii summed less than the distance?
            Vector2 targetPosition = target->getPosition();
            float targetRadius = target->getWidth() / 2;
            
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
                Robot* targetRobot = target->getRTTI().derivesFrom(Robot::rtti) ? static_cast<Robot*>(target) : nullptr;
                if (targetRobot)
                {
                    relVel -= targetRobot->m_velocity;
                }
                
                //got vel with dir between objects to figure out if they're moving towards each other
                //and if so, the magnitude of the impulse (since they're both just balls)
                float relVelDotDir = relVel.dot(dirToTarget);
                
                if (relVelDotDir > 0.f)
                {
                    Vector2 impulse = relVelDotDir * dirToTarget;
                    
                    if (targetRobot)
                    {
                        m_velocity -= impulse;
                        m_velocity *= m_fRobotRestitution;
                    }
                    else
                    {
                        m_velocity -= impulse * 2.f;
                        m_velocity *= m_fWallRestitution;
                    }
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
    
    if (boundsY <= 0.875f && vy < 0)
    {
        m_velocity.setY(0);
        m_acceleration.setY(0);
        position.setY(0.875f + getMainBounds().getHeight() / 2);
        setPosition(position);
        m_isGrounded = true;
        m_isFalling = false;
        m_isJumping = false;
    }
    
    if ((x + radius) >= CAM_WIDTH && vx > 0)
    {
        position.setX(CAM_WIDTH - radius);
        setPosition(position);
    }
    else if (x <= 0 && vx < 0)
    {
        position.setX(0);
        setPosition(position);
    }
}

#ifdef NG_SERVER
void Robot::handleShooting()
{
    float time = Timing::getInstance()->getFrameStartTime();
    
    if (m_isShooting && Timing::getInstance()->getFrameStartTime() > m_fTimeOfNextShot)
    {
        //not exact, but okay
        m_fTimeOfNextShot = time + 0.25f;
        
        //fire!
        Projectile* projectile = static_cast<Projectile*>(EntityRegistry::getInstance()->createEntity(NETWORK_TYPE_Projectile));
        projectile->initFromShooter(this);
    }
}
#elif NG_CLIENT
void Robot::doClientSidePredictionAfterReplicationForLocalRobot(uint32_t inReadState)
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

void Robot::doClientSidePredictionAfterReplicationForRemoteRobot(uint32_t inReadState)
{
    if ((inReadState & ROBT_Pose) != 0)
    {
        // simulate movement for an additional RTT
        float rtt = NetworkManagerClient::getInstance()->getRoundTripTime();
        
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

void Robot::interpolateClientSidePrediction(float& inOldStateTime, Vector2& inOldAcceleration, Vector2& inOldVelocity, Vector2& inOldPos)
{
    if (interpolateVectorsIfNecessary(inOldAcceleration, getAcceleration(), m_fTimeAccelerationBecameOutOfSync))
    {
        LOG("Robot ERROR! Move Replay Acceleration");
    }
    
    if (interpolateVectorsIfNecessary(inOldVelocity, getVelocity(), m_fTimeVelocityBecameOutOfSync))
    {
        LOG("Robot ERROR! Move Replay Velocity");
    }
    
    interpolateClientSidePrediction(inOldStateTime, inOldPos);
}

void Robot::interpolateClientSidePrediction(float& inOldStateTime, Vector2& inOldPos)
{
    if (!areFloatsPracticallyEqual(inOldStateTime, m_fStateTime))
    {
        m_fStateTime = inOldStateTime + 0.1f * (m_fStateTime - inOldStateTime);
    }
    
    if (interpolateVectorsIfNecessary(inOldPos, getPosition(), m_fTimePositionBecameOutOfSync))
    {
        LOG("Robot ERROR! Move Replay Position");
    }
}

bool Robot::interpolateVectorsIfNecessary(Vector2& inA, Vector2& inB, float& syncTracker)
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

void Robot::playSound(int soundId)
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

Robot::Robot() : Entity(0, 0, 1.565217391304348f, 2.0f),
m_fJumpSpeed(10.0f),
m_fSpeed(7.5f),
m_fTimeOfNextShot(0.0f),
m_fWallRestitution(0.1f),
m_fRobotRestitution(0.1f),
m_fTimeAccelerationBecameOutOfSync(0.0f),
m_fTimeVelocityBecameOutOfSync(0.0f),
m_fTimePositionBecameOutOfSync(0.0f),
m_iHealth(1),
m_isFacingLeft(false),
m_isGrounded(false),
m_isFalling(false),
m_isShooting(false),
m_isJumping(false),
m_isSprinting(false),
m_iPlayerId(0)
{
    m_acceleration.setY(-9.8f);
}

RTTI_IMPL(Robot, Entity);

NETWORK_TYPE_IMPL(Robot);
