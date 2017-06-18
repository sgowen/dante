//
//  NGSteamClientHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamClientHelper__
#define __noctisgames__NGSteamClientHelper__

#include "IClientHelper.h"

#include "IPacketHandler.h"
#include "NGSteam.h"
#include "NGSteamGameServer.h"

#include <list>

class NGSteamAddress;

class NGSteamClientHelper : public IClientHelper, ISteamMatchmakingServerListResponse
{
public:
    NGSteamClientHelper(const char* inGameDir, bool isLAN, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc, NGSteamAddress* serverSteamAddress = nullptr);
    
    virtual ~NGSteamClientHelper();
    
    virtual void processIncomingPackets();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream);
    
    virtual std::string& getName();
    
    virtual bool isConnected();
    
    void parseCommandLine(const char *pchCmdLine, const char **ppchServerAddress);
    
    void execCommandLineConnect(const char *pchServerAddress);
    
#pragma mark ISteamMatchmakingServerListResponse
    virtual void ServerResponded(HServerListRequest hReq, int iServer);
    virtual void ServerFailedToRespond(HServerListRequest hReq, int iServer);
    virtual void RefreshComplete(HServerListRequest hReq, EMatchMakingServerResponse response);
    
    std::list<NGSteamGameServer>& getGameServers();
    
    bool isRequestingServers();
    
private:
    enum EClientConnectionState
    {
        k_EClientNotConnected,
        k_EClientConnectedPendingAuthentication,
        k_EClientConnectedAndAuthenticated
    };
    EClientConnectionState m_connectionState;
    const char* m_gameDir;
    NGSteamAddress* m_clientSteamAddress;
    NGSteamAddress* m_serverSteamAddress;
    uint32 m_unServerIP;
    uint16 m_usServerPort;
    float m_fLastNetworkDataReceivedTime;
    float m_fLastConnectionAttemptRetryTime;
    std::string m_name;
    int m_iNumServers; // Track the number of servers we know about
    bool m_isRequestingServers; // Track whether we are in the middle of a refresh or not
    HServerListRequest m_hServerListRequest; // Track what server list request is currently running
    std::list<NGSteamGameServer> m_gameServers;
    
    void refreshInternetServers();
    
    void refreshLANServers();
    
    void onReceiveServerExiting();
    
    void initiateServerConnection(uint32 unServerAddress, const int32 nPort);
    
    void initiateServerConnection(CSteamID steamIDGameServer);
    
    // callback for when the lobby game server has started
    STEAM_CALLBACK(NGSteamClientHelper, onGameJoinRequested, GameRichPresenceJoinRequested_t);
    
    // callbacks for Steam connection state
    STEAM_CALLBACK(NGSteamClientHelper, onSteamServersConnected, SteamServersConnected_t);
    
    STEAM_CALLBACK(NGSteamClientHelper, onSteamServersDisconnected, SteamServersDisconnected_t);
    
    STEAM_CALLBACK(NGSteamClientHelper, onSteamServerConnectFailure, SteamServerConnectFailure_t);
    
    STEAM_CALLBACK(NGSteamClientHelper, onGameOverlayActivated, GameOverlayActivated_t);
    
    // connection handler
    STEAM_CALLBACK(NGSteamClientHelper, onP2PSessionConnectFail, P2PSessionConnectFail_t);
    
    // ipc failure handler
    STEAM_CALLBACK(NGSteamClientHelper, onIPCFailure, IPCFailure_t);
    
    // Steam wants to shut down, Game for Windows applications should shutdown too
    STEAM_CALLBACK(NGSteamClientHelper, onSteamShutdown, SteamShutdown_t);
    
    class GameServerPing : public ISteamMatchmakingPingResponse
    {
    public:
        GameServerPing();
        
        // Server has responded successfully and has updated data
        virtual void ServerResponded(gameserveritem_t &server);
        
        // Server failed to respond to the ping request
        virtual void ServerFailedToRespond();
        
        void retrieveSteamIDFromGameServer(NGSteamClientHelper *client, uint32 unIP, uint16 unPort);
        
        void cancelPing();
        
    private:
        HServerQuery m_hGameServerQuery; // we're ping a game server, so we can convert IP:Port to a steamID
        NGSteamClientHelper *m_client;
    };
    
    GameServerPing m_gameServerPing;
};

#endif /* defined(__noctisgames__NGSteamClientHelper__) */
