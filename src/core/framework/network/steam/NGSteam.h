//
//  NGSteam.h
//  noctisgames
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

#include <stddef.h>

#include <steam/steam_api.h>
#include <steam/steamtypes.h>
#include <steam/isteamuserstats.h>
#include <steam/isteamremotestorage.h>
#include <steam/isteammatchmaking.h>
#include <steam/steam_gameserver.h>

#ifdef STEAM_CEG
// Steam DRM header file
#include <cegclient.h>
#else
#define Steamworks_InitCEGLibrary() (true)
#define Steamworks_TermCEGLibrary()
#define Steamworks_TestSecret()
#define Steamworks_SelfCheck()
#endif

#include <framework/util/Constants.h>

// Steam Auth Network message types
enum EMessage
{
    // Server messages
    k_EMsgServerBegin = NW_PACKET_TYPE_COUNT+1,
    k_EMsgServerSendInfo = NW_PACKET_TYPE_COUNT+2,
    k_EMsgServerFailAuthentication = NW_PACKET_TYPE_COUNT+3,
    k_EMsgServerPassAuthentication = NW_PACKET_TYPE_COUNT+4,
    k_EMsgServerExiting = NW_PACKET_TYPE_COUNT+5,

    // Client messages
    k_EMsgClientBegin = NW_PACKET_TYPE_COUNT+6,
    k_EMsgClientInitiateConnection = NW_PACKET_TYPE_COUNT+7, // Msg from client to server when trying to connect
    k_EMsgClientBeginAuthentication = NW_PACKET_TYPE_COUNT+8,
    k_EMsgClientLeavingServer = NW_PACKET_TYPE_COUNT+9,

    // P2P authentication messages
    k_EMsgP2PBegin = NW_PACKET_TYPE_COUNT+10,
    k_EMsgP2PSendingTicket = NW_PACKET_TYPE_COUNT+11,

    // force 8-bit size enum so the wire protocol doesn't get outgrown later
    k_EForceDWORD  = 0xFFu,
};

#endif /* defined(__noctisgames__NGSteam__) */
