//
//  Client.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "RoboCatClientPCH.h"

bool Client::StaticInit()
{
	// Create the Client pointer first because it initializes SDL
	Client* client = new Client();

	InputManager::StaticInit();

	HUD::StaticInit();

	sInstance.reset(client);

	return true;
}

Client::Client()
{
	GameObjectRegistry::sInstance->RegisterCreationFunction('RCAT', RoboCatClient::StaticCreate);

//	SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString("208.97.168.138:9999");
    SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString("localhost:9999");

	NetworkManagerClient::StaticInit(*serverAddress, "Stephen");
}

void Client::DoFrame()
{
    Engine::DoFrame();
}
