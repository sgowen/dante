//
//  RoboCatServer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "RoboCatServer.h"

#include "NetworkManagerServer.h"
#include "Timing.h"
#include "StringUtils.h"

GameObject* RoboCatServer::create()
{
    RoboCatServer* ret = new RoboCatServer();
    NetworkManagerServer::getInstance()->RegisterGameObject(ret);
    
    return ret;
}

RoboCatServer::RoboCatServer() : RoboCat()
{
    // Empty
}

void RoboCatServer::HandleDying()
{
    NetworkManagerServer::getInstance()->UnregisterGameObject(this);
}

void RoboCatServer::Update()
{
    RoboCat::Update();
    
    Vector2 oldLocation = GetLocation();
    Vector2 oldVelocity = GetVelocity();
    float oldRotation = GetRotation();
    
    // is there a move we haven't processed yet?
    ClientProxy* client = NetworkManagerServer::getInstance()->GetClientProxy(GetPlayerId());
    if (client)
    {
        MoveList& moveList = client->GetUnprocessedMoveList();
        for (const Move& unprocessedMove : moveList)
        {
            const InputState& currentState = unprocessedMove.GetInputState();
            
            float deltaTime = unprocessedMove.GetDeltaTime();
            
            ProcessInput(deltaTime, currentState);
            SimulateMovement(deltaTime);
            
            LOG("Server Move Time: %3.4f deltaTime: %3.4f left rot at %3.4f", unprocessedMove.GetTimestamp(), deltaTime, GetRotation());
        }
        
        moveList.Clear();
    }
    
    if (!oldLocation.isEqualTo(GetLocation()) ||
        !oldVelocity.isEqualTo(GetVelocity()) ||
        oldRotation != GetRotation())
    {
        NetworkManagerServer::getInstance()->SetStateDirty(GetNetworkId(), ECRS_Pose);
    }
}

uint32_t RoboCatServer::Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & ECRS_PlayerId)
    {
        inOutputStream.Write((bool)true);
        inOutputStream.Write(GetPlayerId());
        
        writtenState |= ECRS_PlayerId;
    }
    else
    {
        inOutputStream.Write((bool)false);
    }
    
    if (inDirtyState & ECRS_Pose)
    {
        inOutputStream.Write((bool)true);
        
        Vector2 velocity = mVelocity;
        inOutputStream.Write(velocity.getX());
        inOutputStream.Write(velocity.getY());
        
        Vector2 location = GetLocation();
        inOutputStream.Write(location.getX());
        inOutputStream.Write(location.getY());
        
        inOutputStream.Write(GetRotation());
        
        writtenState |= ECRS_Pose;
    }
    else
    {
        inOutputStream.Write((bool)false);
    }
    
    //always write mThrustDir- it's just two bits
    if (m_fThrustDir != 0.f)
    {
        inOutputStream.Write(true);
        inOutputStream.Write(m_fThrustDir > 0.f);
    }
    else
    {
        inOutputStream.Write(false);
    }
    
    if (inDirtyState & ECRS_Color)
    {
        inOutputStream.Write((bool)true);
        inOutputStream.Write(GetColor());
        
        writtenState |= ECRS_Color;
    }
    else
    {
        inOutputStream.Write((bool)false);
    }
    
    return writtenState;
}

RTTI_IMPL(RoboCatServer, RoboCat);
