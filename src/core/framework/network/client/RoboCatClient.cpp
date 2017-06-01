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

GameObject* RoboCatClient::create()
{
    return static_cast<GameObject*>(new RoboCatClient());
}

void RoboCatClient::HandleDying()
{
    RoboCat::HandleDying();
    
    //and if we're local, tell the hud so our health goes away!
    if (GetPlayerId() == NetworkManagerClient::getInstance()->GetPlayerId())
    {
        //HUD::sInstance->SetPlayerHealth(0);
    }
}

void RoboCatClient::Update()
{
    //is this the cat owned by us?
    if (GetPlayerId() == NetworkManagerClient::getInstance()->GetPlayerId())
    {
        const Move* pendingMove = InputManager::sInstance->GetAndClearPendingMove();
        //in theory, only do this if we want to sample input this frame / if there's a new move (since we have to keep in sync with server)
        if (pendingMove) //is it time to sample a new move...
        {
            float deltaTime = pendingMove->GetDeltaTime();
            
            //apply that input
            
            ProcessInput(deltaTime, pendingMove->GetInputState());
            
            //and simulate!
            
            SimulateMovement(deltaTime);
            
            LOG("Client Move Time: %3.4f deltaTime: %3.4f left rot at %3.4f", pendingMove->GetTimestamp(), deltaTime, GetRotation());
        }
    }
    else
    {
        SimulateMovement(Timing::getInstance()->GetDeltaTime());
        
        if (GetVelocity().isEqualTo(Vector2::Zero))
        {
            //we're in sync if our velocity is 0
            m_fTimeLocationBecameOutOfSync = 0.f;
        }
    }
}

void RoboCatClient::Read(InputMemoryBitStream& inInputStream)
{
    bool stateBit;
    
    uint32_t readState = 0;
    
    inInputStream.Read(stateBit);
    if (stateBit)
    {
        uint32_t playerId;
        inInputStream.Read(playerId);
        SetPlayerId(playerId);
        readState |= ECRS_PlayerId;
    }
    
    float oldRotation = GetRotation();
    Vector2 oldLocation = GetLocation();
    Vector2 oldVelocity = GetVelocity();
    
    float replicatedRotation;
    Vector2 replicatedLocation;
    Vector2 replicatedVelocity;
    
    inInputStream.Read(stateBit);
    if (stateBit)
    {
        inInputStream.Read(replicatedVelocity.getXRef());
        inInputStream.Read(replicatedVelocity.getYRef());
        
        SetVelocity(replicatedVelocity);
        
        inInputStream.Read(replicatedLocation.getXRef());
        inInputStream.Read(replicatedLocation.getYRef());
        
        SetLocation(replicatedLocation);
        
        inInputStream.Read(replicatedRotation);
        SetRotation(replicatedRotation);
        
        readState |= ECRS_Pose;
    }
    
    inInputStream.Read(stateBit);
    if (stateBit)
    {
        inInputStream.Read(stateBit);
        m_fThrustDir = stateBit ? 1.f : -1.f;
    }
    else
    {
        m_fThrustDir = 0.f;
    }
    
    inInputStream.Read(stateBit);
    if (stateBit)
    {
        Color color;
        inInputStream.Read(color);
        SetColor(color);
        readState |= ECRS_Color;
    }
    
    if (GetPlayerId() == NetworkManagerClient::getInstance()->GetPlayerId())
    {
        DoClientSidePredictionAfterReplicationForLocalCat(readState);
        
        //if this is a create packet, don't interpolate
        if ((readState & ECRS_PlayerId) == 0)
        {
            InterpolateClientSidePrediction(oldRotation, oldLocation, oldVelocity, false);
        }
    }
    else
    {
        DoClientSidePredictionAfterReplicationForRemoteCat(readState);
        
        //will this smooth us out too? it'll interpolate us just 10% of the way there...
        if ((readState & ECRS_PlayerId) == 0)
        {
            InterpolateClientSidePrediction(oldRotation, oldLocation, oldVelocity, true);
        }
    }
}

void RoboCatClient::DoClientSidePredictionAfterReplicationForLocalCat(uint32_t inReadState)
{
    if ((inReadState & ECRS_Pose) != 0)
    {
        //simulate pose only if we received new pose- might have just gotten thrustDir
        //in which case we don't need to replay moves because we haven't warped backwards
        
        //all processed moves have been removed, so all that are left are unprocessed moves
        //so we must apply them...
        const MoveList& moveList = InputManager::sInstance->GetMoveList();
        
        for (const Move& move : moveList)
        {
            float deltaTime = move.GetDeltaTime();
            ProcessInput(deltaTime, move.GetInputState());
            
            SimulateMovement(deltaTime);
        }
    }
}

//so what do we want to do here? need to do some kind of interpolation...

void RoboCatClient::DoClientSidePredictionAfterReplicationForRemoteCat(uint32_t inReadState)
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

void RoboCatClient::InterpolateClientSidePrediction(float inOldRotation, Vector2& inOldLocation, Vector2& inOldVelocity, bool inIsForRemoteCat)
{
    if (inOldRotation != GetRotation() && !inIsForRemoteCat)
    {
        LOG("ERROR! Move replay ended with incorrect rotation!", 0);
    }
    
    float roundTripTime = NetworkManagerClient::getInstance()->GetRoundTripTime();
    
    if (!inOldLocation.isEqualTo(GetLocation()))
    {
        LOG("ERROR! Move replay ended with incorrect location!", 0);
        
        //have we been out of sync, or did we just become out of sync?
        float time = Timing::getInstance()->GetFrameStartTime();
        if (m_fTimeLocationBecameOutOfSync == 0.f)
        {
            m_fTimeLocationBecameOutOfSync = time;
        }
        
        float durationOutOfSync = time - m_fTimeLocationBecameOutOfSync;
        if (durationOutOfSync < roundTripTime)
        {
            SetLocation(lerp(inOldLocation, GetLocation(), inIsForRemoteCat ? (durationOutOfSync / roundTripTime) : 0.1f));
        }
    }
    else
    {
        //we're in sync
        m_fTimeLocationBecameOutOfSync = 0.f;
    }
    
    if (!inOldVelocity.isEqualTo(GetVelocity()))
    {
        LOG("ERROR! Move replay ended with incorrect velocity!", 0);
        
        //have we been out of sync, or did we just become out of sync?
        float time = Timing::getInstance()->GetFrameStartTime();
        if (m_fTimeVelocityBecameOutOfSync == 0.f)
        {
            m_fTimeVelocityBecameOutOfSync = time;
        }
        
        //now interpolate to the correct value...
        float durationOutOfSync = time - m_fTimeVelocityBecameOutOfSync;
        if (durationOutOfSync < roundTripTime)
        {
            SetVelocity(lerp(inOldVelocity, GetVelocity(), inIsForRemoteCat ? (durationOutOfSync / roundTripTime) : 0.1f));
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
