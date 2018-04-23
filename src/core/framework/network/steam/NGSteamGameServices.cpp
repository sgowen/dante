//
//  NGSteamGameServices.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 6/11/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/steam/NGSteamGameServices.h>

#include <framework/util/StringUtil.h>
#include <framework/util/Constants.h>
#include <framework/util/macros.h>

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

NGSteamGameServices* NGSteamGameServices::s_instance = NULL;

void NGSteamGameServices::create(const char* inGameDir)
{
    assert(!s_instance);
    
    s_instance = new NGSteamGameServices(inGameDir);
}

void NGSteamGameServices::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = NULL;
}

NGSteamGameServices * NGSteamGameServices::getInstance()
{
    return s_instance;
}

void NGSteamGameServices::update()
{
    SteamGameServer_RunCallbacks();
    SteamAPI_RunCallbacks();
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
    _unServerIP = unServerAddress;
    _usServerPort = (uint16)nPort;
    
    _gameServerPing.retrieveSteamIDFromGameServer(this, _unServerIP, _usServerPort);
}

void NGSteamGameServices::initiateServerConnection(CSteamID steamIDGameServer)
{
    _steamIDGameServerToJoin = steamIDGameServer;
    
    _gameServerPing.cancelPing();
    
    _isRequestingToJoinServer = true;
}

CSteamID NGSteamGameServices::getServerToJoinSteamID()
{
    return _steamIDGameServerToJoin;
}

int NGSteamGameServices::getStatus()
{
    return _status;
}

bool NGSteamGameServices::isRequestingToJoinServer()
{
    return _isRequestingToJoinServer;
}

void NGSteamGameServices::onServerJoined()
{
    SteamFriends()->SetRichPresence("status", "In game");
    
    _isRequestingToJoinServer = false;
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
            _gameServers.push_back(NGSteamGameServer(pServer));
            _numServers++;
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
    
    _isRequestingServers = false;
}

std::vector<NGSteamGameServer>& NGSteamGameServices::getGameServers()
{
    return _gameServers;
}

bool NGSteamGameServices::isRequestingServers()
{
    return _isRequestingServers;
}

void NGSteamGameServices::refreshInternetServers()
{
    // If we are still finishing the previous refresh, then ignore this new request
    if (_isRequestingServers)
    {
        return;
    }
    
    // If another request is outstanding, make sure we release it properly
    if (_hServerListRequest)
    {
        SteamMatchmakingServers()->ReleaseRequest(_hServerListRequest);
        _hServerListRequest = NULL;
    }
    
    LOG("Refreshing internet servers");
    
    // Track that we are now in a refresh, what type of refresh, and reset our server count
    _isRequestingServers = true;
    _numServers = 0;
    _gameServers.clear();
    
    Steamworks_TestSecret();
    
    // Allocate some filters, there are some common pre-defined values that can be used:
    //
    // "gamedir" -- this is used to specify mods inside or a single product/appid
    // "secure" -- this is used to specify whether anti-cheat is enabled for a server
    // "gametype" -- this is used to specify game type and is set to whatever your game server code sets
    
    MatchMakingKeyValuePair_t pFilters[2];
    MatchMakingKeyValuePair_t *pFilter = pFilters;
    
    strncpy(pFilters[0].m_szKey, "gamedir", sizeof(pFilters[0].m_szKey));
    strncpy(pFilters[0].m_szValue, _gameDir, sizeof(pFilters[0].m_szValue));
    
    strncpy(pFilters[1].m_szKey, "secure", sizeof(pFilters[1].m_szKey));
    strncpy(pFilters[1].m_szValue, "1", sizeof(pFilters[1].m_szValue));
    
    // bugbug jmccaskey - passing just the appid without filters results in getting all servers rather than
    // servers filtered by appid alone.  So, we'll use the filters to filter the results better.
    _hServerListRequest = SteamMatchmakingServers()->RequestInternetServerList(SteamUtils()->GetAppID(), &pFilter, ARRAYSIZE(pFilters), this);
    
    SteamFriends()->SetRichPresence("status", "Finding an internet game");
}

void NGSteamGameServices::refreshLANServers()
{
    // If we are still finishing the previous refresh, then ignore this new request
    if (_isRequestingServers)
    {
        return;
    }
    
    // If another request is outstanding, make sure we release it properly
    if (_hServerListRequest)
    {
        SteamMatchmakingServers()->ReleaseRequest(_hServerListRequest);
        _hServerListRequest = NULL;
    }
    
    LOG("Refreshing LAN servers");
    
    // Track that we are now in a refresh, what type of refresh, and reset our server count
    _isRequestingServers = true;
    _numServers = 0;
    _gameServers.clear();
    
    // LAN refresh doesn't accept filters like internet above does
    _hServerListRequest = SteamMatchmakingServers()->RequestLANServerList(SteamUtils()->GetAppID(), this);
    
    SteamFriends()->SetRichPresence("status", "Finding a LAN game");
}

#pragma mark Steam Cloud

bool NGSteamGameServices::writeFileToSteamCloud(const char *inFileName, const char *inData)
{
    refreshSteamCloudFileStats();
    
    bool ret = false;
    int dataSize = (int) strlen(inData);
    if (dataSize < _ulAvailableBytes)
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

void NGSteamGameServices::refreshSteamCloudFileStats()
{
    _ulBytesQuota = 0;
    _ulAvailableBytes = 0;
    _nNumFilesInCloud = SteamRemoteStorage()->GetFileCount();
    SteamRemoteStorage()->GetQuota(&_ulBytesQuota, &_ulAvailableBytes);
    
    LOG("Quota: %llu bytes, %llu bytes remaining", _ulBytesQuota, _ulAvailableBytes);
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
    
    _status = STEAM_IPC_FAILURE;
}

void NGSteamGameServices::onSteamShutdown(SteamShutdown_t *callback)
{
    LOG("Steam shutdown request, shutting down");
    
    _status = STEAM_SHUTDOWN;
}

NGSteamGameServices::NGSteamGameServices(const char* inGameDir) :
_gameDir(inGameDir),
_numServers(0),
_isRequestingServers(false),
_hServerListRequest(NULL),
_unServerIP(0),
_usServerPort(0),
_steamIDGameServerToJoin(CSteamID()),
_nNumFilesInCloud(0),
_ulBytesQuota(0),
_ulAvailableBytes(0),
_status(STEAM_UNINITIALIZED),
_isRequestingToJoinServer(false)
{
    if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
    {
        // if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the
        // local Steam client and also launches this game again.
        
        // Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
        // removed steam_appid.txt from the game depot.
        _status = STEAM_INIT_FAIL_NOT_RUNNING;
        return;
    }
    
    // Init Steam CEG
    if (!Steamworks_InitCEGLibrary())
    {
        LOG("Steamworks_InitCEGLibrary() failed");
        alert("Warning", "Steam must be running to play this game (InitDrmLibrary() failed).");
        _status = STEAM_INIT_FAIL_DRM;
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
        alert("Warning", "Steam must be running to play this game (SteamAPI_Init() failed).");
        _status = STEAM_INIT_FAIL_API_INIT;
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
        alert("Warning", "Steam user must be logged in to play this game (SteamUser()->BLoggedOn() returned false).");
        _status = STEAM_INIT_FAIL_LOGGED_ON;
        return;
    }
    
    // do a DRM self check
    Steamworks_SelfCheck();
    
    if (!SteamController()->Init())
    {
        LOG("SteamController()->Init failed.");
        alert("Warning", "SteamController()->Init failed.");
        _status = STEAM_INIT_FAIL_CONTROLLER_INIT;
        return;
    }
    
    SteamFriends()->SetRichPresence("status", "Main menu");
    
    _status = STEAM_INIT_SUCCESS;
}

NGSteamGameServices::~NGSteamGameServices()
{
    if (_hServerListRequest)
    {
        SteamMatchmakingServers()->ReleaseRequest(_hServerListRequest);
        _hServerListRequest = NULL;
    }
    
    // Shutdown the SteamAPI
    SteamAPI_Shutdown();
    
    // Shutdown Steam CEG
    Steamworks_TermCEGLibrary();
}

NGSteamGameServices::GameServerPing::GameServerPing()
{
    _hGameServerQuery = HSERVERQUERY_INVALID;
    _client = NULL;
}

void NGSteamGameServices::GameServerPing::ServerResponded(gameserveritem_t &server)
{
    if (_hGameServerQuery != HSERVERQUERY_INVALID && server.m_steamID.IsValid())
    {
        _client->initiateServerConnection(server.m_steamID);
    }
    
    _hGameServerQuery = HSERVERQUERY_INVALID;
}

void NGSteamGameServices::GameServerPing::ServerFailedToRespond()
{
    _hGameServerQuery = HSERVERQUERY_INVALID;
}

void NGSteamGameServices::GameServerPing::retrieveSteamIDFromGameServer(NGSteamGameServices *client, uint32 unIP, uint16 unPort)
{
    _client = client;
    _hGameServerQuery = SteamMatchmakingServers()->PingServer(unIP, unPort, this);
}

void NGSteamGameServices::GameServerPing::cancelPing()
{
    _hGameServerQuery = HSERVERQUERY_INVALID;
}
