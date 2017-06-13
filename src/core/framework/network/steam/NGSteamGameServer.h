//
//  NGSteamGameServer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/13/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamGameServer__
#define __noctisgames__NGSteamGameServer__

#include "NGSteam.h"

class NGSteamGameServer
{
public:
    NGSteamGameServer(gameserveritem_t *pGameServerItem);
    
    const char* getName();
    
    const char* getDisplayString();
    
    uint32 getIP();
    
    int32 getPort();
    
    CSteamID getSteamID();
    
private:
    uint32 m_unIPAddress;			// IP address for the server
    int32 m_nConnectionPort;		// Port for game clients to connect to for this server
    int m_nPing;					// current ping time in milliseconds
    char m_szMap[32];				// current map
    char m_szGameDescription[64];	// game description
    int m_nPlayers;					// current number of players on the server
    int m_nMaxPlayers;				// Maximum players that can join this server
    int m_nBotPlayers;				// Number of bots (i.e simulated players) on this server
    bool m_bPassword;				// true if this server needs a password to join
    bool m_bSecure;					// Is this server protected by VAC
    int	m_nServerVersion;			// server version as reported to Steam
    char m_szServerName[64];		// Game server name
    char m_szServerString[128];		// String to show in server browser
    CSteamID m_steamID;
};

#endif /* defined(__noctisgames__NGSteamGameServer__) */
