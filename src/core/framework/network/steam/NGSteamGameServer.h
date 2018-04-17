//
//  NGSteamGameServer.h
//  noctisgames
//
//  Created by Stephen Gowen on 6/13/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamGameServer__
#define __noctisgames__NGSteamGameServer__

#include <framework/network/steam/NGSteam.h>

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
    uint32 _unIPAddress;			// IP address for the server
    int32 _nConnectionPort;		// Port for game clients to connect to for this server
    int _nPing;					// current ping time in milliseconds
    char _szMap[32];				// current map
    char _szGameDescription[64];	// game description
    int _nPlayers;					// current number of players on the server
    int _nMaxPlayers;				// Maximum players that can join this server
    int _nBotPlayers;				// Number of bots (i.e simulated players) on this server
    bool _bPassword;				// true if this server needs a password to join
    bool _bSecure;					// Is this server protected by VAC
    int	_nServerVersion;			// server version as reported to Steam
    char _szServerName[64];		// Game server name
    char _szServerString[128];		// String to show in server browser
    CSteamID _steamID;
};

#endif /* defined(__noctisgames__NGSteamGameServer__) */
