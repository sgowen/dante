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
#include "macros.h"
#include "StringUtil.h"
#include "Timing.h"
#include "OutputMemoryBitStream.h"

NGSteamClientHelper::NGSteamClientHelper(CSteamID serverSteamID, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc) : IClientHelper(new NGSteamPacketHandler(false, processPacketFunc, handleNoResponseFunc, handleConnectionResetFunc)),
m_serverSteamAddress(new NGSteamAddress(serverSteamID)),
m_clientSteamAddress(new NGSteamAddress(SteamUser()->GetSteamID()))
{
    m_name = std::string(SteamFriends()->GetFriendPersonaName(m_clientSteamAddress->getSteamID()));
}

NGSteamClientHelper::~NGSteamClientHelper()
{
    delete m_serverSteamAddress;
    delete m_clientSteamAddress;
}

void NGSteamClientHelper::sendPacket(const OutputMemoryBitStream& inOutputStream)
{
    INetworkHelper::sendPacket(inOutputStream, m_serverSteamAddress);
}

std::string& NGSteamClientHelper::getName()
{
    return m_name;
}
