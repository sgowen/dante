//
//  NGSteamGameServices.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/11/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamGameServices__
#define __noctisgames__NGSteamGameServices__

#include "NGSteam.h"
#include "NGSteamGameServer.h"

#include <string>
#include <list>

class ISteamRemoteStorage;

#define NG_STEAM_GAME_SERVICES (NGSteamGameServices::getInstance())

#define STEAM_INIT_SUCCESS 1
#define STEAM_UNINITIALIZED 0
#define STEAM_INIT_FAIL_NOT_RUNNING -1
#define STEAM_INIT_FAIL_DRM -2
#define STEAM_INIT_FAIL_API_INIT -3
#define STEAM_INIT_FAIL_LOGGED_ON -4
#define STEAM_INIT_FAIL_CONTROLLER_INIT -5

class NGSteamGameServices
{
public:
    static NGSteamGameServices* getInstance();
    
    int init();
    
    void deinit();
    
#pragma mark Steam Cloud
    bool writeFileToSteamCloud(const char *inFileName, const char *inData);
    std::string readFileFromSteamCloud(const char *inFileName);
    
#pragma mark General Player Functions
    uint64_t getLocalPlayerId();
    std::string getLocalPlayerName();
    std::string getRemotePlayerName(uint64_t inPlayerId);
    
private:
    ISteamRemoteStorage* m_steamRemoteStorage;
    int32 m_nNumFilesInCloud;
    uint64 m_ulBytesQuota;
    uint64 m_ulAvailableBytes;
    
    int m_iStatus;
    
    void refreshSteamCloudFileStats();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    NGSteamGameServices();
    ~NGSteamGameServices();
    NGSteamGameServices(const NGSteamGameServices&);
    NGSteamGameServices& operator=(const NGSteamGameServices&);
};

#endif /* defined(__noctisgames__NGSteamGameServices__) */
