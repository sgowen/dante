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
#include "GameObjectRegistry.h"
#include "World.h"
#include "Timing.h"

#define FRAME_RATE 0.01666666666667f // 60 frames per second

bool Server::StaticInit()
{
    sInstance.reset(new Server());
    
    return true;
}

Server::Server() : m_fFrameStateTime(0)
{
    GameObjectRegistry::sInstance->RegisterCreationFunction(NETWORK_TYPE_RoboCat, RoboCatServer::create);
    
    InitNetworkManager();
}

int Server::Run()
{
    while (true)
    {
        Timing::sInstance.Update();
        
        m_fFrameStateTime += Timing::sInstance.GetDeltaTime();
        
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
    int playerId = inClientProxy->GetPlayerId();
    
    SpawnCatForPlayer(playerId);
}

void Server::SpawnCatForPlayer(int inPlayerId)
{
    RoboCat* cat = static_cast<RoboCat*>(GameObjectRegistry::sInstance->CreateGameObject(NETWORK_TYPE_RoboCat));
    cat->SetPlayerId(inPlayerId);
    //gotta pick a better spawn location than this...
    cat->SetLocation(Vector2(8.f - static_cast<float>(inPlayerId), 4.0f));
    
    static Color Red(1.0f, 0.0f, 0.0f, 1);
    static Color Blue(0.0f, 0.0f, 1.0f, 1);
    static Color Green(0.0f, 1.0f, 0.0f, 1);
    static Color LightYellow(1.0f, 1.0f, 0.88f, 1);
    
    switch (inPlayerId)
    {
        case 1:
            cat->SetColor(Red);
            break;
        case 2:
            cat->SetColor(Blue);
            break;
        case 3:
            cat->SetColor(Green);
            break;
        case 4:
            cat->SetColor(LightYellow);
            break;
        default:
            break;
    }
}

void Server::HandleLostClient(ClientProxy* inClientProxy)
{
    //kill client's cat
    //remove client from scoreboard
    int playerId = inClientProxy->GetPlayerId();
    
    RoboCat* cat = GetCatForPlayer(playerId);
    if (cat)
    {
        cat->SetDoesWantToDie(true);
    }
}

RoboCat* Server::GetCatForPlayer(int inPlayerId)
{
    //run through the objects till we find the cat...
    //it would be nice if we kept a pointer to the cat on the clientproxy
    //but then we'd have to clean it up when the cat died, etc.
    //this will work for now until it's a perf issue
    const auto& gameObjects = World::sInstance->GetGameObjects();
    int len = static_cast<int>(gameObjects.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        GameObject* go = gameObjects[i];
        RoboCat* cat = go->GetAsCat();
        if (cat && cat->GetPlayerId() == inPlayerId)
        {
            return static_cast<RoboCat*>(go);
        }
    }
    
    return nullptr;
}
