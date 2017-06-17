//
//  NGSteamClientHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NGSteamClientHelper.h"

#include "NGSteamAddress.h"

#include "NGSteamPacketHandler.h"

#include "NGSteamGameServices.h"

#include <assert.h>

NGSteamClientHelper::NGSteamClientHelper(ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc) : IClientHelper(new NGSteamPacketHandler(processPacketFunc, handleNoResponseFunc, handleConnectionResetFunc)), m_clientSteamID(nullptr), m_serverID(nullptr)
{
    assert(NG_STEAM_GAME_SERVICES->init() == STEAM_INIT_SUCCESS);
    
    m_clientSteamID = new NGSteamAddress(SteamUser()->GetSteamID());
    m_name = std::string(SteamFriends()->GetFriendPersonaName(m_clientSteamID->getSteamID()));
}

NGSteamClientHelper::~NGSteamClientHelper()
{
    if (m_clientSteamID)
    {
        delete m_clientSteamID;
    }
    
    if (m_serverID)
    {
        delete m_serverID;
    }
    
    NG_STEAM_GAME_SERVICES->deinit();
}

void NGSteamClientHelper::sendPacket(const OutputMemoryBitStream& inOutputStream)
{
    if (m_serverID)
    {
        INetworkHelper::sendPacket(inOutputStream, m_serverID);
    }
}

std::string& NGSteamClientHelper::getName()
{
    return m_name;
}
