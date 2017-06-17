//
//  NGSteamServerHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NGSteamServerHelper.h"

#include "NGSteamPacketHandler.h"
#include "NGSteamGameServices.h"
#include "StringUtil.h"
#include "FrameworkConstants.h"

#include <assert.h>

NGSteamServerHelper::NGSteamServerHelper(const char* inGameDir, const char* inVersionString, const char* inProductName, const char* inGameDescription, uint16 inPort, uint16 inAuthPort, uint16 inUpdaterPort, ProcessPacketFunc inProcessPacketFunc, HandleNoResponseFunc inHandleNoResponseFunc, HandleConnectionResetFunc inHandleConnectionResetFunc) : IServerHelper(new NGSteamPacketHandler(inProcessPacketFunc, inHandleNoResponseFunc, inHandleConnectionResetFunc)),
m_isConnectedToSteam(false)
{
    assert(NG_STEAM_GAME_SERVICES->init() == STEAM_INIT_SUCCESS);
    
    // Initialize the SteamGameServer interface, we tell it some info about us, and we request support
    // for both Authentication (making sure users own games) and secure mode, VAC running in our game
    // and kicking users who are VAC banned
    
    if (!SteamGameServer_Init(INADDR_ANY, inAuthPort, inPort, inUpdaterPort, eServerModeAuthenticationAndSecure, inVersionString))
    {
        LOG("SteamGameServer_Init call failed");
    }
    
    if (SteamGameServer())
    {
        // Set the "game dir".
        // This is currently required for all games.  However, soon we will be
        // using the AppID for most purposes, and this string will only be needed
        // for mods.  it may not be changed after the server has logged on
        SteamGameServer()->SetModDir(inGameDir);
        
        // These fields are currently required, but will go away soon.
        // See their documentation for more info
        SteamGameServer()->SetProduct(inProductName);
        SteamGameServer()->SetGameDescription(inGameDescription);
        
        // We don't support specators in our game.
        // .... but if we did:
        //SteamGameServer()->SetSpectatorPort( ... );
        //SteamGameServer()->SetSpectatorServerName( ... );
        
        // Initiate Anonymous logon.
        // Coming soon: Logging into authenticated, persistent game server account
        SteamGameServer()->LogOnAnonymous();
        
        // We want to actively update the master server with our presence so players can
        // find us via the steam matchmaking/server browser interfaces
        SteamGameServer()->EnableHeartbeats(true);
        
        LOG("SteamGameServer() interface is valid");
    }
    else
    {
        LOG("SteamGameServer() interface is invalid");
    }
}

NGSteamServerHelper::~NGSteamServerHelper()
{
    // Notify Steam master server we are going offline
    SteamGameServer()->EnableHeartbeats(false);
    
    // Disconnect from the steam servers
    SteamGameServer()->LogOff();
    
    // release our reference to the steam client library
    SteamGameServer_Shutdown();
    
    NG_STEAM_GAME_SERVICES->deinit();
}

void NGSteamServerHelper::processIncomingPackets()
{
    INetworkHelper::processIncomingPackets();
    
    // Run any Steam Game Server API callbacks
    SteamGameServer_RunCallbacks();
    
    // Update our server details
    sendUpdatedServerDetailsToSteam();
}

bool NGSteamServerHelper::isConnected()
{
    return m_isConnectedToSteam;
}

void NGSteamServerHelper::sendUpdatedServerDetailsToSteam()
{
    // Tell the Steam authentication servers about our game
    char rgchServerName[128];
    // If a client is running (should always be since we don't support a dedicated server)
    // then we'll form the name based off of it
    StringUtil::sprintf_safe( rgchServerName, "%s's game", SteamFriends()->GetPersonaName());
    m_serverName = std::string(rgchServerName);
    
    // These server state variables may be changed at any time. Note that there is no lnoger a mechanism
    // to send the player count. The player count is maintained by steam and you should use the player
    // creation/authentication functions to maintain your player count.
    SteamGameServer()->SetMaxPlayerCount(MAX_NUM_PLAYERS_PER_SERVER);
    SteamGameServer()->SetPasswordProtected(false); // TODO, will definitely have to revisit this
    SteamGameServer()->SetServerName(m_serverName.c_str());
    SteamGameServer()->SetBotPlayerCount(0); // optional, defaults to zero
    SteamGameServer()->SetMapName("Wasteland"); // TODO, do we need to specify a map name?
    
    // Update all the players names/scores
    for (uint32 i=0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        // TODO
//        if (m_rgClientData[i].m_bActive && m_rgpShips[i])
//        {
//            SteamGameServer()->BUpdateUserData( m_rgClientData[i].m_SteamIDUser, m_rgpShips[i]->GetPlayerName(), m_rguPlayerScores[i] );
//        }
    }
}
