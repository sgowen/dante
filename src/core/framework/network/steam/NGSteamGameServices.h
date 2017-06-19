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
#include <vector>

class ISteamRemoteStorage;

#define NG_STEAM_GAME_SERVICES (NGSteamGameServices::getInstance())

#define STEAM_INIT_SUCCESS 1
#define STEAM_UNINITIALIZED 0
#define STEAM_INIT_FAIL_NOT_RUNNING -1
#define STEAM_INIT_FAIL_DRM -2
#define STEAM_INIT_FAIL_API_INIT -3
#define STEAM_INIT_FAIL_LOGGED_ON -4
#define STEAM_INIT_FAIL_CONTROLLER_INIT -5
#define STEAM_IPC_FAILURE -6
#define STEAM_SHUTDOWN -7

class NGSteamGameServices : public ISteamMatchmakingServerListResponse
{
public:
    static void create(const char* inGameDir);
    
    static void destroy();
    
    static NGSteamGameServices* getInstance();
    
    void refreshInternetServers();
    
    void refreshLANServers();
    
    void parseCommandLine(const char *pchCmdLine, const char **ppchServerAddress);
    
    void connectToServerWithAddress(const char *pchServerAddress);
    
    void initiateServerConnection(uint32 unServerAddress, const int32 nPort);
    
    void initiateServerConnection(CSteamID steamIDGameServer);
    
    CSteamID getServerToJoinSteamID();
    
    int getStatus();
    
    bool isRequestingToJoinServer();
    
#pragma mark ISteamMatchmakingServerListResponse
    virtual void ServerResponded(HServerListRequest hReq, int iServer);
    virtual void ServerFailedToRespond(HServerListRequest hReq, int iServer);
    virtual void RefreshComplete(HServerListRequest hReq, EMatchMakingServerResponse response);
    
    std::vector<NGSteamGameServer>& getGameServers();
    
    bool isRequestingServers();
    
#pragma mark Steam Cloud
    bool writeFileToSteamCloud(const char *inFileName, const char *inData);
    std::string readFileFromSteamCloud(const char *inFileName);
    
#pragma mark General Player Functions
    uint64_t getLocalPlayerId();
    std::string getLocalPlayerName();
    std::string getRemotePlayerName(uint64_t inPlayerId);
    
private:
    static NGSteamGameServices* s_instance;
    
    const char* m_gameDir;
    int m_iNumServers; // Track the number of servers we know about
    bool m_isRequestingServers; // Track whether we are in the middle of a refresh or not
    HServerListRequest m_hServerListRequest; // Track what server list request is currently running
    std::vector<NGSteamGameServer> m_gameServers;
    
    uint32 m_unServerIP;
    uint16 m_usServerPort;
    
    class GameServerPing : public ISteamMatchmakingPingResponse
    {
    public:
        GameServerPing();
        
        // Server has responded successfully and has updated data
        virtual void ServerResponded(gameserveritem_t &server);
        
        // Server failed to respond to the ping request
        virtual void ServerFailedToRespond();
        
        void retrieveSteamIDFromGameServer(NGSteamGameServices *client, uint32 unIP, uint16 unPort);
        
        void cancelPing();
        
    private:
        HServerQuery m_hGameServerQuery; // we're ping a game server, so we can convert IP:Port to a steamID
        NGSteamGameServices *m_client;
    };
    
    GameServerPing m_gameServerPing;
    
    CSteamID m_steamIDGameServerToJoin;
    
    ISteamRemoteStorage* m_steamRemoteStorage;
    int32 m_nNumFilesInCloud;
    uint64 m_ulBytesQuota;
    uint64 m_ulAvailableBytes;
    
    int m_iStatus;
    bool m_isRequestingToJoinServer;
    
    void refreshSteamCloudFileStats();
    
    STEAM_CALLBACK(NGSteamGameServices, onGameJoinRequested, GameRichPresenceJoinRequested_t);
    STEAM_CALLBACK(NGSteamGameServices, onIPCFailure, IPCFailure_t);
    STEAM_CALLBACK(NGSteamGameServices, onSteamShutdown, SteamShutdown_t);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    NGSteamGameServices(const char* inGameDir);
    ~NGSteamGameServices();
    NGSteamGameServices(const NGSteamGameServices&);
    NGSteamGameServices& operator=(const NGSteamGameServices&);
};

#endif /* defined(__noctisgames__NGSteamGameServices__) */
