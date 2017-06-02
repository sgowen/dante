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

Entity* RoboCatServer::create()
{
    RoboCatServer* ret = new RoboCatServer();
    
    NetworkManagerServer::getInstance()->registerEntity(ret);
    
    return ret;
}

RoboCatServer::RoboCatServer() : RoboCat()
{
    // Empty
}

void RoboCatServer::onDeletion()
{
    NetworkManagerServer::getInstance()->unregisterEntity(this);
}

void RoboCatServer::update()
{
    Vector2 oldLocation = getPosition();
    Vector2 oldVelocity = getVelocity();
    
    // is there a move we haven't processed yet?
    ClientProxy* client = NetworkManagerServer::getInstance()->getClientProxy(getPlayerId());
    if (client)
    {
        MoveList& moveList = client->getUnprocessedMoveList();
        for (const Move& unprocessedMove : moveList)
        {
            IInputState* currentState = unprocessedMove.getInputState();
            
            float deltaTime = unprocessedMove.getDeltaTime();
            
            processInput(deltaTime, currentState);
            simulateMovement(deltaTime);
            
            LOG("Server Move Time: %3.4f deltaTime: %3.4f", unprocessedMove.getTimestamp(), deltaTime);
        }
        
        moveList.clear();
    }
    
    if (!oldLocation.isEqualTo(getPosition()) ||
        !oldVelocity.isEqualTo(getVelocity()))
    {
        NetworkManagerServer::getInstance()->setStateDirty(getID(), ECRS_Pose);
    }
}

RTTI_IMPL(RoboCatServer, RoboCat);
