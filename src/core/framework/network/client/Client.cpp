//
//  Client.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Client.h"

#include "RoboCatClient.h"
#include "GameObjectRegistry.h"
#include "NetworkManagerClient.h"
#include "SocketAddressFactory.h"

Client* Client::getInstance()
{
    static Client client = Client();
    
    return &client;
}

void Client::init(std::string& serverIPAddress, std::string& userID)
{
    GameObjectRegistry::sInstance->RegisterCreationFunction('PLYR', RoboCatClient::StaticCreate);
    
    SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString(serverIPAddress);
    
    NetworkManagerClient::StaticInit(*serverAddress, userID);
}

Client::Client()
{
    // Empty
}
