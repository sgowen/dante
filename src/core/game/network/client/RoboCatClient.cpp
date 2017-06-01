//
//  RoboCatClient.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "RoboCatClient.h"

#include "NetworkManagerClient.h"
#include "Move.h"
#include "InputManager.h"
#include "StringUtil.h"
#include "Timing.h"
#include "Color.h"
#include "Vector2.h"

NWPhysicalEntity* RoboCatClient::create()
{
    return static_cast<NWPhysicalEntity*>(new RoboCatClient());
}

void RoboCatClient::onDeletion()
{
    RoboCat::onDeletion();
    
    //and if we're local, tell the hud so our health goes away!
    if (getPlayerId() == NetworkManagerClient::getInstance()->getPlayerId())
    {
        // Show a death animation or something?
    }
}

void RoboCatClient::update()
{
    //is this the cat owned by us?
    if (getPlayerId() == NetworkManagerClient::getInstance()->getPlayerId())
    {
        const Move* pendingMove = InputManager::getInstance()->getAndClearPendingMove();
        //in theory, only do this if we want to sample input this frame / if there's a new move (since we have to keep in sync with server)
        if (pendingMove) //is it time to sample a new move...
        {
            float deltaTime = pendingMove->GetDeltaTime();
            
            //apply that input
            
            ProcessInput(deltaTime, pendingMove->GetInputState());
            
            //and simulate!
            
            SimulateMovement(deltaTime);
            
            LOG("Client Move Time: %3.4f deltaTime: %3.4f left rot at %3.4f", pendingMove->GetTimestamp(), deltaTime, getAngle());
        }
    }
    else
    {
        SimulateMovement(Timing::getInstance()->getDeltaTime());
        
        if (getVelocity().isEqualTo(Vector2::Zero))
        {
            //we're in sync if our velocity is 0
            m_fTimeLocationBecameOutOfSync = 0.f;
        }
    }
}

void RoboCatClient::read(InputMemoryBitStream& inInputStream)
{
    bool stateBit;
    
    uint32_t readState = 0;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        uint32_t playerId;
        inInputStream.read(playerId);
        setPlayerId(playerId);
        readState |= ECRS_PlayerId;
    }
    
    float oldRotation = getAngle();
    Vector2 oldLocation = getPosition();
    Vector2 oldVelocity = getVelocity();
    
    float replicatedRotation;
    Vector2 replicatedLocation;
    Vector2 replicatedVelocity;
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(replicatedVelocity.getXRef());
        inInputStream.read(replicatedVelocity.getYRef());
        
        m_velocity.set(replicatedVelocity);
        
        inInputStream.read(replicatedLocation.getXRef());
        inInputStream.read(replicatedLocation.getYRef());
        
        setPosition(replicatedLocation);
        
        inInputStream.read(replicatedRotation);
        setAngle(replicatedRotation);
        
        readState |= ECRS_Pose;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        inInputStream.read(stateBit);
        m_fThrustDir = stateBit ? 1.f : -1.f;
    }
    else
    {
        m_fThrustDir = 0.f;
    }
    
    inInputStream.read(stateBit);
    if (stateBit)
    {
        Color color;
        inInputStream.read(color);
        setColor(color);
        readState |= ECRS_Color;
    }
    
    if (getPlayerId() == NetworkManagerClient::getInstance()->getPlayerId())
    {
        doClientSidePredictionAfterReplicationForLocalCat(readState);
        
        //if this is a create packet, don't interpolate
        if ((readState & ECRS_PlayerId) == 0)
        {
            interpolateClientSidePrediction(oldRotation, oldLocation, oldVelocity, false);
        }
    }
    else
    {
        doClientSidePredictionAfterReplicationForRemoteCat(readState);
        
        //will this smooth us out too? it'll interpolate us just 10% of the way there...
        if ((readState & ECRS_PlayerId) == 0)
        {
            interpolateClientSidePrediction(oldRotation, oldLocation, oldVelocity, true);
        }
    }
}

void RoboCatClient::doClientSidePredictionAfterReplicationForLocalCat(uint32_t inReadState)
{
    if ((inReadState & ECRS_Pose) != 0)
    {
        //simulate pose only if we received new pose- might have just gotten thrustDir
        //in which case we don't need to replay moves because we haven't warped backwards
        
        //all processed moves have been removed, so all that are left are unprocessed moves
        //so we must apply them...
        const MoveList& moveList = InputManager::getInstance()->getMoveList();
        
        for (const Move& move : moveList)
        {
            float deltaTime = move.GetDeltaTime();
            ProcessInput(deltaTime, move.GetInputState());
            
            SimulateMovement(deltaTime);
        }
    }
}

//so what do we want to do here? need to do some kind of interpolation...

void RoboCatClient::doClientSidePredictionAfterReplicationForRemoteCat(uint32_t inReadState)
{
    if ((inReadState & ECRS_Pose) != 0)
    {
        //simulate movement for an additional RTT
        float rtt = NetworkManagerClient::getInstance()->GetRoundTripTime();
        //LOG("Other cat came in, simulating for an extra %f", rtt);
        
        //let's break into framerate sized chunks though so that we don't run through walls and do crazy things...
        float deltaTime = 1.f / 60.f;
        
        while (true)
        {
            if (rtt < deltaTime)
            {
                SimulateMovement(rtt);
                break;
            }
            else
            {
                SimulateMovement(deltaTime);
                rtt -= deltaTime;
            }
        }
    }
}

RoboCatClient::RoboCatClient() :
m_fTimeLocationBecameOutOfSync(0.f),
m_fTimeVelocityBecameOutOfSync(0.f)
{
    // Empty
}

void RoboCatClient::interpolateClientSidePrediction(float inOldRotation, Vector2& inOldLocation, Vector2& inOldVelocity, bool inIsForRemoteCat)
{
    if (inOldRotation != getAngle() && !inIsForRemoteCat)
    {
        LOG("ERROR! Move replay ended with incorrect rotation!", 0);
    }
    
    float roundTripTime = NetworkManagerClient::getInstance()->GetRoundTripTime();
    
    if (!inOldLocation.isEqualTo(getPosition()))
    {
        LOG("ERROR! Move replay ended with incorrect location!", 0);
        
        //have we been out of sync, or did we just become out of sync?
        float time = Timing::getInstance()->getFrameStartTime();
        if (m_fTimeLocationBecameOutOfSync == 0.f)
        {
            m_fTimeLocationBecameOutOfSync = time;
        }
        
        float durationOutOfSync = time - m_fTimeLocationBecameOutOfSync;
        if (durationOutOfSync < roundTripTime)
        {
            setPosition(lerp(inOldLocation, getPosition(), inIsForRemoteCat ? (durationOutOfSync / roundTripTime) : 0.1f));
        }
    }
    else
    {
        //we're in sync
        m_fTimeLocationBecameOutOfSync = 0.f;
    }
    
    if (!inOldVelocity.isEqualTo(getVelocity()))
    {
        LOG("ERROR! Move replay ended with incorrect velocity!", 0);
        
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

RTTI_IMPL(RoboCatClient, RoboCat);

NETWORK_TYPE_IMPL(RoboCatClient);
