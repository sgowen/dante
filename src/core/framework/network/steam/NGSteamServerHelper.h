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

class NGSteamServerHelper : public IServerHelper
{
public:
    NGSteamServerHelper(const char* inGameDir, const char* inVersionString, const char* inProductName, const char* inGameDescription, uint16 inPort, uint16 inAuthPort, uint16 inUpdaterPort, ProcessPacketFunc inProcessPacketFunc, HandleNoResponseFunc inHandleNoResponseFunc, HandleConnectionResetFunc inHandleConnectionResetFunc);
    
    virtual ~NGSteamServerHelper();
    
    virtual void processIncomingPackets();
    
    virtual IMachineAddress* getServerAddress();
    
    virtual bool isConnected();
    
private:
    const char* m_inGameDir;
    NGSteamAddress* m_serverSteamAddress;
    std::string m_serverName;
    bool m_isConnectedToSteam; // Track whether our server is connected to Steam ok (meaning we can restrict who plays based on ownership and VAC bans, etc...)
    
    void sendUpdatedServerDetailsToSteam();
    
    void onAuthCompleted(bool bAuthSuccessful, uint32 iPendingAuthIndex);
    
    // Tells us when we have successfully connected to Steam
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onSteamServersConnected, SteamServersConnected_t);
    
    // Tells us when there was a failure to connect to Steam
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onSteamServersConnectFailure, SteamServerConnectFailure_t);
    
    // Tells us when we have been logged out of Steam
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onSteamServersDisconnected, SteamServersDisconnected_t);
    
    // Tells us that Steam has set our security policy (VAC on or off)
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onPolicyResponse, GSPolicyResponse_t);
    
    //
    // Various callback functions that Steam will call to let us know about whether we should
    // allow clients to play or we should kick/deny them.
    //
    
    // Tells us a client has been authenticated and approved to play by Steam (passes auth, license check, VAC status, etc...)
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onValidateAuthTicketResponse, ValidateAuthTicketResponse_t);
    
    // client connection state
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onP2PSessionRequest, P2PSessionRequest_t);
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onP2PSessionConnectFail, P2PSessionConnectFail_t);
};

#endif /* defined(__noctisgames__NGSteamServerHelper__) */
