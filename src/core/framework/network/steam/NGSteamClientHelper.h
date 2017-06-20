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

class NGSteamAddress;

class NGSteamClientHelper : public IClientHelper
{
public:
    NGSteamClientHelper(CSteamID serverSteamID, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc);
    
    virtual ~NGSteamClientHelper();
    
    virtual void processSpecialPacket(uint32_t packetType, InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress);
    
    virtual int handleUninitialized();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream);
    
    virtual std::string& getName();
    
private:
    // Enum for various client connection states
    enum EClientConnectionState
    {
        k_EClientNotConnected,							// Initial state, not connected to a server
        k_EClientConnectedPendingAuthentication,		// We've established communication with the server, but it hasn't authed us yet
        k_EClientConnectedAndAuthenticated,				// Final phase, server has authed us, we are actually able to play on it
        k_EClientConnectionFailure
    };
    
    EClientConnectionState m_eConnectedStatus;
    NGSteamAddress* m_serverSteamAddress;
    NGSteamAddress* m_clientSteamAddress;
    std::string m_name;
    float m_fTimeOfLastMsgClientBeginAuthentication;
    
    // Server address data
    CSteamID m_steamIDGameServer;
    uint32 m_unServerIP;
    uint16 m_usServerPort;
    HAuthTicket m_hAuthTicket;
    
    void onReceiveServerInfo(CSteamID steamIDGameServer, bool bVACSecure, const char *pchServerName);
    
    void updateRichPresenceConnectionInfo();
};

#endif /* defined(__noctisgames__NGSteamClientHelper__) */
