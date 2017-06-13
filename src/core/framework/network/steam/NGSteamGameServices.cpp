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

#include "steam/steam_api.h"
#include "steam/isteamuserstats.h"
#include "steam/isteamremotestorage.h"
#include "steam/isteammatchmaking.h"
#include "steam/steam_gameserver.h"

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

NGSteamGameServices* NGSteamGameServices::getInstance()
{
    static NGSteamGameServices instance = NGSteamGameServices();
    return &instance;
}

int NGSteamGameServices::init()
{
    if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
    {
        // if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the
        // local Steam client and also launches this game again.
        
        // Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
        // removed steam_appid.txt from the game depot.
        m_iStatus = STEAM_INIT_FAIL_NOT_RUNNING;
        return m_iStatus;
    }
    
    // Init Steam CEG
    if (!Steamworks_InitCEGLibrary())
    {
        LOG("Steamworks_InitCEGLibrary() failed");
        alert("Fatal Error", "Steam must be running to play this game (InitDrmLibrary() failed).");
        m_iStatus = STEAM_INIT_FAIL_DRM;
        return m_iStatus;
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
        return m_iStatus;
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
        return m_iStatus;
    }
    
    // do a DRM self check
    Steamworks_SelfCheck();
    
    if (!SteamController()->Init())
    {
        LOG("SteamController()->Init failed.");
        alert("Fatal Error", "SteamController()->Init failed.");
        m_iStatus = STEAM_INIT_FAIL_CONTROLLER_INIT;
        return m_iStatus;
    }
    
    m_steamRemoteStorage = SteamRemoteStorage();
    
    refreshSteamCloudFileStats();
    
    m_iStatus = STEAM_INIT_SUCCESS;
    return m_iStatus;
}

void NGSteamGameServices::deinit()
{
    // Shutdown the SteamAPI
    SteamAPI_Shutdown();
    
    // Shutdown Steam CEG
    Steamworks_TermCEGLibrary();
    
    if (m_hServerListRequest)
    {
        SteamMatchmakingServers()->ReleaseRequest(m_hServerListRequest);
        m_hServerListRequest = nullptr;
    }
}

void NGSteamGameServices::refreshInternetServers()
{
    // If we are still finishing the previous refresh, then ignore this new request
    if (m_bRequestingServers)
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
    m_bRequestingServers = true;
    m_nServers = 0;
    m_ListGameServers.clear();
    
    Steamworks_TestSecret();
    
    // Allocate some filters, there are some common pre-defined values that can be used:
    //
    // "gamedir" -- this is used to specify mods inside or a single product/appid
    // "secure" -- this is used to specify whether anti-cheat is enabled for a server
    // "gametype" -- this is used to specify game type and is set to whatever your game server code sets
    
    MatchMakingKeyValuePair_t pFilters[2];
    MatchMakingKeyValuePair_t *pFilter = pFilters;
    
    strncpy(pFilters[0].m_szKey, "gamedir", sizeof(pFilters[0].m_szKey));
    strncpy(pFilters[0].m_szValue, "projectdante", sizeof(pFilters[0].m_szValue));
    
    strncpy(pFilters[1].m_szKey, "secure", sizeof(pFilters[1].m_szKey));
    strncpy(pFilters[1].m_szValue, "1", sizeof(pFilters[1].m_szValue));
    
    // bugbug jmccaskey - passing just the appid without filters results in getting all servers rather than
    // servers filtered by appid alone.  So, we'll use the filters to filter the results better.
    m_hServerListRequest = SteamMatchmakingServers()->RequestInternetServerList(SteamUtils()->GetAppID(), &pFilter, ARRAYSIZE(pFilters), this);
}

void NGSteamGameServices::refreshLANServers()
{
    // If we are still finishing the previous refresh, then ignore this new request
    if (m_bRequestingServers)
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
    m_bRequestingServers = true;
    m_nServers = 0;
    m_ListGameServers.clear();
    
    // LAN refresh doesn't accept filters like internet above does
    m_hServerListRequest = SteamMatchmakingServers()->RequestLANServerList(SteamUtils()->GetAppID(), this);
}

#pragma mark ISteamMatchmakingServerListResponse

void NGSteamGameServices::ServerResponded(HServerListRequest hReq, int iServer)
{
    gameserveritem_t *pServer = SteamMatchmakingServers()->GetServerDetails(hReq, iServer);
    if (pServer)
    {
        // Filter out servers that don't match our appid here (might get these in LAN calls since we can't put more filters on it)
        if (pServer->m_nAppID == SteamUtils()->GetAppID())
        {
            m_ListGameServers.push_back(NGSteamGameServer(pServer));
            m_nServers++;
        }
    }
}

void NGSteamGameServices::ServerFailedToRespond(HServerListRequest hReq, int iServer)
{
    // Unused
    
    UNUSED(hReq);
    UNUSED(iServer);
}

void NGSteamGameServices::RefreshComplete(HServerListRequest hReq, EMatchMakingServerResponse response)
{
    m_bRequestingServers = false;
}

#pragma mark Steam Cloud

bool NGSteamGameServices::writeFileToSteamCloud(const char *inFileName, const char *inData)
{
    bool ret = m_steamRemoteStorage->FileWrite(inFileName, inData, (int) strlen(inData));
    
    refreshSteamCloudFileStats();
    
    if (!ret)
    {
        LOG("NGSteamGameServices: Failed to write file!");
    }
    
    return ret;
}

std::string NGSteamGameServices::readFileFromSteamCloud(const char *inFileName)
{
    static char temp[4096];
    bzero(temp, 4096);
    
    int32 numBytesRead = 0;
    
    if (m_steamRemoteStorage->FileExists(inFileName))
    {
        int32 fileByteSize = m_steamRemoteStorage->GetFileSize(inFileName);
        if (fileByteSize >= sizeof(temp))
        {
            LOG("NGSteamGameServices: File was larger than expected, removing it...");
            
            char c = 0;
            m_steamRemoteStorage->FileWrite(inFileName, &c, 1);
        }
        else
        {
            numBytesRead = m_steamRemoteStorage->FileRead(inFileName, temp, sizeof(temp) - 1);
        }
    }
    
    temp[numBytesRead] = 0; // null-terminate
    
    return std::string(temp);
}

void NGSteamGameServices::refreshSteamCloudFileStats()
{
    m_ulBytesQuota = 0;
    m_ulAvailableBytes = 0;
    m_nNumFilesInCloud = m_steamRemoteStorage->GetFileCount();
    m_steamRemoteStorage->GetQuota(&m_ulBytesQuota, &m_ulAvailableBytes);
    
    LOG("Quota: %llu bytes, %llu bytes remaining", m_ulBytesQuota, m_ulAvailableBytes);
}

NGSteamGameServices::NGSteamGameServices() :
m_steamRemoteStorage(nullptr),
m_ulBytesQuota(0),
m_ulAvailableBytes(0),
m_nNumFilesInCloud(0),
m_nServers(0),
m_bRequestingServers(false),
m_hServerListRequest(nullptr),
m_iStatus(STEAM_INIT_NOT_CALLED)
{
    // Empty
}

NGSteamGameServices::~NGSteamGameServices()
{
    deinit();
}
