//
//  RoboCatServer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "RoboCatServer.h"

#include "Timing.h"
#include "StringUtils.h"

RoboCatServer::RoboCatServer() :
m_fTimeOfNextShot(0.f),
m_fTimeBetweenShots(0.2f)
{}

void RoboCatServer::HandleDying()
{
    NetworkManagerServer::sInstance->UnregisterGameObject(this);
}

void RoboCatServer::Update()
{
    RoboCat::Update();
    
    Vector3 oldLocation = GetLocation();
    Vector3 oldVelocity = GetVelocity();
    float oldRotation = GetRotation();
    
    // is there a move we haven't processed yet?
    ClientProxyPtr client = NetworkManagerServer::sInstance->GetClientProxy(GetPlayerId());
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
    
    if (!RoboMath::Is2DVectorEqual(oldLocation, GetLocation()) ||
        !RoboMath::Is2DVectorEqual(oldVelocity, GetVelocity()) ||
        oldRotation != GetRotation())
    {
        NetworkManagerServer::sInstance->SetStateDirty(GetNetworkId(), ECRS_Pose);
    }
}
