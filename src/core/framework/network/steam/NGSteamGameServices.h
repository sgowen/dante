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

class OutputMemoryBitStream;
class ISteamRemoteStorage;

#define NG_STEAM_GAME_SERVICES (NGSteamGameServices::getInstance())

#define STEAM_INIT_SUCCESS 1
#define STEAM_INIT_NOT_CALLED 0
#define STEAM_INIT_FAIL_NOT_RUNNING -1
#define STEAM_INIT_FAIL_DRM -2
#define STEAM_INIT_FAIL_API_INIT -3
#define STEAM_INIT_FAIL_LOGGED_ON -4
#define STEAM_INIT_FAIL_CONTROLLER_INIT -5

class NGSteamGameServices : public ISteamMatchmakingServerListResponse
{
public:
    static NGSteamGameServices* getInstance();
    
    int init();
    
    void deinit();
    
    void update();
    
    void refreshInternetServers();
    
    void refreshLANServers();
    
#pragma mark ISteamMatchmakingServerListResponse
    virtual void ServerResponded(HServerListRequest hReq, int iServer);
    virtual void ServerFailedToRespond(HServerListRequest hReq, int iServer);
    virtual void RefreshComplete(HServerListRequest hReq, EMatchMakingServerResponse response);
    
#pragma mark Steam Cloud
    bool writeFileToSteamCloud(const char *inFileName, const char *inData);
    std::string readFileFromSteamCloud(const char *inFileName);
    
#pragma mark General Player Functions
    uint64_t getLocalPlayerId();
    std::string getLocalPlayerName();
    std::string getRemotePlayerName(uint64_t inPlayerId);
    
#pragma mark P2P Networking
    STEAM_CALLBACK(NGSteamGameServices, onP2PSessionRequest, P2PSessionRequest_t, m_sessionRequestCallback);
    STEAM_CALLBACK(NGSteamGameServices, onP2PSessionFail, P2PSessionConnectFail_t, m_sessionFailCallback);
    bool sendP2PReliable(const OutputMemoryBitStream& inOutputStream, uint64_t inToPlayer);
    bool sendP2PUnreliable(const OutputMemoryBitStream& inOutputStream, uint64_t inToPlayer);
    bool isP2PPacketAvailable(uint32_t& outPacketSize);
    uint32_t readP2PPacket(void* inToReceive, uint32_t inMaxLength, uint64_t& outFromPlayer);
    
private:
    ISteamRemoteStorage* m_steamRemoteStorage;
    int32 m_nNumFilesInCloud;
    uint64 m_ulBytesQuota;
    uint64 m_ulAvailableBytes;
    
    int m_nServers; // Track the number of servers we know about
    bool m_bRequestingServers; // Track whether we are in the middle of a refresh or not
    HServerListRequest m_hServerListRequest; // Track what server list request is currently running
    std::list<NGSteamGameServer> m_ListGameServers;
    
    int m_iStatus;
    
    void refreshSteamCloudFileStats();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    NGSteamGameServices();
    ~NGSteamGameServices();
    NGSteamGameServices(const NGSteamGameServices&);
    NGSteamGameServices& operator=(const NGSteamGameServices&);
};

#endif /* defined(__noctisgames__NGSteamGameServices__) */
