//
//  SteamGameServices.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/11/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SteamGameServices.h"

#include "StringUtil.h"

#include "steam/steam_api.h"
#include "steam/isteamuserstats.h"
#include "steam/isteamremotestorage.h"
#include "steam/isteammatchmaking.h"
#include "steam/steam_gameserver.h"

//-----------------------------------------------------------------------------
// Purpose: Helper to display critical errors
//-----------------------------------------------------------------------------
int alert(const char *lpCaption, const char *lpText)
{
#ifndef _WIN32
    fprintf( stderr, "Message: '%s', Detail: '%s'\n", lpCaption, lpText );
    return 0;
#else
    return ::MessageBox( NULL, lpText, lpCaption, MB_OK );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: callback hook for debug text emitted from the Steam API
//-----------------------------------------------------------------------------
extern "C" void __cdecl SteamAPIDebugTextHook( int nSeverity, const char *pchDebugText )
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

SteamGameServices* SteamGameServices::getInstance()
{
    static SteamGameServices instance = SteamGameServices();
    return &instance;
}

bool SteamGameServices::init()
{
    if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
    {
        // if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the
        // local Steam client and also launches this game again.
        
        // Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
        // removed steam_appid.txt from the game depot.
        return false;
    }
    
    // Init Steam CEG
    if (!Steamworks_InitCEGLibrary())
    {
        LOG("Steamworks_InitCEGLibrary() failed\n");
        alert("Fatal Error", "Steam must be running to play this game (InitDrmLibrary() failed).\n");
        return false;
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
        LOG("SteamAPI_Init() failed\n");
        alert("Fatal Error", "Steam must be running to play this game (SteamAPI_Init() failed).\n");
        return false;
    }
    
    // set our debug handler
    SteamClient()->SetWarningMessageHook(&SteamAPIDebugTextHook);
    
    // Ensure that the user has logged into Steam. This will always return true if the game is launched
    // from Steam, but if Steam is at the login prompt when you run your game from the debugger, it
    // will return false.
    if (!SteamUser()->BLoggedOn())
    {
        LOG("Steam user is not logged in\n");
        alert("Fatal Error", "Steam user must be logged in to play this game (SteamUser()->BLoggedOn() returned false).\n");
        return false;
    }
    
    // do a DRM self check
    Steamworks_SelfCheck();
    
    if (!SteamController()->Init())
    {
        LOG("SteamController()->Init failed.\n");
        alert("Fatal Error", "SteamController()->Init failed.\n");
        return false;
    }
    
    return true;
}

void SteamGameServices::deinit()
{
    // Shutdown the SteamAPI
    SteamAPI_Shutdown();
    
    // Shutdown Steam CEG
    Steamworks_TermCEGLibrary();
}

SteamGameServices::SteamGameServices()
{
    // Empty
}

SteamGameServices::~SteamGameServices()
{
    // Empty
}
