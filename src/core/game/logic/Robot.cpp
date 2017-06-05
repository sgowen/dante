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
        || old_isJumping != m_isJumping)
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
    Vector2 oldAcceleration = getAcceleration();
    Vector2 oldVelocity = getVelocity();
    Vector2 oldPosition = getPosition();
    bool wasJumping = m_isJumping;
#endif
    
    bool stateBit;
    
    uint32_t readState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        uint32_t playerId;
        inInputStream.read(playerId);
        setPlayerId(playerId);
        readState |= ROBT_PlayerId;
    }
    
    Vector2 replicatedAcceleration;
    Vector2 replicatedVelocity;
    Vector2 replicatedPosition;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_fStateTime);
        
        inInputStream.read(replicatedAcceleration.getXRef());
        inInputStream.read(replicatedAcceleration.getYRef());
        
        m_acceleration.set(replicatedAcceleration);
        
        inInputStream.read(replicatedVelocity.getXRef());
        inInputStream.read(replicatedVelocity.getYRef());
        
        m_velocity.set(replicatedVelocity);
        
        inInputStream.read(replicatedPosition.getXRef());
        inInputStream.read(replicatedPosition.getYRef());
        
        setPosition(replicatedPosition);
        
        inInputStream.read(m_isFacingLeft);
        inInputStream.read(m_isGrounded);
        inInputStream.read(m_isFalling);
        inInputStream.read(m_isShooting);
        inInputStream.read(m_isJumping);
        
        readState |= ROBT_Pose;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        Color color;
        inInputStream.read(color);
        setColor(color);
        readState |= ROBT_Color;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        m_iHealth = 0;
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
        
        if (m_isJumping && !wasJumping)
        {
            //playSoundForRemotePlayer(SOUND_ID_ROBOT_JUMP);
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
        inOutputStream.write(getPlayerId());
        
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
        
        Vector2 acceleration = m_acceleration;
        inOutputStream.write(acceleration.getX());
        inOutputStream.write(acceleration.getY());
        
        Vector2 velocity = m_velocity;
        inOutputStream.write(velocity.getX());
        inOutputStream.write(velocity.getY());
        
        Vector2 position = getPosition();
        inOutputStream.write(position.getX());
        inOutputStream.write(position.getY());
        
        inOutputStream.write((bool)m_isFacingLeft);
        inOutputStream.write((bool)m_isGrounded);
        inOutputStream.write((bool)m_isFalling);
        inOutputStream.write((bool)m_isShooting);
        inOutputStream.write((bool)m_isJumping);
        
        writtenState |= ROBT_Pose;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & ROBT_Color)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(getColor());
        
        writtenState |= ROBT_Color;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & ROBT_Health)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(m_iHealth, 4);
        
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
    m_iHealth--;
    
    if (m_iHealth <= 0)
    {
        requestDeletion();
        
//        ClientProxy* clientProxy = NetworkManagerServer::getInstance()->getClientProxy(getPlayerId());
//        
//        Server::staticHandleNewClient(clientProxy);
    }
    
    // tell the world our health dropped
//    NetworkManagerServer::getInstance()->setStateDirty(getID(), ROBT_Health);
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

void Robot::processMove(const Move& inMove)
{
#ifdef NG_CLIENT
    bool wasJumping = m_isJumping;
#endif
    
    IInputState* currentState = inMove.getInputState();
    
    float deltaTime = inMove.getDeltaTime();
    
    processInput(deltaTime, currentState);
    
    updateInternal(deltaTime);
    
#ifdef NG_CLIENT
    if (m_isJumping && !wasJumping)
    {
        //NG_AUDIO_ENGINE->playSound(SOUND_ID_ROBOT_JUMP);
    }
#endif
}

void Robot::processInput(float inDeltaTime, IInputState* inInputState)
{
    InputState* inputState = static_cast<InputState*>(inInputState);
    
    m_velocity.setX(inputState->getDesiredHorizontalDelta() * m_fSpeed);
    
    m_isFacingLeft = m_velocity.getX() < 0 ? true : m_velocity.getX() > 0 ? false : m_isFacingLeft;
    
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
    
    //now let's iterate through the world and see what we hit...
    //note: since there's a small number of objects in our game, this is fine.
    //but in a real game, brute-force checking collisions against every other object is not efficient.
    //it would be preferable to use a quad tree or some other structure to minimize the
    //number of collisions that need to be tested.
    for (auto goIt = World::getInstance()->getEntities().begin(), end = World::getInstance()->getEntities().end(); goIt != end; ++goIt)
    {
        Entity* target = *goIt;
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
                //important note- we only move this cat. the other cat can take care of moving itself
                setPosition(targetPosition - acceptableDeltaFromSourceToTarget);
                
                Vector2 relVel = m_velocity;
                
                //if other object is a cat, it might have velocity, so there might be relative velocity...
                Robot* targetCat = target->getRTTI().derivesFrom(Robot::rtti) ? static_cast<Robot*>(target) : nullptr;
                if (targetCat)
                {
                    relVel -= targetCat->m_velocity;
                }
                
                //got vel with dir between objects to figure out if they're moving towards each other
                //and if so, the magnitude of the impulse (since they're both just balls)
                float relVelDotDir = relVel.dot(dirToTarget);
                
                if (relVelDotDir > 0.f)
                {
                    Vector2 impulse = relVelDotDir * dirToTarget;
                    
                    if (targetCat)
                    {
                        m_velocity -= impulse;
                        m_velocity *= m_fCatRestitution;
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
    float y = position.getY();
    
    float boundsY = getMainBounds().getBottom();
    
    float vx = m_velocity.getX();
    float vy = m_velocity.getY();
    
    float radius = m_fWidth / 2;
    
    //if the robot collides against a wall, the quick solution is to push it off
    if ((y + radius) >= CAM_HEIGHT && vy > 0)
    {
        m_velocity.setY(-vy * m_fWallRestitution);
        position.setY(CAM_HEIGHT - radius);
        setPosition(position);
        m_isGrounded = false;
        m_isFalling = true;
    }
    else if (boundsY <= 1.3f && vy < 0)
    {
        m_velocity.setY(0);
        m_acceleration.setY(0);
        position.setY(1.3f + getMainBounds().getHeight() / 2);
        setPosition(position);
        m_isGrounded = true;
        m_isFalling = false;
        m_isJumping = false;
    }
    
    if ((x + radius) >= CAM_WIDTH && vx > 0)
    {
        m_velocity.setX(-vx * m_fWallRestitution);
        position.setX(CAM_WIDTH - radius);
        setPosition(position);
    }
    else if (x <= (0) && vx < 0)
    {
        m_velocity.setX(-vx * m_fWallRestitution);
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

void Robot::interpolateClientSidePrediction(float& inOldStateTime, Vector2& inOldAcceleration, Vector2& inOldVelocity, Vector2& inOldPosition)
{
    float roundTripTime = NetworkManagerClient::getInstance()->getRoundTripTime();
    
    if (!areFloatsPracticallyEqual(inOldStateTime, m_fStateTime))
    {
        m_fStateTime = inOldStateTime + 0.1f * (m_fStateTime - inOldStateTime);
    }
    
    if (!inOldAcceleration.isEqualTo(getAcceleration()))
    {
        LOG("ERROR! Move replay ended with incorrect acceleration! Old %3.8f, %3.8f - New %3.8f, %3.8f", inOldAcceleration.getX(), inOldAcceleration.getY(), getAcceleration().getX(), getAcceleration().getY());
        
        //have we been out of sync, or did we just become out of sync?
        float time = Timing::getInstance()->getFrameStartTime();
        if (m_fTimeAccelerationBecameOutOfSync == 0.0f)
        {
            m_fTimeAccelerationBecameOutOfSync = time;
        }
        
        //now interpolate to the correct value...
        float durationOutOfSync = time - m_fTimeAccelerationBecameOutOfSync;
        if (durationOutOfSync < roundTripTime)
        {
            m_acceleration.set(lerp(inOldAcceleration, getAcceleration(), 0.1f));
        }
        //otherwise, fine...
    }
    else
    {
        //we're in sync
        m_fTimeAccelerationBecameOutOfSync = 0.0f;
    }
    
    if (!inOldVelocity.isEqualTo(getVelocity()))
    {
        LOG("ERROR! Move replay ended with incorrect velocity! Old %3.8f, %3.8f - New %3.8f, %3.8f", inOldVelocity.getX(), inOldVelocity.getY(), getVelocity().getX(), getVelocity().getY());
        
        //have we been out of sync, or did we just become out of sync?
        float time = Timing::getInstance()->getFrameStartTime();
        if (m_fTimeVelocityBecameOutOfSync == 0.0f)
        {
            m_fTimeVelocityBecameOutOfSync = time;
        }
        
        //now interpolate to the correct value...
        float durationOutOfSync = time - m_fTimeVelocityBecameOutOfSync;
        if (durationOutOfSync < roundTripTime)
        {
            m_velocity.set(lerp(inOldVelocity, getVelocity(), 0.1f));
        }
        //otherwise, fine...
    }
    else
    {
        //we're in sync
        m_fTimeVelocityBecameOutOfSync = 0.0f;
    }
    
    if (!inOldPosition.isEqualTo(getPosition()))
    {
        LOG("ERROR! Move replay ended with incorrect position! Old %3.8f, %3.8f - New %3.8f, %3.8f", inOldPosition.getX(), inOldPosition.getY(), getPosition().getX(), getPosition().getY());
        
        //have we been out of sync, or did we just become out of sync?
        float time = Timing::getInstance()->getFrameStartTime();
        if (m_fTimePositionBecameOutOfSync == 0.0f)
        {
            m_fTimePositionBecameOutOfSync = time;
        }
        
        float durationOutOfSync = time - m_fTimePositionBecameOutOfSync;
        if (durationOutOfSync < roundTripTime)
        {
            setPosition(lerp(inOldPosition, getPosition(), 0.1f));
        }
    }
    else
    {
        //we're in sync
        m_fTimePositionBecameOutOfSync = 0.0f;
    }
}

void Robot::playSoundForRemotePlayer(int soundId)
{
    Robot* playerRobot = World::staticGetRobotWithPlayerId(NetworkManagerClient::getInstance()->getPlayerId());
    float volume = 1;
    if (playerRobot)
    {
        float distance = playerRobot->getPosition().dist(getPosition());
        volume = 1.0f / (distance / 4.0f);
    }
    
    NG_AUDIO_ENGINE->playSound(soundId, volume);
}
#endif

Robot::Robot() : Entity(0, 0, 1.565217391304348f, 2.0f),
m_fJumpSpeed(8.0f),
m_fSpeed(7.5f),
m_fTimeOfNextShot(0.0f),
m_fWallRestitution(0.1f),
m_fCatRestitution(0.1f),
m_fTimeAccelerationBecameOutOfSync(0.0f),
m_fTimeVelocityBecameOutOfSync(0.0f),
m_fTimePositionBecameOutOfSync(0.0f),
m_iHealth(5),
m_isFacingLeft(false),
m_isGrounded(false),
m_isFalling(false),
m_isShooting(false),
m_isJumping(false),
m_iPlayerId(0)
{
    m_acceleration.setY(-9.8f);
}

RTTI_IMPL(Robot, Entity);

NETWORK_TYPE_IMPL(Robot);
