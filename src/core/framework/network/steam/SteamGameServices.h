//
//  SteamGameServices.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/11/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SteamGameServices__
#define __noctisgames__SteamGameServices__

#include "stdafx.h"
#include "steam/steamtypes.h"

#define STEAM_GAME_SERVICES (SteamGameServices::getInstance())

#ifdef STEAM_CEG
// Steam DRM header file
#include "cegclient.h"
#else
#define Steamworks_InitCEGLibrary() (true)
#define Steamworks_TermCEGLibrary()
#define Steamworks_TestSecret()
#define Steamworks_SelfCheck()
#endif

#define STEAM_INIT_SUCCESS 1
#define STEAM_INIT_FAIL_NOT_RUNNING -1
#define STEAM_INIT_FAIL_DRM -2
#define STEAM_INIT_FAIL_API_INIT -3
#define STEAM_INIT_FAIL_LOGGED_ON -4
#define STEAM_INIT_FAIL_CONTROLLER_INIT -5

class ISteamRemoteStorage;

class SteamGameServices
{
public:
    static SteamGameServices* getInstance();
    
    int init();
    
    void deinit();
    
    void writeFile(const char *inFileName, const void *inData, int32 inDataLength);
    
    int32 readFile(const char *inFileName, void *inData, int32 inDataLength);
    
private:
    ISteamRemoteStorage* m_steamRemoteStorage;
    int32 m_nNumFilesInCloud;
    uint64 m_ulBytesQuota;
    uint64 m_ulAvailableBytes;
    int m_iStatus;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    SteamGameServices();
    ~SteamGameServices();
    SteamGameServices(const SteamGameServices&);
    SteamGameServices& operator=(const SteamGameServices&);
};

#endif /* defined(__noctisgames__SteamGameServices__) */
