//
//  NGSteamGameServerPing.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/13/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NGSteamGameServerPing.h"

#include "INGSteamClient.h"

NGSteamGameServerPing::NGSteamGameServerPing() :
m_hGameServerQuery(HSERVERQUERY_INVALID),
m_client(nullptr)
{
    // Empty
}

void NGSteamGameServerPing::retrieveSteamIDFromGameServer(INGSteamClient* client, uint32 unIP, uint16 unPort)
{
    m_client = client;
    m_hGameServerQuery = SteamMatchmakingServers()->PingServer(unIP, unPort, this);
}

void NGSteamGameServerPing::cancelPing()
{
    m_hGameServerQuery = HSERVERQUERY_INVALID;
}

void NGSteamGameServerPing::ServerResponded(gameserveritem_t &server)
{
    if (m_hGameServerQuery != HSERVERQUERY_INVALID && server.m_steamID.IsValid())
    {
        m_client->initiateServerConnection(server.m_steamID);
    }
    
    m_hGameServerQuery = HSERVERQUERY_INVALID;
}

void NGSteamGameServerPing::ServerFailedToRespond()
{
    m_hGameServerQuery = HSERVERQUERY_INVALID;
}
