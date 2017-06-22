//
//  NGSteamGameServer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/13/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NGSteamGameServer.h"

#include "StringUtil.h"
#include "macros.h"

NGSteamGameServer::NGSteamGameServer(gameserveritem_t *pGameServerItem)
{
    m_unIPAddress = pGameServerItem->m_NetAdr.GetIP();
    m_nConnectionPort = pGameServerItem->m_NetAdr.GetConnectionPort();
    m_nPing = pGameServerItem->m_nPing;
    
    strncpy(m_szMap, pGameServerItem->m_szMap, ARRAYSIZE(m_szMap));
    strncpy(m_szGameDescription, pGameServerItem->m_szGameDescription, ARRAYSIZE(m_szGameDescription));
    
    m_nPlayers = pGameServerItem->m_nPlayers;
    m_nMaxPlayers = pGameServerItem->m_nMaxPlayers;
    m_nBotPlayers = pGameServerItem->m_nBotPlayers;
    m_bPassword = pGameServerItem->m_bPassword;
    m_bSecure = pGameServerItem->m_bSecure;
    m_nServerVersion = pGameServerItem->m_nServerVersion;
    
    strncpy(m_szServerName, pGameServerItem->GetName(), ARRAYSIZE(m_szServerName));
    
    StringUtil::sprintf_safe(m_szServerString, "%s (%i/%i) at %s ping(%d)", pGameServerItem->GetName(), pGameServerItem->m_nPlayers, pGameServerItem->m_nMaxPlayers, pGameServerItem->m_NetAdr.GetConnectionAddressString(), pGameServerItem->m_nPing);
    
    m_steamID = pGameServerItem->m_steamID;
}

const char* NGSteamGameServer::getName()
{
    return m_szServerName;
}

const char* NGSteamGameServer::getDisplayString()
{
    return m_szServerString;
}

uint32 NGSteamGameServer::getIP()
{
    return m_unIPAddress;
}

int32 NGSteamGameServer::getPort()
{
    return m_nConnectionPort;
}

CSteamID NGSteamGameServer::getSteamID()
{
    return m_steamID;
}
