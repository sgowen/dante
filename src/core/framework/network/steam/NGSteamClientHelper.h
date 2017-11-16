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
#include "FrameworkConstants.h"

class NGSteamP2PAuth;
class NGSteamAddress;

typedef uint64_t (*GetPlayerAddressHashFunc)(uint8_t inPlayerIndex);

class NGSteamClientHelper : public IClientHelper
{
public:
    NGSteamClientHelper(CSteamID serverSteamID, GetPlayerAddressHashFunc getPlayerAddressHashFunc, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc);
    
    virtual ~NGSteamClientHelper();
    
    virtual void processIncomingPackets();
    
    virtual void processSpecialPacket(uint32_t packetType, InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress);
    
    virtual void handleUninitialized();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream);
    
    virtual std::string& getName();
    
private:
    // Enum for various client connection states
    enum EClientConnectionState
    {
        k_EClientNotConnected,							// Initial state, not connected to a server
        k_EClientConnectedPendingAuthentication,		// We've established communication with the server, but it hasn't authed us yet
        k_EClientConnectedAndAuthenticated,				// Final phase, server has authed us, we are actually able to play on it
        k_EClientConnectionFailure,
        k_EServerIsNotAuthorized,
        k_EServerShuttingDown
    };
    
    NGSteamP2PAuth* m_steamP2PAuth;
    GetPlayerAddressHashFunc m_getPlayerAddressHashFunc;
    EClientConnectionState m_eConnectedStatus;
    NGSteamAddress* m_serverSteamAddress;
    std::string m_name;
    float m_fTimeOfLastMsgClientBeginAuthentication;
    
    CSteamID m_rgSteamIDPlayers[MAX_NUM_PLAYERS_PER_SERVER];
    
    // Server address data
    uint32_t m_unServerIP;
    uint16 m_usServerPort;
    HAuthTicket m_hAuthTicket;
    
    void onReceiveServerInfo(CSteamID steamIDGameServer, bool bVACSecure, const char *pchServerName);
    
    void updateRichPresenceConnectionInfo();
    
    void updateState();
};

#endif /* defined(__noctisgames__NGSteamClientHelper__) */
