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

//// Network ////

#define NETWORK_PACKET_TYPE_STEAM_CLIENT_INITIATE_CONNECTION 'SCIC'

#endif /* defined(__noctisgames__NGSteam__) */
