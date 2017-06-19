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

class NGSteamClientHelper : public IClientHelper
{
public:
    NGSteamClientHelper(CSteamID serverSteamID, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc);
    
    virtual ~NGSteamClientHelper();
    
    virtual void processIncomingPackets();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream);
    
    virtual std::string& getName();
    
private:
    NGSteamAddress* m_serverSteamAddress;
    NGSteamAddress* m_clientSteamAddress;
    std::string m_name;
    
    void onReceiveServerExiting();
    
    STEAM_CALLBACK(NGSteamClientHelper, onSteamServersConnected, SteamServersConnected_t);
    STEAM_CALLBACK(NGSteamClientHelper, onSteamServersDisconnected, SteamServersDisconnected_t);
    STEAM_CALLBACK(NGSteamClientHelper, onSteamServerConnectFailure, SteamServerConnectFailure_t);
    STEAM_CALLBACK(NGSteamClientHelper, onP2PSessionConnectFail, P2PSessionConnectFail_t);
};

#endif /* defined(__noctisgames__NGSteamClientHelper__) */
