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

#ifdef NG_SERVER
#include "NetworkManagerServer.h"
#include "ClientProxy.h"
#elif NG_CLIENT
#include "NetworkManagerClient.h"
#include "InputManager.h"
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
    Vector2 oldLocation = getPosition();
    Vector2 oldVelocity = getVelocity();
    
    // is there a move we haven't processed yet?
    ClientProxy* client = NetworkManagerServer::getInstance()->getClientProxy(getPlayerId());
    if (client)
    {
        MoveList& moveList = client->getUnprocessedMoveList();
        for (Move& unprocessedMove : moveList)
        {
            processMove(unprocessedMove);
        }
        
        moveList.clear();
    }
    
    handleShooting();
    
    if (!oldLocation.isEqualTo(getPosition()) ||
        !oldVelocity.isEqualTo(getVelocity()))
    {
        NetworkManagerServer::getInstance()->setStateDirty(getID(), RBRS_Pose);
    }
#elif NG_CLIENT
    // TODO, allow for multiple client inputs
    if (getPlayerId() == NetworkManagerClient::getInstance()->getPlayerId())
    {
        Move* pendingMove = InputManager::getInstance()->getAndClearPendingMove();
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
    return RBRS_AllState;
}

void Robot::read(InputMemoryBitStream& inInputStream)
{
    Vector2 oldLocation = getPosition();
    Vector2 oldVelocity = getVelocity();
    
    bool stateBit;
    
    m_iReadState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        uint32_t playerId;
        inInputStream.read(playerId);
        setPlayerId(playerId);
        m_iReadState |= RBRS_PlayerId;
    }
    
    Vector2 replicatedLocation;
    Vector2 replicatedVelocity;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(m_fStateTime);
        
        inInputStream.read(replicatedVelocity.getXRef());
        inInputStream.read(replicatedVelocity.getYRef());
        
        m_velocity.set(replicatedVelocity);
        
        inInputStream.read(replicatedLocation.getXRef());
        inInputStream.read(replicatedLocation.getYRef());
        
        setPosition(replicatedLocation);
        
        inInputStream.read(m_isFacingLeft);
        inInputStream.read(m_isGrounded);
        inInputStream.read(m_isFalling);
        inInputStream.read(m_isShooting);
        
        m_iReadState |= RBRS_Pose;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        Color color;
        inInputStream.read(color);
        setColor(color);
        m_iReadState |= RBRS_Color;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        m_iHealth = 0;
        inInputStream.read(m_iHealth, 4); // Support up to 15 health points, for now...
        m_iReadState |= RBRS_Health;
    }
    
#ifdef NG_CLIENT
    if (getPlayerId() == NetworkManagerClient::getInstance()->getPlayerId())
    {
        doClientSidePredictionAfterReplicationForLocalCat(m_iReadState);
        
        //if this is a create packet, don't interpolate
        if ((m_iReadState & RBRS_PlayerId) == 0)
        {
            interpolateClientSidePrediction(oldLocation, oldVelocity, false);
        }
    }
    else
    {
        doClientSidePredictionAfterReplicationForRemoteCat(m_iReadState);
        
        //will this smooth us out too? it'll interpolate us just 10% of the way there...
        if ((m_iReadState & RBRS_PlayerId) == 0)
        {
            interpolateClientSidePrediction(oldLocation, oldVelocity, true);
        }
    }
#endif
}

uint32_t Robot::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & RBRS_PlayerId)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(getPlayerId());
        
        writtenState |= RBRS_PlayerId;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & RBRS_Pose)
    {
        inOutputStream.write((bool)true);
        
        inOutputStream.write(m_fStateTime);
        
        Vector2 velocity = m_velocity;
        inOutputStream.write(velocity.getX());
        inOutputStream.write(velocity.getY());
        
        Vector2 location = getPosition();
        inOutputStream.write(location.getX());
        inOutputStream.write(location.getY());
        
        inOutputStream.write((bool)m_isFacingLeft);
        inOutputStream.write((bool)m_isGrounded);
        inOutputStream.write((bool)m_isFalling);
        inOutputStream.write((bool)m_isShooting);
        
        writtenState |= RBRS_Pose;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & RBRS_Color)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(getColor());
        
        writtenState |= RBRS_Color;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & RBRS_Health)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(m_iHealth, 4);
        
        writtenState |= RBRS_Health;
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
//    NetworkManagerServer::getInstance()->setStateDirty(getID(), RBRS_Health);
}

void Robot::setPlayerId(uint32_t inPlayerId)
{
    m_iPlayerId = inPlayerId;
}

uint32_t Robot::getPlayerId() const
{
    return m_iPlayerId;
}

void Robot::setIndexInWorld(int inIndex)
{
    m_iIndexInWorld = inIndex;
}

int Robot::getIndexInWorld() const
{
    return m_iIndexInWorld;
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

void Robot::processMove(Move& inMove)
{
    IInputState* currentState = inMove.getInputState();
    
    float deltaTime = inMove.getDeltaTime();
    
    processInput(deltaTime, currentState);
    
    updateInternal(deltaTime);
    
    LOG("Move Time: %3.4f deltaTime: %3.4f", inMove.getTimestamp(), deltaTime);
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

void Robot::processInput(float inDeltaTime, IInputState* inInputState)
{
    InputState* inputState = static_cast<InputState*>(inInputState);
    
    // moving
    m_velocity.setX(inputState->getDesiredHorizontalDelta() * m_fSpeed);
    
    m_isFacingLeft = m_velocity.getX() < 0 ? true : m_velocity.getX() > 0 ? false : m_isFacingLeft;
    
    // jumping
    if (inputState->getDesiredJumpIntensity() > 0
        && m_isGrounded)
    {
        m_velocity.setY(inputState->getDesiredJumpIntensity() * m_fJumpSpeed);
        m_fStateTime = 0;
        m_isGrounded = false;
        m_isFalling = false;
    }
    
    m_isShooting = inputState->isShooting();
}

void Robot::processCollisions()
{
    //right now just bounce off the sides..
    processCollisionsWithScreenWalls();
    
    float sourceRadius = 0.5f;
    Vector2 sourceLocation = getPosition();
    
    //now let's iterate through the world and see what we hit...
    //note: since there's a small number of objects in our game, this is fine.
    //but in a real game, brute-force checking collisions against every other object is not efficient.
    //it would be preferable to use a quad tree or some other structure to minimize the
    //number of collisions that need to be tested.
    for (auto goIt = World::sInstance->GetRobots().begin(), end = World::sInstance->GetRobots().end(); goIt != end; ++goIt)
    {
        Entity* target = *goIt;
        if (target != this && !target->isRequestingDeletion())
        {
            //simple collision test for spheres- are the radii summed less than the distance?
            Vector2 targetLocation = target->getPosition();
            float targetRadius = 0.5f;
            
            Vector2 delta = targetLocation - sourceLocation;
            float distSq = delta.lenSquared();
            float collisionDist = (sourceRadius + targetRadius);
            if (distSq < (collisionDist * collisionDist))
            {
                //okay, you hit something!
                //so, project your location far enough that you're not colliding
                Vector2 dirToTarget = delta;
                dirToTarget.nor();
                Vector2 acceptableDeltaFromSourceToTarget = dirToTarget * collisionDist;
                //important note- we only move this cat. the other cat can take care of moving itself
                setPosition(targetLocation - acceptableDeltaFromSourceToTarget);
                
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
    Vector2 location = getPosition();
    float x = location.getX();
    float y = location.getY();
    
    float vx = m_velocity.getX();
    float vy = m_velocity.getY();
    
    float radius = 0.5f;
    
    //if the cat collides against a wall, the quick solution is to push it off
    if ((y + radius) >= CAM_HEIGHT && vy > 0)
    {
        m_velocity.setY(-vy * m_fWallRestitution);
        location.setY(CAM_HEIGHT - radius);
        setPosition(location);
        m_isGrounded = false;
        m_isFalling = true;
    }
    else if (y <= 2.3f && vy < 0)
    {
        m_velocity.setY(0);
        location.setY(2.3f);
        setPosition(location);
        m_isGrounded = true;
        m_isFalling = false;
    }
    
    if ((x + radius) >= CAM_WIDTH && vx > 0)
    {
        m_velocity.setX(-vx * m_fWallRestitution);
        location.setX(CAM_WIDTH - radius);
        setPosition(location);
    }
    else if (x <= (0) && vx < 0)
    {
        m_velocity.setX(-vx * m_fWallRestitution);
        location.setX(0);
        setPosition(location);
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
        //        YarnPtr yarn = std::static_pointer_cast< Yarn >( GameObjectRegistry::sInstance->CreateGameObject( 'YARN' ) );
        //        yarn->InitFromShooter( this );
    }
}
#elif NG_CLIENT
void Robot::doClientSidePredictionAfterReplicationForLocalCat(uint32_t inReadState)
{
    if ((inReadState & RBRS_Pose) != 0)
    {
        //simulate pose only if we received new pose- might have just gotten thrustDir
        //in which case we don't need to replay moves because we haven't warped backwards
        
        //all processed moves have been removed, so all that are left are unprocessed moves
        //so we must apply them...
        MoveList& moveList = InputManager::getInstance()->getMoveList();
        
        for (Move& move : moveList)
        {
            float deltaTime = move.getDeltaTime();
            processInput(deltaTime, move.getInputState());
            
            updateInternal(deltaTime);
        }
    }
}

//so what do we want to do here? need to do some kind of interpolation...

void Robot::doClientSidePredictionAfterReplicationForRemoteCat(uint32_t inReadState)
{
    if ((inReadState & RBRS_Pose) != 0)
    {
        // simulate movement for an additional RTT
        float rtt = NetworkManagerClient::getInstance()->getRoundTripTime();
        LOG("Other cat came in, simulating for an extra %f", rtt);
        
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

void Robot::interpolateClientSidePrediction(Vector2& inOldLocation, Vector2& inOldVelocity, bool inIsForRemoteCat)
{
    float roundTripTime = NetworkManagerClient::getInstance()->getRoundTripTime();
    
    if (!inOldLocation.isEqualTo(getPosition()))
    {
        LOG("ERROR! Move replay ended with incorrect location! Old %3.8f, %3.8f - New %3.8f, %3.8f", inOldLocation.getX(), inOldLocation.getY(), getPosition().getX(), getPosition().getY());
        
        //have we been out of sync, or did we just become out of sync?
        float time = Timing::getInstance()->getFrameStartTime();
        if (m_fTimePositionBecameOutOfSync == 0.f)
        {
            m_fTimePositionBecameOutOfSync = time;
        }
        
        float durationOutOfSync = time - m_fTimePositionBecameOutOfSync;
        if (durationOutOfSync < roundTripTime)
        {
            setPosition(lerp(inOldLocation, getPosition(), inIsForRemoteCat ? (durationOutOfSync / roundTripTime) : 0.1f));
        }
    }
    else
    {
        //we're in sync
        m_fTimePositionBecameOutOfSync = 0.f;
    }
    
    if (!inOldVelocity.isEqualTo(getVelocity()))
    {
        LOG("ERROR! Move replay ended with incorrect velocity! Old %3.8f, %3.8f - New %3.8f, %3.8f", inOldVelocity.getX(), inOldVelocity.getY(), getVelocity().getX(), getVelocity().getY());
        
        //have we been out of sync, or did we just become out of sync?
        float time = Timing::getInstance()->getFrameStartTime();
        if (m_fTimeVelocityBecameOutOfSync == 0.f)
        {
            m_fTimeVelocityBecameOutOfSync = time;
        }
        
        //now interpolate to the correct value...
        float durationOutOfSync = time - m_fTimeVelocityBecameOutOfSync;
        if (durationOutOfSync < roundTripTime)
        {
            m_velocity.set(lerp(inOldVelocity, getVelocity(), inIsForRemoteCat ? (durationOutOfSync / roundTripTime) : 0.1f));
        }
        //otherwise, fine...
    }
    else
    {
        //we're in sync
        m_fTimeVelocityBecameOutOfSync = 0.f;
    }
}
#endif

Robot::Robot() : Entity(0, 0, 1.565217391304348f, 2.0f),
m_fJumpSpeed(8.0f),
m_fSpeed(7.5f),
m_fTimeOfNextShot(0.0f),
m_fWallRestitution(0.1f),
m_fCatRestitution(0.1f),
m_fTimePositionBecameOutOfSync(0.0f),
m_fTimeVelocityBecameOutOfSync(0.0f),
m_iHealth(5),
m_isFacingLeft(false),
m_isGrounded(false),
m_isFalling(false),
m_isShooting(false),
m_iPlayerId(0),
m_iReadState(0),
m_iIndexInWorld(-1)
{
    m_acceleration.setY(-9.8f);
}

RTTI_IMPL(Robot, Entity);

NETWORK_TYPE_IMPL(Robot);
