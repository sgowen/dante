//
//  NGSteam.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/13/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteam__
#define __noctisgames__NGSteam__

#ifdef WIN32

typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

#elif defined(POSIX)

typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef uint32 DWORD;
typedef DWORD HWND;
typedef DWORD HINSTANCE;
typedef short SHORT;
typedef long LONG;
typedef unsigned char byte;
typedef unsigned char uint8;

#endif

#include "steam/steam_api.h"
#include "steam/steamtypes.h"
#include "steam/isteamuserstats.h"
#include "steam/isteamremotestorage.h"
#include "steam/isteammatchmaking.h"
#include "steam/steam_gameserver.h"

#ifdef STEAM_CEG
// Steam DRM header file
#include "cegclient.h"
#else
#define Steamworks_InitCEGLibrary() (true)
#define Steamworks_TermCEGLibrary()
#define Steamworks_TestSecret()
#define Steamworks_SelfCheck()
#endif

// Steam Auth Network message types
enum EMessage
{
    // Server messages
    k_EMsgServerBegin = 0,
    k_EMsgServerSendInfo = k_EMsgServerBegin+1,
    k_EMsgServerFailAuthentication = k_EMsgServerBegin+2,
    k_EMsgServerPassAuthentication = k_EMsgServerBegin+3,
    k_EMsgServerExiting = k_EMsgServerBegin+4,
    
    // Client messages
    k_EMsgClientBegin = 500,
    k_EMsgClientInitiateConnection = k_EMsgClientBegin+1, // Msg from client to server when trying to connect
    k_EMsgClientBeginAuthentication = k_EMsgClientBegin+2,
    k_EMsgClientLeavingServer = k_EMsgClientBegin+3,
    
    // P2P authentication messages
    k_EMsgP2PBegin = 600,
    k_EMsgP2PSendingTicket = k_EMsgP2PBegin+1,
    
    // force 32-bit size enum so the wire protocol doesn't get outgrown later
    k_EForceDWORD  = 0x7fffffff, 
};

// UDP port for the server to listen on
#define STEAM_SERVER_PORT 27015

// UDP port for the master server updater to listen on
#define STEAM_MASTER_SERVER_UPDATER_PORT 27016

#endif /* defined(__noctisgames__NGSteam__) */
