//
//  Server.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Server.h"

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
    GameObjectRegistry::sInstance->RegisterCreationFunction('RCAT', RoboCatServer::StaticCreate);
    
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
            NetworkManagerServer::sInstance->ProcessIncomingPackets();
            
            NetworkManagerServer::sInstance->CheckForDisconnects();
            
            NetworkManagerServer::sInstance->RespawnCats();
            
            while (m_fFrameStateTime >= FRAME_RATE)
            {
                m_fFrameStateTime -= FRAME_RATE;
                
                DoFrame();
            }
            
            NetworkManagerServer::sInstance->SendOutgoingPackets();
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

void Server::HandleNewClient(ClientProxyPtr inClientProxy)
{
    int playerId = inClientProxy->GetPlayerId();
    
    SpawnCatForPlayer(playerId);
}

void Server::SpawnCatForPlayer(int inPlayerId)
{
    RoboCatPtr cat = std::static_pointer_cast< RoboCat >(GameObjectRegistry::sInstance->CreateGameObject('RCAT'));
    cat->SetPlayerId(inPlayerId);
    //gotta pick a better spawn location than this...
    cat->SetLocation(Vector3(8.f - static_cast<float>(inPlayerId), 4.f, 0.f));
    switch (inPlayerId)
    {
        case 1:
            cat->SetColor(Colors::Red);
            break;
        case 2:
            cat->SetColor(Colors::Blue);
            break;
        case 3:
            cat->SetColor(Colors::LightGreen);
            break;
        case 4:
            cat->SetColor(Colors::LightYellow);
            break;
        default:
            break;
    }
}

void Server::HandleLostClient(ClientProxyPtr inClientProxy)
{
    //kill client's cat
    //remove client from scoreboard
    int playerId = inClientProxy->GetPlayerId();
    
    RoboCatPtr cat = GetCatForPlayer(playerId);
    if (cat)
    {
        cat->SetDoesWantToDie(true);
    }
}

RoboCatPtr Server::GetCatForPlayer(int inPlayerId)
{
    //run through the objects till we find the cat...
    //it would be nice if we kept a pointer to the cat on the clientproxy
    //but then we'd have to clean it up when the cat died, etc.
    //this will work for now until it's a perf issue
    const auto& gameObjects = World::sInstance->GetGameObjects();
    int len = static_cast<int>(gameObjects.size());
    for (int i = 0, c = len; i < c; ++i)
    {
        GameObjectPtr go = gameObjects[i];
        RoboCat* cat = go->GetAsCat();
        if (cat && cat->GetPlayerId() == inPlayerId)
        {
            return std::static_pointer_cast< RoboCat >(go);
        }
    }
    
    return nullptr;
}
