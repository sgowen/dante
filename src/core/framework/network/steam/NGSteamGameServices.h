//
//  NGSteamGameServices.h
//  noctisgames
//
//  Created by Stephen Gowen on 6/11/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamGameServices__
#define __noctisgames__NGSteamGameServices__

#include <framework/network/steam/NGSteam.h>
#include <framework/network/steam/NGSteamGameServer.h>

#include <string>
#include <vector>

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
    
    void update();
    void refreshInternetServers();
    void refreshLANServers();
    void parseCommandLine(const char *pchCmdLine, const char **ppchServerAddress);
    void connectToServerWithAddress(const char *pchServerAddress);
    void initiateServerConnection(uint32 unServerAddress, const int32 nPort);
    void initiateServerConnection(CSteamID steamIDGameServer);
    CSteamID getServerToJoinSteamID();
    int getStatus();
    bool isRequestingToJoinServer();
    void onServerJoined();
    
#pragma mark ISteamMatchmakingServerListResponse
    virtual void ServerResponded(HServerListRequest hReq, int iServer);
    virtual void ServerFailedToRespond(HServerListRequest hReq, int iServer);
    virtual void RefreshComplete(HServerListRequest hReq, EMatchMakingServerResponse response);
    
    std::vector<NGSteamGameServer>& getGameServers();
    bool isRequestingServers();
    
#pragma mark Steam Cloud
    bool writeFileToSteamCloud(const char *inFileName, const char *inData);
    std::string readFileFromSteamCloud(const char *inFileName);
    void refreshSteamCloudFileStats();
    
private:
    static NGSteamGameServices* s_instance;
    
    const char* _gameDir;
    int _numServers; // Track the number of servers we know about
    bool _isRequestingServers; // Track whether we are in the middle of a refresh or not
    HServerListRequest _hServerListRequest; // Track what server list request is currently running
    std::vector<NGSteamGameServer> _gameServers;
    
    uint32 _unServerIP;
    uint16 _usServerPort;
    
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
        HServerQuery _hGameServerQuery; // we're ping a game server, so we can convert IP:Port to a steamID
        NGSteamGameServices *_client;
    };
    
    GameServerPing _gameServerPing;
    
    CSteamID _steamIDGameServerToJoin;
    
    int32 _nNumFilesInCloud;
    uint64 _ulBytesQuota;
    uint64 _ulAvailableBytes;
    
    int _status;
    bool _isRequestingToJoinServer;
    
    STEAM_CALLBACK(NGSteamGameServices, onGameOverlayActivated, GameOverlayActivated_t);
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
