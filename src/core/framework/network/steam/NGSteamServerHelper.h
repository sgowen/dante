//
//  NGSteamServerHelper.h
//  noctisgames
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamServerHelper__
#define __noctisgames__NGSteamServerHelper__

#include <framework/network/server/ServerHelper.h>

#include <framework/network/steam/NGSteam.h>
#include <framework/network/portable/PacketHandler.h>
#include <framework/util/Constants.h>

#include <string>

class NGSteamAddress;
class ClientProxy;

class NGSteamServerHelper : public ServerHelper
{
public:
    NGSteamServerHelper(std::string inGameDir, std::string inVersionString, std::string inProductName, std::string inGameDescription, uint16 inPort, ProcessPacketFunc inProcessPacketFunc, HandleNoResponseFunc inHandleNoResponseFunc, HandleConnectionResetFunc inHandleConnectionResetFunc, GetClientProxyFunc inGetClientProxyFunc, HandleClientDisconnectedFunc inHandleClientDisconnectedFunc);
    
    virtual ~NGSteamServerHelper();
    
    virtual void processIncomingPackets();
    
    virtual void processSpecialPacket(uint8_t packetType, InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress);
    
    virtual void onClientDisconnected(ClientProxy* clientProxy);
    
    virtual MachineAddress* getServerAddress();
    
    virtual bool isConnected();
    
    void kickPlayerOffServer(CSteamID steamID);
    
private:
    std::string _gameDir;
    NGSteamAddress* _serverSteamAddress;
    std::string _serverName;
    bool _isConnectedToSteam;
    NGSteamAddress* _outgoingPacketAddress;
    
    struct ClientConnectionData_t
    {
        bool _isActive;					// Is this slot in use? Or is it available for new connections?
        CSteamID _steamIDUser;			// What is the steamid of the player?
    };
    
    // Vector to keep track of client connections which are pending auth
    ClientConnectionData_t _rgPendingClientData[MAX_NUM_PLAYERS_PER_SERVER];
    
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
