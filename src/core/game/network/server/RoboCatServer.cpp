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
#include "StringUtil.h"

NWPhysicalEntity* RoboCatServer::create()
{
    RoboCatServer* ret = new RoboCatServer();
    NetworkManagerServer::getInstance()->RegisterNWPhysicalEntity(ret);
    
    return ret;
}

RoboCatServer::RoboCatServer() : RoboCat()
{
    // Empty
}

void RoboCatServer::onDeletion()
{
    NetworkManagerServer::getInstance()->UnregisterNWPhysicalEntity(this);
}

void RoboCatServer::update()
{
    Vector2 oldLocation = getPosition();
    Vector2 oldVelocity = getVelocity();
    float oldRotation = getAngle();
    
    // is there a move we haven't processed yet?
    ClientProxy* client = NetworkManagerServer::getInstance()->GetClientProxy(getPlayerId());
    if (client)
    {
        MoveList& moveList = client->GetUnprocessedMoveList();
        for (const Move& unprocessedMove : moveList)
        {
            const InputState& currentState = unprocessedMove.GetInputState();
            
            float deltaTime = unprocessedMove.GetDeltaTime();
            
            ProcessInput(deltaTime, currentState);
            SimulateMovement(deltaTime);
            
            LOG("Server Move Time: %3.4f deltaTime: %3.4f left rot at %3.4f", unprocessedMove.GetTimestamp(), deltaTime, getAngle());
        }
        
        moveList.Clear();
    }
    
    if (!oldLocation.isEqualTo(getPosition()) ||
        !oldVelocity.isEqualTo(getVelocity()) ||
        oldRotation != getAngle())
    {
        NetworkManagerServer::getInstance()->SetStateDirty(getID(), ECRS_Pose);
    }
}

uint32_t RoboCatServer::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    if (inDirtyState & ECRS_PlayerId)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(getPlayerId());
        
        writtenState |= ECRS_PlayerId;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    if (inDirtyState & ECRS_Pose)
    {
        inOutputStream.write((bool)true);
        
        Vector2 velocity = m_velocity;
        inOutputStream.write(velocity.getX());
        inOutputStream.write(velocity.getY());
        
        Vector2 location = getPosition();
        inOutputStream.write(location.getX());
        inOutputStream.write(location.getY());
        
        inOutputStream.write(getAngle());
        
        writtenState |= ECRS_Pose;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    //always write mThrustDir- it's just two bits
    if (m_fThrustDir != 0.f)
    {
        inOutputStream.write(true);
        inOutputStream.write(m_fThrustDir > 0.f);
    }
    else
    {
        inOutputStream.write(false);
    }
    
    if (inDirtyState & ECRS_Color)
    {
        inOutputStream.write((bool)true);
        inOutputStream.write(getColor());
        
        writtenState |= ECRS_Color;
    }
    else
    {
        inOutputStream.write((bool)false);
    }
    
    return writtenState;
}

RTTI_IMPL(RoboCatServer, RoboCat);

NETWORK_TYPE_IMPL(RoboCatServer);
