//
//  NGSteamServerHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamServerHelper__
#define __noctisgames__NGSteamServerHelper__

#include "IServerHelper.h"

#include "NGSteam.h"
#include "IPacketHandler.h"

class NGSteamAddress;
class ClientProxy;

typedef ClientProxy* (*GetClientProxyFunc)(int inPlayerId);

class NGSteamServerHelper : public IServerHelper
{
public:
    NGSteamServerHelper(const char* inGameDir, const char* inVersionString, const char* inProductName, const char* inGameDescription, uint16 inPort, uint16 inAuthPort, uint16 inUpdaterPort, ProcessPacketFunc inProcessPacketFunc, HandleNoResponseFunc inHandleNoResponseFunc, HandleConnectionResetFunc inHandleConnectionResetFunc, GetClientProxyFunc inGetClientProxyFunc);
    
    virtual ~NGSteamServerHelper();
    
    virtual void processIncomingPackets();
    
    virtual IMachineAddress* getServerAddress();
    
    virtual bool isConnected();
    
private:
    const char* m_inGameDir;
    GetClientProxyFunc m_getClientProxyFunc;
    NGSteamAddress* m_serverSteamAddress;
    std::string m_serverName;
    bool m_isConnectedToSteam;
    
    void sendUpdatedServerDetailsToSteam();
    
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onSteamServersConnected, SteamServersConnected_t);
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onSteamServersConnectFailure, SteamServerConnectFailure_t);
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onSteamServersDisconnected, SteamServersDisconnected_t);
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onPolicyResponse, GSPolicyResponse_t);
};

#endif /* defined(__noctisgames__NGSteamServerHelper__) */
