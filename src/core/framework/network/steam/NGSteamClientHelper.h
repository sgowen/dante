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
    NGSteamClientHelper(const char* inGameDir, bool isLAN, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc);
    
    virtual ~NGSteamClientHelper();
    
    virtual void processIncomingPackets();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream);
    
    virtual std::string& getName();
    
#pragma mark ISteamMatchmakingServerListResponse
    virtual void ServerResponded(HServerListRequest hReq, int iServer);
    virtual void ServerFailedToRespond(HServerListRequest hReq, int iServer);
    virtual void RefreshComplete(HServerListRequest hReq, EMatchMakingServerResponse response);
    
    std::list<NGSteamGameServer>& getGameServers();
    
    bool isRequestingServers();
    
private:
    const char* m_gameDir;
    NGSteamAddress* m_clientSteamAddress;
    NGSteamAddress* m_serverSteamAddress;
    std::string m_name;
    int m_iNumServers; // Track the number of servers we know about
    bool m_isRequestingServers; // Track whether we are in the middle of a refresh or not
    HServerListRequest m_hServerListRequest; // Track what server list request is currently running
    std::list<NGSteamGameServer> m_gameServers;
    
    void refreshInternetServers();
    
    void refreshLANServers();
};

#endif /* defined(__noctisgames__NGSteamClientHelper__) */
