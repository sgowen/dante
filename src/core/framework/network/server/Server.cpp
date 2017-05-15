//
//  Server.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

//uncomment this when you begin working on the server

#define FRAME_RATE 0.01666666666667f // 60 frames per second

bool Server::StaticInit()
{
	sInstance.reset(new Server());

	return true;
}

Server::Server() : m_fFrameStateTime(0)
{
	GameObjectRegistry::sInstance->RegisterCreationFunction('RCAT', RoboCatServer::StaticCreate);
	GameObjectRegistry::sInstance->RegisterCreationFunction('MOUS', RodentServer::StaticCreate);
	GameObjectRegistry::sInstance->RegisterCreationFunction('YARN', YarnServer::StaticCreate);

	InitNetworkManager();
	
	//NetworkManagerServer::sInstance->SetDropPacketChance(0.8f);
	//NetworkManagerServer::sInstance->SetSimulatedLatency(0.25f);
	//NetworkManagerServer::sInstance->SetSimulatedLatency(0.5f);
	//NetworkManagerServer::sInstance->SetSimulatedLatency(0.1f);
}

int Server::Run()
{
	SetupWorld();

    while (mShouldKeepRunning)
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
	string portString = "9999";//StringUtils::GetCommandLineArg(1);
	uint16_t port = stoi(portString);

	return NetworkManagerServer::StaticInit(port);
}

namespace
{
	void CreateRandomMice(int inRodentCount)
	{
		Vector3 mouseMin(0, 0, 0.f);
		Vector3 mouseMax(10.f, 6.0f, 0.f);
		GameObjectPtr go;

		//make a mouse somewhere- where will these come from?
		for (int i = 0; i < inRodentCount; ++i)
		{
			go = GameObjectRegistry::sInstance->CreateGameObject('MOUS');
			Vector3 mouseLocation = RoboMath::GetRandomVector(mouseMin, mouseMax);
			go->SetLocation(mouseLocation);
		}
	}
}

void Server::SetupWorld()
{
	//spawn some random mice
	CreateRandomMice(10);
	
	//spawn more random mice!
	CreateRandomMice(10);
}

void Server::HandleNewClient(ClientProxyPtr inClientProxy)
{
	int playerId = inClientProxy->GetPlayerId();
	
	ScoreBoardManager::sInstance->AddEntry(playerId, inClientProxy->GetName());
	SpawnCatForPlayer(playerId);
}

void Server::SpawnCatForPlayer(int inPlayerId)
{
	RoboCatPtr cat = std::static_pointer_cast< RoboCat >(GameObjectRegistry::sInstance->CreateGameObject('RCAT'));
	cat->SetColor(ScoreBoardManager::sInstance->GetEntry(inPlayerId)->GetColor());
	cat->SetPlayerId(inPlayerId);
	//gotta pick a better spawn location than this...
	cat->SetLocation(Vector3(1.f - static_cast< float >(inPlayerId), 0.f, 0.f));
}

void Server::HandleLostClient(ClientProxyPtr inClientProxy)
{
	//kill client's cat
	//remove client from scoreboard
	int playerId = inClientProxy->GetPlayerId();

	ScoreBoardManager::sInstance->RemoveEntry(playerId);
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
	for (int i = 0, c = gameObjects.size(); i < c; ++i)
	{
		GameObjectPtr go = gameObjects[ i ];
		RoboCat* cat = go->GetAsCat();
		if (cat && cat->GetPlayerId() == inPlayerId)
		{
			return std::static_pointer_cast< RoboCat >(go);
		}
	}

	return nullptr;
}
