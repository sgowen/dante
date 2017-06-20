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
#include "FrameworkConstants.h"

class NGSteamAddress;
class ClientProxy;

typedef ClientProxy* (*GetClientProxyFunc)(int inPlayerId);
typedef void (*HandleClientDisconnectedFunc)(ClientProxy* inClientProxy);

class NGSteamServerHelper : public IServerHelper
{
public:
    NGSteamServerHelper(const char* inGameDir, const char* inVersionString, const char* inProductName, const char* inGameDescription, uint16 inPort, uint16 inAuthPort, uint16 inUpdaterPort, ProcessPacketFunc inProcessPacketFunc, HandleNoResponseFunc inHandleNoResponseFunc, HandleConnectionResetFunc inHandleConnectionResetFunc, GetClientProxyFunc inGetClientProxyFunc, HandleClientDisconnectedFunc inHandleClientDisconnectedFunc);
    
    virtual ~NGSteamServerHelper();
    
    virtual void processIncomingPackets();
    
    virtual void processSpecialPacket(uint32_t packetType, InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress);
    
    virtual void onClientDisconnected(ClientProxy* clientProxy);
    
    virtual IMachineAddress* getServerAddress();
    
    virtual bool isConnected();
    
    void kickPlayerOffServer(CSteamID steamID);
    
private:
    const char* m_inGameDir;
    GetClientProxyFunc m_getClientProxyFunc;
    HandleClientDisconnectedFunc m_handleClientDisconnectedFunc;
    NGSteamAddress* m_serverSteamAddress;
    std::string m_serverName;
    bool m_isConnectedToSteam;
    NGSteamAddress* m_outgoingPacketAddress;
    
    struct ClientConnectionData_t
    {
        bool m_bActive;					// Is this slot in use? Or is it available for new connections?
        CSteamID m_SteamIDUser;			// What is the steamid of the player?
    };
    
    // Vector to keep track of client connections which are pending auth
    ClientConnectionData_t m_rgPendingClientData[MAX_NUM_PLAYERS_PER_SERVER];
    
    void sendUpdatedServerDetailsToSteam();
    
    void onClientBeginAuthentication(CSteamID steamIDClient, void* token, uint32 uTokenLen);
    
    void onAuthCompleted(bool bAuthSuccessful, uint32 iPendingAuthIndex);
    
    void sendDataToClient(CSteamID steamIDUser, const OutputMemoryBitStream& inOutputStream);
    
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onSteamServersConnected, SteamServersConnected_t);
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onSteamServersConnectFailure, SteamServerConnectFailure_t);
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onSteamServersDisconnected, SteamServersDisconnected_t);
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onPolicyResponse, GSPolicyResponse_t);
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onValidateAuthTicketResponse, ValidateAuthTicketResponse_t);
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onP2PSessionRequest, P2PSessionRequest_t);
    STEAM_GAMESERVER_CALLBACK(NGSteamServerHelper, onP2PSessionConnectFail, P2PSessionConnectFail_t);
};

#endif /* defined(__noctisgames__NGSteamServerHelper__) */
