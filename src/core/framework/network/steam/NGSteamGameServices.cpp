//
//  NGSteamGameServices.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/11/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NGSteamGameServices.h"

#include "StringUtil.h"
#include "FrameworkConstants.h"
#include "macros.h"

#include <assert.h>

void alert(const char *lpCaption, const char *lpText)
{
	LOG("Message: %s, Detail: %s", lpCaption, lpText);
}

//-----------------------------------------------------------------------------
// Purpose: callback hook for debug text emitted from the Steam API
//-----------------------------------------------------------------------------
extern "C" void __cdecl steamAPIDebugTextHook(int nSeverity, const char *pchDebugText)
{
    // if you're running in the debugger, only warnings (nSeverity >= 1) will be sent
    // if you add -debug_steamapi to the command-line, a lot of extra informational messages will also be sent
    LOG(pchDebugText);
    
    if (nSeverity >= 1)
    {
        // place to set a breakpoint for catching API errors
        int x = 3;
        x = x;
    }
}

NGSteamGameServices* NGSteamGameServices::s_instance = nullptr;

void NGSteamGameServices::create(const char* inGameDir)
{
    assert(!s_instance);
    
    s_instance = new NGSteamGameServices(inGameDir);
}

void NGSteamGameServices::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = nullptr;
}

NGSteamGameServices * NGSteamGameServices::getInstance()
{
    return s_instance;
}

void NGSteamGameServices::update(bool isServer)
{
    SteamAPI_RunCallbacks();
    
    if (isServer)
    {
        SteamGameServer_RunCallbacks();
    }
}

void NGSteamGameServices::parseCommandLine(const char *pchCmdLine, const char **ppchServerAddress)
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

void NGSteamGameServices::connectToServerWithAddress(const char *pchServerAddress)
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

void NGSteamGameServices::initiateServerConnection(uint32 unServerAddress, const int32 nPort)
{
    // ping the server to find out what it's steamID is
    m_unServerIP = unServerAddress;
    m_usServerPort = (uint16)nPort;
    
    m_gameServerPing.retrieveSteamIDFromGameServer(this, m_unServerIP, m_usServerPort);
}

void NGSteamGameServices::initiateServerConnection(CSteamID steamIDGameServer)
{
    m_steamIDGameServerToJoin = steamIDGameServer;
    
    m_gameServerPing.cancelPing();
    
    m_isRequestingToJoinServer = true;
}

CSteamID NGSteamGameServices::getServerToJoinSteamID()
{
    return m_steamIDGameServerToJoin;
}

int NGSteamGameServices::getStatus()
{
    return m_iStatus;
}

bool NGSteamGameServices::isRequestingToJoinServer()
{
    return m_isRequestingToJoinServer;
}

void NGSteamGameServices::onServerJoined()
{
    SteamFriends()->SetRichPresence("status", "In game");
    
    m_isRequestingToJoinServer = false;
}

#pragma mark ISteamMatchmakingServerListResponse

void NGSteamGameServices::ServerResponded(HServerListRequest hReq, int iServer)
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

void NGSteamGameServices::ServerFailedToRespond(HServerListRequest hReq, int iServer)
{
    LOG("ServerFailedToRespond");
    
    UNUSED(hReq);
    UNUSED(iServer);
}

void NGSteamGameServices::RefreshComplete(HServerListRequest hReq, EMatchMakingServerResponse response)
{
    LOG("RefreshComplete");
    
    m_isRequestingServers = false;
}

std::vector<NGSteamGameServer>& NGSteamGameServices::getGameServers()
{
    return m_gameServers;
}

bool NGSteamGameServices::isRequestingServers()
{
    return m_isRequestingServers;
}

void NGSteamGameServices::refreshInternetServers()
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
    
    SteamFriends()->SetRichPresence("status", "Finding an internet game");
}

void NGSteamGameServices::refreshLANServers()
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
    
    SteamFriends()->SetRichPresence("status", "Finding a LAN game");
}

#pragma mark Steam Cloud

bool NGSteamGameServices::writeFileToSteamCloud(const char *inFileName, const char *inData)
{
    refreshSteamCloudFileStats();
    
    bool ret = false;
    int dataSize = (int) strlen(inData);
    if (dataSize < m_ulAvailableBytes)
    {
        ret = SteamRemoteStorage()->FileWrite(inFileName, inData, dataSize);
    }
    
    if (ret)
    {
        LOG("NGSteamGameServices: File written successfully to Steam Remote Storage");
    }
    else
    {
        LOG("NGSteamGameServices: Failed to write file!");
    }
    
    return ret;
}

std::string NGSteamGameServices::readFileFromSteamCloud(const char *inFileName)
{
	char temp[4096];
    
    int32 numBytesRead = 0;
    
    if (SteamRemoteStorage()->FileExists(inFileName))
    {
        int32 fileByteSize = SteamRemoteStorage()->GetFileSize(inFileName);
        if (fileByteSize >= sizeof(temp))
        {
            LOG("NGSteamGameServices: File was larger than expected, removing it...");
            
            char c = 0;
            SteamRemoteStorage()->FileWrite(inFileName, &c, 1);
        }
        else
        {
            numBytesRead = SteamRemoteStorage()->FileRead(inFileName, temp, sizeof(temp) - 1);
        }
    }
    
    temp[numBytesRead] = 0; // null-terminate
    
    return std::string(temp);
}

#pragma mark General Player Functions

uint64_t NGSteamGameServices::getLocalPlayerId()
{
    CSteamID myID = SteamUser()->GetSteamID();
    return myID.ConvertToUint64();
}

std::string NGSteamGameServices::getLocalPlayerName()
{
    return std::string(SteamFriends()->GetPersonaName());
}

std::string NGSteamGameServices::getRemotePlayerName(uint64_t inPlayerId)
{
    return std::string(SteamFriends()->GetFriendPersonaName(inPlayerId));
}

void NGSteamGameServices::refreshSteamCloudFileStats()
{
    m_ulBytesQuota = 0;
    m_ulAvailableBytes = 0;
    m_nNumFilesInCloud = SteamRemoteStorage()->GetFileCount();
    SteamRemoteStorage()->GetQuota(&m_ulBytesQuota, &m_ulAvailableBytes);
    
    LOG("Quota: %llu bytes, %llu bytes remaining", m_ulBytesQuota, m_ulAvailableBytes);
}

#pragma mark - STEAM_CALLBACK

void NGSteamGameServices::onGameOverlayActivated(GameOverlayActivated_t *callback)
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

void NGSteamGameServices::onGameJoinRequested(GameRichPresenceJoinRequested_t *pCallback)
{
    LOG("onGameJoinRequested");
    
    // parse out the connect
    const char *pchServerAddress;
    parseCommandLine(pCallback->m_rgchConnect, &pchServerAddress);
    
    connectToServerWithAddress(pchServerAddress);
}

void NGSteamGameServices::onIPCFailure(IPCFailure_t *failure)
{
    LOG("Steam IPC Failure, shutting down");
    
    m_iStatus = STEAM_IPC_FAILURE;
}

void NGSteamGameServices::onSteamShutdown(SteamShutdown_t *callback)
{
    LOG("Steam shutdown request, shutting down");
    
    m_iStatus = STEAM_SHUTDOWN;
}

NGSteamGameServices::NGSteamGameServices(const char* inGameDir) :
m_gameDir(inGameDir),
m_iNumServers(0),
m_isRequestingServers(false),
m_hServerListRequest(nullptr),
m_unServerIP(0),
m_usServerPort(0),
m_steamIDGameServerToJoin(CSteamID()),
m_nNumFilesInCloud(0),
m_ulBytesQuota(0),
m_ulAvailableBytes(0),
m_iStatus(STEAM_UNINITIALIZED),
m_isRequestingToJoinServer(false)
{
    if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
    {
        // if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the
        // local Steam client and also launches this game again.
        
        // Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
        // removed steam_appid.txt from the game depot.
        m_iStatus = STEAM_INIT_FAIL_NOT_RUNNING;
        return;
    }
    
    // Init Steam CEG
    if (!Steamworks_InitCEGLibrary())
    {
        LOG("Steamworks_InitCEGLibrary() failed");
        alert("Fatal Error", "Steam must be running to play this game (InitDrmLibrary() failed).");
        m_iStatus = STEAM_INIT_FAIL_DRM;
        return;
    }
    
    // Initialize SteamAPI, if this fails we bail out since we depend on Steam for lots of stuff.
    // You don't necessarily have to though if you write your code to check whether all the Steam
    // interfaces are NULL before using them and provide alternate paths when they are unavailable.
    //
    // This will also load the in-game steam overlay dll into your process.  That dll is normally
    // injected by steam when it launches games, but by calling this you cause it to always load,
    // even when not launched via steam.
    if (!SteamAPI_Init())
    {
        LOG("SteamAPI_Init() failed");
        alert("Fatal Error", "Steam must be running to play this game (SteamAPI_Init() failed).");
        m_iStatus = STEAM_INIT_FAIL_API_INIT;
        return;
    }
    
    // set our debug handler
    SteamClient()->SetWarningMessageHook(&steamAPIDebugTextHook);
    
    // Ensure that the user has logged into Steam. This will always return true if the game is launched
    // from Steam, but if Steam is at the login prompt when you run your game from the debugger, it
    // will return false.
    if (!SteamUser()->BLoggedOn())
    {
        LOG("Steam user is not logged in");
        alert("Fatal Error", "Steam user must be logged in to play this game (SteamUser()->BLoggedOn() returned false).");
        m_iStatus = STEAM_INIT_FAIL_LOGGED_ON;
        return;
    }
    
    // do a DRM self check
    Steamworks_SelfCheck();
    
    if (!SteamController()->Init())
    {
        LOG("SteamController()->Init failed.");
        alert("Fatal Error", "SteamController()->Init failed.");
        m_iStatus = STEAM_INIT_FAIL_CONTROLLER_INIT;
        return;
    }
    
    SteamFriends()->SetRichPresence("status", "Main menu");
    
    m_iStatus = STEAM_INIT_SUCCESS;
}

NGSteamGameServices::~NGSteamGameServices()
{
    if (m_hServerListRequest)
    {
        SteamMatchmakingServers()->ReleaseRequest(m_hServerListRequest);
        m_hServerListRequest = nullptr;
    }
    
    // Shutdown the SteamAPI
    SteamAPI_Shutdown();
    
    // Shutdown Steam CEG
    Steamworks_TermCEGLibrary();
}

NGSteamGameServices::GameServerPing::GameServerPing()
{
    m_hGameServerQuery = HSERVERQUERY_INVALID;
    m_client = nullptr;
}

void NGSteamGameServices::GameServerPing::ServerResponded(gameserveritem_t &server)
{
    if (m_hGameServerQuery != HSERVERQUERY_INVALID && server.m_steamID.IsValid())
    {
        m_client->initiateServerConnection(server.m_steamID);
    }
    
    m_hGameServerQuery = HSERVERQUERY_INVALID;
}

void NGSteamGameServices::GameServerPing::ServerFailedToRespond()
{
    m_hGameServerQuery = HSERVERQUERY_INVALID;
}

void NGSteamGameServices::GameServerPing::retrieveSteamIDFromGameServer(NGSteamGameServices *client, uint32 unIP, uint16 unPort)
{
    m_client = client;
    m_hGameServerQuery = SteamMatchmakingServers()->PingServer(unIP, unPort, this);
}

void NGSteamGameServices::GameServerPing::cancelPing()
{
    m_hGameServerQuery = HSERVERQUERY_INVALID;
}
