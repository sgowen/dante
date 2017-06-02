//
//  RobotServer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "RobotServer.h"

#include "NetworkManagerServer.h"
#include "Timing.h"
#include "StringUtil.h"
#include "Server.h"

Entity* RobotServer::create()
{
    RobotServer* ret = new RobotServer();
    
    NetworkManagerServer::getInstance()->registerEntity(ret);
    
    return ret;
}

void RobotServer::onDeletion()
{
    NetworkManagerServer::getInstance()->unregisterEntity(this);
}

void RobotServer::update()
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
    
    handleShooting();
    
    if (!oldLocation.isEqualTo(getPosition()) ||
        !oldVelocity.isEqualTo(getVelocity()))
    {
        NetworkManagerServer::getInstance()->setStateDirty(getID(), RBRS_Pose);
    }
}

void RobotServer::takeDamage()
{
    m_iHealth--;
    
    if (m_iHealth <= 0)
    {
        requestDeletion();
        
        ClientProxy* clientProxy = NetworkManagerServer::getInstance()->getClientProxy(getPlayerId());
        
        Server::staticHandleNewClient(clientProxy);
    }
    
    // tell the world our health dropped
    NetworkManagerServer::getInstance()->setStateDirty(getID(), RBRS_Health);
}

void RobotServer::handleShooting()
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

RobotServer::RobotServer() : Robot(), m_fTimeOfNextShot(0)
{
    // Empty
}

RTTI_IMPL(RobotServer, Robot);
