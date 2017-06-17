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
#include "macros.h"
#include "StringUtil.h"

#include <assert.h>

NGSteamClientHelper::NGSteamClientHelper(const char* inGameDir, bool isLAN, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc) : IClientHelper(new NGSteamPacketHandler(processPacketFunc, handleNoResponseFunc, handleConnectionResetFunc)),
m_gameDir(inGameDir),
m_clientSteamAddress(nullptr),
m_serverAddress(nullptr),
m_iNumServers(0),
m_isRequestingServers(false),
m_hServerListRequest(nullptr)
{
    assert(NG_STEAM_GAME_SERVICES->init() == STEAM_INIT_SUCCESS);
    
    m_clientSteamAddress = new NGSteamAddress(SteamUser()->GetSteamID());
    m_name = std::string(SteamFriends()->GetFriendPersonaName(m_clientSteamAddress->getSteamID()));
    
    if (isLAN)
    {
        refreshLANServers();
    }
    else
    {
        refreshInternetServers();
    }
}

NGSteamClientHelper::~NGSteamClientHelper()
{
    if (m_clientSteamAddress)
    {
        delete m_clientSteamAddress;
    }
    
    if (m_serverAddress)
    {
        delete m_serverAddress;
    }
    
    if (m_hServerListRequest)
    {
        SteamMatchmakingServers()->ReleaseRequest(m_hServerListRequest);
        m_hServerListRequest = nullptr;
    }
    
    NG_STEAM_GAME_SERVICES->deinit();
}

void NGSteamClientHelper::processIncomingPackets()
{
    INetworkHelper::processIncomingPackets();
    
    // without this, callbacks will never fire
    SteamAPI_RunCallbacks();
}

void NGSteamClientHelper::sendPacket(const OutputMemoryBitStream& inOutputStream)
{
    if (m_serverAddress)
    {
        INetworkHelper::sendPacket(inOutputStream, m_serverAddress);
    }
}

std::string& NGSteamClientHelper::getName()
{
    return m_name;
}

#pragma mark ISteamMatchmakingServerListResponse

void NGSteamClientHelper::ServerResponded(HServerListRequest hReq, int iServer)
{
    gameserveritem_t *pServer = SteamMatchmakingServers()->GetServerDetails(hReq, iServer);
    if (pServer)
    {
        // Filter out servers that don't match our appid here (might get these in LAN calls since we can't put more filters on it)
        if (pServer->m_nAppID == SteamUtils()->GetAppID())
        {
            m_gameServers.push_back(NGSteamGameServer(pServer));
            m_iNumServers++;
        }
    }
}

void NGSteamClientHelper::ServerFailedToRespond(HServerListRequest hReq, int iServer)
{
    UNUSED(hReq);
    UNUSED(iServer);
}

void NGSteamClientHelper::RefreshComplete(HServerListRequest hReq, EMatchMakingServerResponse response)
{
    m_isRequestingServers = false;
    
    NGSteamAddress* serverAddress = m_gameServers.size() > 0 ? new NGSteamAddress((*m_gameServers.begin()).getSteamID()) : nullptr;
    
    if (serverAddress)
    {
        if (m_serverAddress)
        {
            delete m_serverAddress;
        }
        
        m_serverAddress = serverAddress;
    }
}

std::list<NGSteamGameServer>& NGSteamClientHelper::getGameServers()
{
    return m_gameServers;
}

bool NGSteamClientHelper::isRequestingServers()
{
    return m_isRequestingServers;
}

void NGSteamClientHelper::refreshInternetServers()
{
    // If we are still finishing the previous refresh, then ignore this new request
    if (m_isRequestingServers)
    {
        return;
    }
    
    // If another request is outstanding, make sure we release it properly
    if (m_hServerListRequest)
    {
        SteamMatchmakingServers()->ReleaseRequest(m_hServerListRequest);
        m_hServerListRequest = nullptr;
    }
    
    LOG("Refreshing internet servers");
    
    // Track that we are now in a refresh, what type of refresh, and reset our server count
    m_isRequestingServers = true;
    m_iNumServers = 0;
    m_gameServers.clear();
    
    Steamworks_TestSecret();
    
    // Allocate some filters, there are some common pre-defined values that can be used:
    //
    // "gamedir" -- this is used to specify mods inside or a single product/appid
    // "secure" -- this is used to specify whether anti-cheat is enabled for a server
    // "gametype" -- this is used to specify game type and is set to whatever your game server code sets
    
    MatchMakingKeyValuePair_t pFilters[2];
    MatchMakingKeyValuePair_t *pFilter = pFilters;
    
    strncpy(pFilters[0].m_szKey, "gamedir", sizeof(pFilters[0].m_szKey));
    strncpy(pFilters[0].m_szValue, m_gameDir, sizeof(pFilters[0].m_szValue));
    
    strncpy(pFilters[1].m_szKey, "secure", sizeof(pFilters[1].m_szKey));
    strncpy(pFilters[1].m_szValue, "1", sizeof(pFilters[1].m_szValue));
    
    // bugbug jmccaskey - passing just the appid without filters results in getting all servers rather than
    // servers filtered by appid alone.  So, we'll use the filters to filter the results better.
    m_hServerListRequest = SteamMatchmakingServers()->RequestInternetServerList(SteamUtils()->GetAppID(), &pFilter, ARRAYSIZE(pFilters), this);
}

void NGSteamClientHelper::refreshLANServers()
{
    // If we are still finishing the previous refresh, then ignore this new request
    if (m_isRequestingServers)
    {
        return;
    }
    
    // If another request is outstanding, make sure we release it properly
    if (m_hServerListRequest)
    {
        SteamMatchmakingServers()->ReleaseRequest(m_hServerListRequest);
        m_hServerListRequest = nullptr;
    }
    
    LOG("Refreshing LAN servers");
    
    // Track that we are now in a refresh, what type of refresh, and reset our server count
    m_isRequestingServers = true;
    m_iNumServers = 0;
    m_gameServers.clear();
    
    // LAN refresh doesn't accept filters like internet above does
    m_hServerListRequest = SteamMatchmakingServers()->RequestLANServerList(SteamUtils()->GetAppID(), this);
}
