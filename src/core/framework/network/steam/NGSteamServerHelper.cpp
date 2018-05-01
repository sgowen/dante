//
//  NGSteamServerHelper.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/steam/NGSteamServerHelper.h>

#include <framework/network/steam/NGSteamAddress.h>
#include <framework/network/server/ClientProxy.h>

#include <framework/network/steam/NGSteamPacketHandler.h>
#include <framework/network/steam/NGSteamGameServices.h>
#include <framework/util/StringUtil.h>
#include <framework/util/Constants.h>
#include <framework/network/portable/OutputMemoryBitStream.h>
#include <framework/util/Timing.h>
#include <framework/util/InstanceManager.h>

#include <assert.h>

// UDP port for the server to listen on
#define STEAM_SERVER_PORT 27015

// UDP port for the master server updater to listen on
#define STEAM_MASTER_SERVER_UPDATER_PORT 27016

NGSteamServerHelper::NGSteamServerHelper(std::string inGameDir, std::string inVersionString, std::string inProductName, std::string inGameDescription, uint16 inPort, ProcessPacketFunc inProcessPacketFunc, HandleNoResponseFunc inHandleNoResponseFunc, HandleConnectionResetFunc inHandleConnectionResetFunc, GetClientProxyFunc inGetClientProxyFunc, HandleClientDisconnectedFunc inHandleClientDisconnectedFunc) : ServerHelper(new NGSteamPacketHandler(static_cast<Timing*>(INSTANCE_MANAGER->get(INSTANCE_TIME_SERVER)), true, inProcessPacketFunc, inHandleNoResponseFunc, inHandleConnectionResetFunc), inGetClientProxyFunc, inHandleClientDisconnectedFunc),
_gameDir(inGameDir),
_serverSteamAddress(new NGSteamAddress()),
_isConnectedToSteam(false),
_outgoingPacketAddress(new NGSteamAddress())
{
    // Initialize the SteamGameServer interface, we tell it some info about us, and we request support
    // for both Authentication (making sure users own games) and secure mode, VAC running in our game
    // and kicking users who are VAC banned
    
    if (!SteamGameServer_Init(INADDR_ANY, STEAM_SERVER_PORT, inPort, STEAM_MASTER_SERVER_UPDATER_PORT, eServerModeAuthenticationAndSecure, inVersionString.c_str()))
    {
        LOG("SteamGameServer_Init call failed");
    }
    
    if (SteamGameServer())
    {
        // Set the "game dir".
        // This is currently required for all games.  However, soon we will be
        // using the AppID for most purposes, and this string will only be needed
        // for mods.  it may not be changed after the server has logged on
        SteamGameServer()->SetModDir(_gameDir.c_str());
        
        // These fields are currently required, but will go away soon.
        // See their documentation for more info
        SteamGameServer()->SetProduct(inProductName.c_str());
        SteamGameServer()->SetGameDescription(inGameDescription.c_str());
        
        // We don't support specators in our game.
        // .... but if we did:
        //SteamGameServer()->SetSpectatorPort(...);
        //SteamGameServer()->SetSpectatorServerName(...);
        
        // Initiate Anonymous logon.
        // Coming soon: Logging into authenticated, persistent game server account
        SteamGameServer()->LogOnAnonymous();
        
        // We want to actively update the master server with our presence so players can
        // find us via the steam matchmaking/server browser interfaces
        SteamGameServer()->EnableHeartbeats(true);
        
        LOG("SteamGameServer() interface is valid");
    }
    else
    {
        LOG("SteamGameServer() interface is invalid");
    }
    
    // zero the client connection data
    memset(&_rgPendingClientData, 0, sizeof(_rgPendingClientData));
}

NGSteamServerHelper::~NGSteamServerHelper()
{
    // Notify Steam master server we are going offline
    SteamGameServer()->EnableHeartbeats(false);
    
    for (uint8_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        ClientProxy* clientProxy = _getClientProxyFunc(i);
        if (clientProxy)
        {
            NGSteamAddress* userAddress = static_cast<NGSteamAddress*>(clientProxy->getMachineAddress());
            
            OutputMemoryBitStream packet;
            packet.write(static_cast<uint8_t>(k_EMsgServerExiting));
            
            sendPacket(packet, userAddress);
        }
    }
    
    delete _serverSteamAddress;
    delete _outgoingPacketAddress;
    
    // Disconnect from the steam servers
    SteamGameServer()->LogOff();
    
    // release our reference to the steam client library
    SteamGameServer_Shutdown();
}

void NGSteamServerHelper::processIncomingPackets()
{
    NetworkHelper::processIncomingPackets();
    
    sendUpdatedServerDetailsToSteam();
}

void NGSteamServerHelper::processSpecialPacket(uint8_t packetType, InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress)
{
    switch (packetType)
    {
        case k_EMsgClientInitiateConnection:
        {
            // We always let clients do this without even checking for room on the server since we reserve that for
            // the authentication phase of the connection which comes next
            // TODO
            OutputMemoryBitStream packet;
            packet.write(static_cast<uint8_t>(k_EMsgServerSendInfo));
            packet.write(SteamGameServer()->GetSteamID().ConvertToUint64());
            // You can only make use of VAC when using the Steam authentication system
            packet.write(SteamGameServer()->BSecure());
            packet.writeLarge(_serverName);
            
            sendPacket(packet, inFromAddress);
        }
            break;
        case k_EMsgClientBeginAuthentication:
        {
            CSteamID steamIDRemote = (static_cast<NGSteamAddress*>(inFromAddress))->getSteamID();
            
            uint32_t uTokenLen;
            inInputStream.read(uTokenLen);
            
            char token[1024];
            inInputStream.readBytes(&token, uTokenLen);
            
            onClientBeginAuthentication(steamIDRemote, (void*)token, uTokenLen);
        }
            break;
        case k_EMsgClientLeavingServer:
        {
            LOG("Client is leaving the server");
            
            CSteamID steamIDRemote = (static_cast<NGSteamAddress*>(inFromAddress))->getSteamID();
            
            // Find the connection that should exist for this users address
            bool isFound = false;
            for (uint8_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
            {
                ClientProxy* clientProxy = _getClientProxyFunc(i);
                if (clientProxy)
                {
                    NGSteamAddress* userAddress = static_cast<NGSteamAddress*>(clientProxy->getMachineAddress());
                    if (userAddress->getSteamID() == steamIDRemote)
                    {
                        isFound = true;
                        _handleClientDisconnectedFunc(clientProxy);
                        break;
                    }
                }
                
                // Also check for pending connections that may match
                if (_rgPendingClientData[i]._steamIDUser == steamIDRemote)
                {
                    // Tell the GS the user is leaving the server
                    SteamGameServer()->SendUserDisconnect(_rgPendingClientData[i]._steamIDUser);
                    
                    // Clear our data on the user
                    memset(&_rgPendingClientData[i], 0 , sizeof(ClientConnectionData_t));
                    break;
                }
            }
            
            if (!isFound)
            {
                LOG("Got a client leaving server msg, but couldn't find a matching client");
            }
        }
            break;
    }
}

void NGSteamServerHelper::onClientDisconnected(ClientProxy* clientProxy)
{
    NGSteamAddress* userAddress = static_cast<NGSteamAddress*>(clientProxy->getMachineAddress());
    
    // Tell the GS the user is leaving the server
    SteamGameServer()->EndAuthSession(userAddress->getSteamID());
}

MachineAddress* NGSteamServerHelper::getServerAddress()
{
    _serverSteamAddress->setSteamID(SteamGameServer()->GetSteamID());
    
    return _serverSteamAddress;
}

bool NGSteamServerHelper::isConnected()
{
    return _isConnectedToSteam;
}

void NGSteamServerHelper::kickPlayerOffServer(CSteamID steamID)
{
    for (uint8_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        ClientProxy* clientProxy = _getClientProxyFunc(i);
        // If client is not active, skip
        if (!clientProxy)
        {
            continue;
        }
        
        NGSteamAddress* userAddress = static_cast<NGSteamAddress*>(clientProxy->getMachineAddress());
        
        if (userAddress->getSteamID() == steamID)
        {
            LOG("Kicking player");
            _handleClientDisconnectedFunc(clientProxy);
            
            // send him a kick message
            OutputMemoryBitStream packet;
            packet.write(static_cast<uint8_t>(k_EMsgServerFailAuthentication));
            sendDataToClient(steamID, packet);
        }
    }
}

void NGSteamServerHelper::sendUpdatedServerDetailsToSteam()
{
    // Tell the Steam authentication servers about our game
    // If a client is running (should always be since we don't support a dedicated server)
    // then we'll form the name based off of it
    _serverName = StringUtil::format("%s's game", SteamFriends()->GetPersonaName());
    
    // These server state variables may be changed at any time. Note that there is no lnoger a mechanism
    // to send the player count. The player count is maintained by steam and you should use the player
    // creation/authentication functions to maintain your player count.
    SteamGameServer()->SetMaxPlayerCount(MAX_NUM_PLAYERS_PER_SERVER);
    SteamGameServer()->SetPasswordProtected(false); // TODO, will definitely want to revisit this
    SteamGameServer()->SetServerName(_serverName.c_str());
    SteamGameServer()->SetBotPlayerCount(0); // optional, defaults to zero
    SteamGameServer()->SetMapName("Wasteland"); // TODO, do we need to specify a map name?
    
    // Update all the players names
    for (uint8_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        ClientProxy* clientProxy = _getClientProxyFunc(i);
        if (clientProxy)
        {
            NGSteamAddress* userAddress = static_cast<NGSteamAddress*>(clientProxy->getMachineAddress());
            SteamGameServer()->BUpdateUserData(userAddress->getSteamID(), clientProxy->getName().c_str(), 0);
        }
    }
}

//-----------------------------------------------------------------------------
// Purpose: Handle a new client connecting
//-----------------------------------------------------------------------------
void NGSteamServerHelper::onClientBeginAuthentication(CSteamID steamIDClient, void* token, uint32_t uTokenLen)
{
    LOG("onClientBeginAuthentication, uTokenLen: %i", uTokenLen);
    
    // First, check this isn't a duplicate and we already have a user logged on from the same steamid
    for (uint8_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        ClientProxy* clientProxy = _getClientProxyFunc(i);
        if (clientProxy)
        {
            NGSteamAddress* userAddress = static_cast<NGSteamAddress*>(clientProxy->getMachineAddress());
            if (userAddress->getSteamID() == steamIDClient)
            {
                // We already logged them on... (should maybe tell them again incase they don't know?)
                return;
            }
        }
    }
    
    // Second, do we have room?
    uint32_t nPendingOrActivePlayerCount = 0;
    for (uint8_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (_rgPendingClientData[i]._isActive)
        {
            ++nPendingOrActivePlayerCount;
        }
        
        if (_getClientProxyFunc(i))
        {
            ++nPendingOrActivePlayerCount;
        }
    }
    
    // We are full (or will be if the pending players auth), deny new login
    if (nPendingOrActivePlayerCount >= MAX_NUM_PLAYERS_PER_SERVER)
    {
        OutputMemoryBitStream packet;
        packet.write(static_cast<uint8_t>(k_EMsgServerFailAuthentication));
        sendDataToClient(steamIDClient, packet);
    }
    
    // If we get here there is room, add the player as pending
    for (uint32_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (!_rgPendingClientData[i]._isActive)
        {
            // authenticate the user with the Steam back-end servers
            if (k_EBeginAuthSessionResultOK != SteamGameServer()->BeginAuthSession(token, uTokenLen, steamIDClient))
            {
                OutputMemoryBitStream packet;
                packet.write(static_cast<uint8_t>(k_EMsgServerFailAuthentication));
                sendDataToClient(steamIDClient, packet);
                break;
            }
            
            _rgPendingClientData[i]._steamIDUser = steamIDClient;
            _rgPendingClientData[i]._isActive = true;
            break;
        }
    }
}

/// Purpose: A new client that connected has had their authentication processed
void NGSteamServerHelper::onAuthCompleted(bool bAuthSuccessful, uint32_t iPendingAuthIndex)
{
    LOG("NGSteamServerHelper onAuthCompleted");
    
    if (!_rgPendingClientData[iPendingAuthIndex]._isActive)
    {
        LOG("Got auth completed callback for client who is not pending");
        return;
    }
    
    if (bAuthSuccessful)
    {
        for (uint8_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
        {
            ClientProxy* clientProxy = _getClientProxyFunc(i);
            if (!clientProxy)
            {
                OutputMemoryBitStream packet;
                packet.write(static_cast<uint8_t>(k_EMsgServerPassAuthentication));
                sendDataToClient(_rgPendingClientData[iPendingAuthIndex]._steamIDUser, packet);
                
                // copy over the data from the pending array
                memset(&_rgPendingClientData[iPendingAuthIndex], 0, sizeof(ClientConnectionData_t));
                
                break;
            }
        }
    }
    else
    {
        // Tell the GS the user is leaving the server
        SteamGameServer()->EndAuthSession(_rgPendingClientData[iPendingAuthIndex]._steamIDUser);
        
        // Send a deny for the client, and zero out the pending data
        OutputMemoryBitStream packet;
        packet.write(static_cast<uint8_t>(k_EMsgServerFailAuthentication));
        sendDataToClient(_rgPendingClientData[iPendingAuthIndex]._steamIDUser, packet);
        
        memset(&_rgPendingClientData[iPendingAuthIndex], 0, sizeof(ClientConnectionData_t));
    }
}

void NGSteamServerHelper::sendDataToClient(CSteamID steamIDUser, const OutputMemoryBitStream& inOutputStream)
{
    _outgoingPacketAddress->setSteamID(steamIDUser);
    
    sendPacket(inOutputStream, _outgoingPacketAddress);
}

#pragma mark - STEAM_GAMESERVER_CALLBACK

void NGSteamServerHelper::onSteamServersConnected(SteamServersConnected_t *pLogonSuccess)
{
    LOG("%s connected to Steam successfully", _gameDir.c_str());
    
    _isConnectedToSteam = true;
    
    // log on is not finished until OnPolicyResponse() is called
    
    sendUpdatedServerDetailsToSteam();
}

void NGSteamServerHelper::onSteamServersConnectFailure(SteamServerConnectFailure_t *pConnectFailure)
{
    LOG("%s failed to connect to Steam", _gameDir.c_str());
    
    _isConnectedToSteam = false;
}

void NGSteamServerHelper::onSteamServersDisconnected(SteamServersDisconnected_t *pLoggedOff)
{
    LOG("%s got logged out of Steam", _gameDir.c_str());
    
    _isConnectedToSteam = false;
}

void NGSteamServerHelper::onPolicyResponse(GSPolicyResponse_t *pPolicyResponse)
{
    if (SteamGameServer()->BSecure())
    {
        LOG("%s is VAC Secure!", _gameDir.c_str());
    }
    else
    {
        LOG("%s is not VAC Secure!", _gameDir.c_str());
    }
    
    LOG("Game Server Steam ID: %llu", SteamGameServer()->GetSteamID().ConvertToUint64());
}

/// Purpose: Tells us Steam3 (VAC and newer license checking) has accepted the user connection
void NGSteamServerHelper::onValidateAuthTicketResponse(ValidateAuthTicketResponse_t *pResponse)
{
    LOG("onValidateAuthTicketResponse");
    
    if (pResponse->m_eAuthSessionResponse == k_EAuthSessionResponseOK)
    {
        // This is the final approval, and means we should let the client play (find the pending auth by steamid)
        for (uint32_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
        {
            if (!_rgPendingClientData[i]._isActive)
            {
                continue;
            }
            else if (_rgPendingClientData[i]._steamIDUser == pResponse->m_SteamID)
            {
                LOG("Auth completed for a client");
                onAuthCompleted(true, i);
                return;
            }
        }
    }
    else
    {
        // Looks like we shouldn't let this user play, kick them
        for (uint32_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
        {
            if (!_rgPendingClientData[i]._isActive)
            {
                continue;
            }
            else if (_rgPendingClientData[i]._steamIDUser == pResponse->m_SteamID)
            {
                LOG("Auth failed for a client");
                onAuthCompleted(false, i);
                return;
            }
        }
    }
}

/// Purpose: Handle clients connecting
void NGSteamServerHelper::onP2PSessionRequest(P2PSessionRequest_t *pCallback)
{
    LOG("onP2PSessionRequest");
    
    // we'll accept a connection from anyone
    SteamGameServerNetworking()->AcceptP2PSessionWithUser(pCallback->m_steamIDRemote);
}

/// Handle clients disconnecting
void NGSteamServerHelper::onP2PSessionConnectFail(P2PSessionConnectFail_t *pCallback)
{
    LOG("onP2PSessionConnectFail");
    
    // socket has closed, kick the user associated with it
    for (uint8_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        ClientProxy* clientProxy = _getClientProxyFunc(i);
        // If client is not active, skip
        if (!clientProxy)
        {
            continue;
        }
        
        NGSteamAddress* userAddress = static_cast<NGSteamAddress*>(clientProxy->getMachineAddress());
        
        if (userAddress->getSteamID() == pCallback->m_steamIDRemote)
        {
            LOG("Disconnected dropped user");
            _handleClientDisconnectedFunc(clientProxy);
            break;
        }
    }
}
