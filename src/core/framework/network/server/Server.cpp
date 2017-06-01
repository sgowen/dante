//
//  Server.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Server.h"

#include "NetworkManagerServer.h"
#include "EntityRegistry.h"
#include "World.h"
#include "Timing.h"
#include "FrameworkConstants.h"

bool Server::StaticInit()
{
    sInstance.reset(new Server());
    
    return true;
}

Server::Server() : m_fFrameStateTime(0)
{
    EntityRegistry::sInstance->RegisterCreationFunction(NETWORK_TYPE_RoboCat, RoboCatServer::create);
    
    InitNetworkManager();
}

int Server::Run()
{
    while (true)
    {
        Timing::getInstance()->update();
        
        m_fFrameStateTime += Timing::getInstance()->getDeltaTime();
        
        if (m_fFrameStateTime >= FRAME_RATE)
        {
            NetworkManagerServer::getInstance()->ProcessIncomingPackets();
            
            NetworkManagerServer::getInstance()->CheckForDisconnects();
            
            NetworkManagerServer::getInstance()->RespawnCats();
            
            while (m_fFrameStateTime >= FRAME_RATE)
            {
                m_fFrameStateTime -= FRAME_RATE;
                
                DoFrame();
            }
            
            NetworkManagerServer::getInstance()->SendOutgoingPackets();
        }
    }
    
    return 1;
}

bool Server::InitNetworkManager()
{
    std::string portString = "9999";
    uint16_t port = stoi(portString);
    
    return NetworkManagerServer::StaticInit(port);
}

void Server::HandleNewClient(ClientProxy* inClientProxy)
{
    int playerId = inClientProxy->getPlayerId();
    
    SpawnCatForPlayer(playerId);
}

void Server::SpawnCatForPlayer(int inPlayerId)
{
    RoboCat* cat = static_cast<RoboCat*>(EntityRegistry::sInstance->CreateNWPhysicalEntity(NETWORK_TYPE_RoboCat));
    cat->setPlayerId(inPlayerId);
    //gotta pick a better spawn location than this...
    cat->setPosition(Vector2(8.f - static_cast<float>(inPlayerId), 4.0f));
    
    static Color Red(1.0f, 0.0f, 0.0f, 1);
    static Color Blue(0.0f, 0.0f, 1.0f, 1);
    static Color Green(0.0f, 1.0f, 0.0f, 1);
    static Color LightYellow(1.0f, 1.0f, 0.88f, 1);
    
    switch (inPlayerId)
    {
        case 1:
            cat->setColor(Red);
            break;
        case 2:
            cat->setColor(Blue);
            break;
        case 3:
            cat->setColor(Green);
            break;
        case 4:
            cat->setColor(LightYellow);
            break;
        default:
            break;
    }
}

void Server::HandleLostClient(ClientProxy* inClientProxy)
{
    //kill client's cat
    //remove client from scoreboard
    int playerId = inClientProxy->getPlayerId();
    
    RoboCat* cat = GetCatForPlayer(playerId);
    if (cat)
    {
        cat->requestDeletion();
    }
}

RoboCat* Server::GetCatForPlayer(int inPlayerId)
{
    //run through the objects till we find the cat...
    //it would be nice if we kept a pointer to the cat on the clientproxy
    //but then we'd have to clean it up when the cat died, etc.
    //this will work for now until it's a perf issue
    const auto& gameObjects = World::sInstance->GetRoboCats();
    int len = static_cast<int>(gameObjects.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        NWPhysicalEntity* go = gameObjects[i];
        RoboCat* cat = go->getRTTI().derivesFrom(RoboCat::rtti) ? (RoboCat*)go : nullptr;
        if (cat && cat->getPlayerId() == inPlayerId)
        {
            return static_cast<RoboCat*>(go);
        }
    }
    
    return nullptr;
}
