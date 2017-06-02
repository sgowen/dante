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

Entity* RoboCatClient::create()
{
    return new RoboCatClient();
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
    // TODO, allow for multiple client inputs
    if (getPlayerId() == NetworkManagerClient::getInstance()->getPlayerId())
    {
        const Move* pendingMove = InputManager::getInstance()->getAndClearPendingMove();
        //in theory, only do this if we want to sample input this frame / if there's a new move (since we have to keep in sync with server)
        if (pendingMove) //is it time to sample a new move...
        {
            float deltaTime = pendingMove->getDeltaTime();
            
            //apply that input
            
            processInput(deltaTime, pendingMove->getInputState());
            
            //and simulate!
            
            simulateMovement(deltaTime);
            
            LOG("Client Move Time: %3.4f deltaTime: %3.4f", pendingMove->getTimestamp(), deltaTime);
        }
    }
    else
    {
        simulateMovement(Timing::getInstance()->getDeltaTime());
        
        if (getVelocity().isEqualTo(Vector2::Zero))
        {
            //we're in sync if our velocity is 0
            m_fTimeLocationBecameOutOfSync = 0.f;
        }
    }
}

void RoboCatClient::read(InputMemoryBitStream& inInputStream)
{
    Vector2 oldLocation = getPosition();
    Vector2 oldVelocity = getVelocity();
    
    RoboCat::read(inInputStream);
    
    if (getPlayerId() == NetworkManagerClient::getInstance()->getPlayerId())
    {
        doClientSidePredictionAfterReplicationForLocalCat(m_iReadState);
        
        //if this is a create packet, don't interpolate
        if ((m_iReadState & ECRS_PlayerId) == 0)
        {
            interpolateClientSidePrediction(oldLocation, oldVelocity, false);
        }
    }
    else
    {
        doClientSidePredictionAfterReplicationForRemoteCat(m_iReadState);
        
        //will this smooth us out too? it'll interpolate us just 10% of the way there...
        if ((m_iReadState & ECRS_PlayerId) == 0)
        {
            interpolateClientSidePrediction(oldLocation, oldVelocity, true);
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
            float deltaTime = move.getDeltaTime();
            processInput(deltaTime, move.getInputState());
            
            simulateMovement(deltaTime);
        }
    }
}

//so what do we want to do here? need to do some kind of interpolation...

void RoboCatClient::doClientSidePredictionAfterReplicationForRemoteCat(uint32_t inReadState)
{
    if ((inReadState & ECRS_Pose) != 0)
    {
        // simulate movement for an additional RTT
        float rtt = NetworkManagerClient::getInstance()->getRoundTripTime();
        LOG("Other cat came in, simulating for an extra %f", rtt);
        
        // let's break into framerate sized chunks so we don't run through walls and do crazy things...
        while (true)
        {
            if (rtt < FRAME_RATE)
            {
                simulateMovement(rtt);
                break;
            }
            else
            {
                simulateMovement(FRAME_RATE);
                rtt -= FRAME_RATE;
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

void RoboCatClient::interpolateClientSidePrediction(Vector2& inOldLocation, Vector2& inOldVelocity, bool inIsForRemoteCat)
{
    float roundTripTime = NetworkManagerClient::getInstance()->getRoundTripTime();
    
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
