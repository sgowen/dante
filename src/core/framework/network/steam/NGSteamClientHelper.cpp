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
#include "Timing.h"
#include "OutputMemoryBitStream.h"

#include <assert.h>

NGSteamClientHelper::NGSteamClientHelper(const char* inGameDir, bool isLAN, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc, NGSteamAddress* serverSteamAddress) : IClientHelper(new NGSteamPacketHandler(false, processPacketFunc, handleNoResponseFunc, handleConnectionResetFunc)),
m_connectionState(k_EClientNotConnected),
m_gameDir(inGameDir),
m_clientSteamAddress(nullptr),
m_serverSteamAddress(serverSteamAddress ? static_cast<NGSteamAddress*>(serverSteamAddress->createCopy()) : nullptr),
m_unServerIP(0),
m_usServerPort(0),
m_fLastNetworkDataReceivedTime(0.0f),
m_fLastConnectionAttemptRetryTime(0.0f),
m_iNumServers(0),
m_isRequestingServers(false),
m_hServerListRequest(nullptr)
{
    assert(NG_STEAM_GAME_SERVICES->init() == STEAM_INIT_SUCCESS);
    
    m_clientSteamAddress = new NGSteamAddress(SteamUser()->GetSteamID());
    m_name = std::string(SteamFriends()->GetFriendPersonaName(m_clientSteamAddress->getSteamID()));
    
    if (!m_serverSteamAddress)
    {
        if (isLAN)
        {
            refreshLANServers();
        }
        else
        {
            refreshInternetServers();
        }
    }
}

NGSteamClientHelper::~NGSteamClientHelper()
{
    if (m_clientSteamAddress)
    {
        delete m_clientSteamAddress;
    }
    
    if (m_serverSteamAddress)
    {
        delete m_serverSteamAddress;
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
    if (m_serverSteamAddress)
    {
        INetworkHelper::sendPacket(inOutputStream, m_serverSteamAddress);
    }
}

std::string& NGSteamClientHelper::getName()
{
    return m_name;
}

bool NGSteamClientHelper::isConnected()
{
    return m_connectionState == k_EClientConnectedAndAuthenticated;
}

void NGSteamClientHelper::parseCommandLine(const char *pchCmdLine, const char **ppchServerAddress)
{
    // Look for the +connect ipaddress:port parameter in the command line,
    // Steam will pass this when a user has used the Steam Server browser to find
    // a server for our game and is trying to join it.
    const char *pchConnectParam = "+connect";
    const char *pchConnect = strstr(pchCmdLine, pchConnectParam);
    *ppchServerAddress = NULL;
    if (pchConnect && strlen(pchCmdLine) > (pchConnect - pchCmdLine) + strlen(pchConnectParam) + 1)
    {
        // Address should be right after the +connect, +1 on the end to skip the space
        *ppchServerAddress = pchCmdLine + (pchConnect - pchCmdLine) + strlen(pchConnectParam) + 1;
    }
}

void NGSteamClientHelper::execCommandLineConnect(const char *pchServerAddress)
{
    if (pchServerAddress)
    {
        int32 octet0 = 0, octet1 = 0, octet2 = 0, octet3 = 0;
        int32 uPort = 0;
        int nConverted = sscanf(pchServerAddress, "%d.%d.%d.%d:%d", &octet0, &octet1, &octet2, &octet3, &uPort);
        if (nConverted == 5)
        {
            char rgchIPAddress[128];
            StringUtil::sprintf_safe(rgchIPAddress, "%d.%d.%d.%d", octet0, octet1, octet2, octet3);
            uint32 unIPAddress = (octet3) + (octet2 << 8) + (octet1 << 16) + (octet0 << 24);
            
            initiateServerConnection(unIPAddress, uPort);
        }
    }
}

#pragma mark ISteamMatchmakingServerListResponse

void NGSteamClientHelper::ServerResponded(HServerListRequest hReq, int iServer)
{
    LOG("ServerResponded");
    
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
    LOG("ServerFailedToRespond");
    
    UNUSED(hReq);
    UNUSED(iServer);
}

void NGSteamClientHelper::RefreshComplete(HServerListRequest hReq, EMatchMakingServerResponse response)
{
    LOG("RefreshComplete");
    
    m_isRequestingServers = false;
    
    NGSteamAddress* serverAddress = m_gameServers.size() > 0 ? new NGSteamAddress((*m_gameServers.begin()).getSteamID()) : nullptr;
    
    if (serverAddress)
    {
        if (m_serverSteamAddress)
        {
            delete m_serverSteamAddress;
        }
        
        m_serverSteamAddress = serverAddress;
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

//-----------------------------------------------------------------------------
// Purpose: Handle the server telling us it is exiting
//-----------------------------------------------------------------------------
void NGSteamClientHelper::onReceiveServerExiting()
{
    LOG("onReceiveServerExiting");
    
//    if (m_pP2PAuthedGame)
//    {
//        m_pP2PAuthedGame->EndGame();
//    }
//    
//    if (m_hAuthTicket != k_HAuthTicketInvalid)
//    {
//        SteamUser()->CancelAuthTicket(m_hAuthTicket);
//    }
//    
//    m_hAuthTicket = k_HAuthTicketInvalid;
//    
//    if (m_eGameState != k_EClientGameActive)
//    {
//        return;
//    }
//    
//    m_eConnectedStatus = k_EClientNotConnected;
//    
//    SetGameState(k_EClientGameConnectionFailure);
}

void NGSteamClientHelper::initiateServerConnection(uint32 unServerAddress, const int32 nPort)
{
    m_fLastNetworkDataReceivedTime = Timing::getInstance()->getFrameStartTime();
    m_fLastConnectionAttemptRetryTime = m_fLastNetworkDataReceivedTime;
    
    // ping the server to find out what it's steamID is
    m_unServerIP = unServerAddress;
    m_usServerPort = (uint16)nPort;
    
    m_gameServerPing.retrieveSteamIDFromGameServer(this, m_unServerIP, m_usServerPort);
}

void NGSteamClientHelper::initiateServerConnection(CSteamID steamIDGameServer)
{
    if (m_serverSteamAddress)
    {
        delete m_serverSteamAddress;
    }
    
    m_serverSteamAddress = new NGSteamAddress(steamIDGameServer);
    
    m_fLastNetworkDataReceivedTime = Timing::getInstance()->getFrameStartTime();
    m_fLastConnectionAttemptRetryTime = m_fLastNetworkDataReceivedTime;
    
    // TODO, move this logic into NetworkManagerClient so that it can retry sending the packet as needed
    OutputMemoryBitStream packet;
    packet.write(NETWORK_PACKET_TYPE_STEAM_CLIENT_INITIATE_CONNECTION);
    sendPacket(packet);
}

void NGSteamClientHelper::onGameJoinRequested(GameRichPresenceJoinRequested_t *pCallback)
{
    LOG("onGameJoinRequested");
    
    // parse out the connect
    const char *pchServerAddress;
    parseCommandLine(pCallback->m_rgchConnect, &pchServerAddress);

    execCommandLineConnect(pchServerAddress);
}

void NGSteamClientHelper::onSteamServersConnected(SteamServersConnected_t *callback)
{
    LOG("onSteamServersConnected");
}

void NGSteamClientHelper::onSteamServersDisconnected(SteamServersDisconnected_t *callback)
{
    LOG("onSteamServersDisconnected");
}

void NGSteamClientHelper::onSteamServerConnectFailure(SteamServerConnectFailure_t *callback)
{
    LOG("onSteamServerConnectFailure: %d", callback->m_eResult);
}

void NGSteamClientHelper::onGameOverlayActivated(GameOverlayActivated_t *callback)
{
    if (callback->m_bActive)
    {
        LOG("Steam overlay now active");
    }
    else
    {
        LOG("Steam overlay now inactive");
    }
}

void NGSteamClientHelper::onP2PSessionConnectFail(P2PSessionConnectFail_t *pCallback)
{
    if (pCallback->m_steamIDRemote == m_serverSteamAddress->getSteamID())
    {
        // failed, error out
        LOG("Failed to make P2P connection, quiting server");
        onReceiveServerExiting();
    }
}

void NGSteamClientHelper::onIPCFailure(IPCFailure_t *failure)
{
    static bool bExiting = false;
    if (!bExiting)
    {
        LOG("Steam IPC Failure, shutting down");
        
        // TODO, exit back to main menu
        
        bExiting = true;
    }
}

void NGSteamClientHelper::onSteamShutdown(SteamShutdown_t *callback)
{
    static bool bExiting = false;
    if (!bExiting)
    {
        LOG("Steam shutdown request, shutting down");
        
        // TODO, exit back to main menu
        
        bExiting = true;
    }
}

NGSteamClientHelper::GameServerPing::GameServerPing()
{
    m_hGameServerQuery = HSERVERQUERY_INVALID;
    m_client = nullptr;
}

void NGSteamClientHelper::GameServerPing::ServerResponded(gameserveritem_t &server)
{
    if (m_hGameServerQuery != HSERVERQUERY_INVALID && server.m_steamID.IsValid())
    {
        m_client->initiateServerConnection(server.m_steamID);
    }
    
    m_hGameServerQuery = HSERVERQUERY_INVALID;
}

void NGSteamClientHelper::GameServerPing::ServerFailedToRespond()
{
    m_hGameServerQuery = HSERVERQUERY_INVALID;
}

void NGSteamClientHelper::GameServerPing::retrieveSteamIDFromGameServer(NGSteamClientHelper *client, uint32 unIP, uint16 unPort)
{
    m_client = client;
    m_hGameServerQuery = SteamMatchmakingServers()->PingServer(unIP, unPort, this);
}

void NGSteamClientHelper::GameServerPing::cancelPing()
{
    m_hGameServerQuery = HSERVERQUERY_INVALID;
}
