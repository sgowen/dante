//
//  NGSteamClientHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NGSteamClientHelper.h"

#include "NGSteamP2PAuth.h"
#include "NGSteamAddress.h"

#include "NGSteamPacketHandler.h"
#include "macros.h"
#include "StringUtil.h"
#include "Timing.h"
#include "OutputMemoryBitStream.h"
#include "NGSteamGameServices.h"
#include "NetworkManagerServer.h"
#include "NGSteamServerHelper.h"

NGSteamClientHelper::NGSteamClientHelper(CSteamID serverSteamID, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc) : IClientHelper(new NGSteamPacketHandler(false, processPacketFunc, handleNoResponseFunc, handleConnectionResetFunc)),
m_steamP2PAuth(new NGSteamP2PAuth(this)),
m_eConnectedStatus(k_EClientNotConnected),
m_serverSteamAddress(new NGSteamAddress(serverSteamID)),
m_fTimeOfLastMsgClientBeginAuthentication(0.0f),
m_unServerIP(0),
m_usServerPort(0),
m_hAuthTicket(k_HAuthTicketInvalid)
{
    m_name = std::string(SteamFriends()->GetFriendPersonaName(SteamUser()->GetSteamID()));
    
    LOG("Client %s is connecting to Game Server with Steam ID: %s", m_name.c_str(), m_serverSteamAddress->toString().c_str());
    
    NG_STEAM_GAME_SERVICES->onServerJoined();
}

NGSteamClientHelper::~NGSteamClientHelper()
{
    if (m_hAuthTicket != k_HAuthTicketInvalid)
    {
        SteamUser()->CancelAuthTicket(m_hAuthTicket);
    }
    
    OutputMemoryBitStream packet;
    packet.write(k_EMsgClientLeavingServer);
    sendPacket(packet);
    
    m_steamP2PAuth->endGame();
    
    delete m_steamP2PAuth;
    m_steamP2PAuth = nullptr;
    
    if (m_serverSteamAddress->getSteamID().IsValid())
    {
        SteamNetworking()->CloseP2PSessionWithUser(m_serverSteamAddress->getSteamID());
    }
    
    delete m_serverSteamAddress;
}

void NGSteamClientHelper::processIncomingPackets()
{
    INetworkHelper::processIncomingPackets();
    
    // TODO P2P Auth
//    // has the player list changed?
//    if (NG_SERVER)
//    {
//        // if i am the server owner i need to auth everyone who wants to play
//        // assume i am in slot 0, so start at slot 1
//        for (uint32 i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
//        {
//            CSteamID steamIDNew(pUpdateData->GetPlayerSteamID(i));
//            if (steamIDNew == SteamUser()->GetSteamID())
//            {
//                LOG("Server player slot 0 is not server owner.");
//            }
//            else if (steamIDNew != m_rgSteamIDPlayers[i])
//            {
//                if (m_rgSteamIDPlayers[i].IsValid())
//                {
//                    m_steamP2PAuth->playerDisconnect(i);
//                }
//                if (steamIDNew.IsValid())
//                {
//                    m_steamP2PAuth->registerPlayer(i, steamIDNew);
//                }
//            }
//        }
//    }
//    else
//    {
//        // i am just a client, i need to auth the game owner (slot 0)
//        CSteamID steamIDNew(pUpdateData->GetPlayerSteamID(0));
//        if (steamIDNew == SteamUser()->GetSteamID())
//        {
//            LOG("Server player slot 0 is not server owner.");
//        }
//        else if (steamIDNew != m_rgSteamIDPlayers[0])
//        {
//            if (m_rgSteamIDPlayers[0].IsValid())
//            {
//                LOG("Server player slot 0 has disconnected - but thats the server owner.");
//                m_steamP2PAuth->playerDisconnect(0);
//            }
//            if (steamIDNew.IsValid())
//            {
//                m_steamP2PAuth->startAuthPlayer(0, steamIDNew);
//            }
//        }
//    }
//    
//    if (NG_SERVER)
//    {
//        // Now if we are the owner of the game, lets make sure all of our players are legit.
//        // if they are not, we tell the server to kick them off
//        // Start at 1 to skip myself
//        for (int i = 1; i < MAX_NUM_PLAYERS_PER_SERVER; i++)
//        {
//            if (m_steamP2PAuth->m_rgpP2PAuthPlayer[i] && !m_steamP2PAuth->m_rgpP2PAuthPlayer[i]->isAuthOk())
//            {
//                NGSteamServerHelper* helper = static_cast<NGSteamServerHelper*>(NG_SERVER->getServerHelper());
//                helper->kickPlayerOffServer(m_steamP2PAuth->m_rgpP2PAuthPlayer[i]->m_steamID);
//            }
//        }
//    }
//    else
//    {
//        // If we are not the owner of the game, lets make sure the game owner is legit
//        // if he is not, we leave the game
//        if (m_steamP2PAuth->m_rgpP2PAuthPlayer[0])
//        {
//            if (!m_steamP2PAuth->m_rgpP2PAuthPlayer[0]->isAuthOk())
//            {
//                // leave the game
//                m_eConnectedStatus = k_EServerIsNotAuthorized;
//                updateState();
//            }
//        }
//    }
}

void NGSteamClientHelper::processSpecialPacket(uint32_t packetType, InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress)
{
    switch (packetType)
    {
        case k_EMsgServerSendInfo:
        {
            if (m_eConnectedStatus == k_EClientNotConnected)
            {
                uint64 steamIDGameServer;
                bool bVACSecure;
                std::string serverName;
                inInputStream.read(steamIDGameServer);
                inInputStream.read(bVACSecure);
                inInputStream.read(serverName);
                
                onReceiveServerInfo(CSteamID(steamIDGameServer), bVACSecure, serverName.c_str());
            }
        }
            break;
        case k_EMsgServerPassAuthentication:
            m_eConnectedStatus = k_EClientConnectedAndAuthenticated;
            // set information so our friends can join the lobby
            updateRichPresenceConnectionInfo();
            break;
        case k_EMsgServerFailAuthentication:
            LOG("Connection failure. Multiplayer authentication failed");
            m_eConnectedStatus = k_EClientConnectionFailure;
            break;
        case k_EMsgServerExiting:
            LOG("Server Shutting Down");
            m_eConnectedStatus = k_EServerShuttingDown;
            
            m_steamP2PAuth->endGame();
        default:
            m_steamP2PAuth->handleMessage(packetType, inInputStream);
            break;
    }
    
    updateState();
}

void NGSteamClientHelper::handleUninitialized()
{
    switch (m_eConnectedStatus)
    {
        case k_EClientNotConnected:
        {
            OutputMemoryBitStream packet;
            packet.write(k_EMsgClientInitiateConnection);
            sendPacket(packet);
        }
            break;
        case k_EClientConnectedPendingAuthentication:
        {
            float time = Timing::getInstance()->getFrameStartTime();
            
            if (time > m_fTimeOfLastMsgClientBeginAuthentication + 7.0f)
            {
                char rgchToken[1024];
                uint32 unTokenLen = 0;
                m_hAuthTicket = SteamUser()->GetAuthSessionTicket(rgchToken, sizeof(rgchToken), &unTokenLen);
                
                Steamworks_TestSecret();
                
                if (unTokenLen < 1)
                {
                    LOG("Warning: Looks like GetAuthSessionTicket didn't give us a good ticket");
                }
                
                LOG("MsgClientBeginAuthentication_t, uTokenLen: %i", unTokenLen);
                
                OutputMemoryBitStream packet;
                packet.write(k_EMsgClientBeginAuthentication);
                packet.write(unTokenLen);
                packet.writeBytes(rgchToken, unTokenLen);
                sendPacket(packet);
                
                m_fTimeOfLastMsgClientBeginAuthentication = time;
            }
        }
            break;
        default:
            break;
    }
    
    updateState();
}

void NGSteamClientHelper::sendPacket(const OutputMemoryBitStream& inOutputStream)
{
    INetworkHelper::sendPacket(inOutputStream, m_serverSteamAddress);
}

std::string& NGSteamClientHelper::getName()
{
    return m_name;
}

void NGSteamClientHelper::onReceiveServerInfo(CSteamID steamIDGameServer, bool bVACSecure, const char *pchServerName)
{
    m_eConnectedStatus = k_EClientConnectedPendingAuthentication;
    
    m_serverSteamAddress->setSteamID(steamIDGameServer);
    
    // look up the servers IP and Port from the connection
    P2PSessionState_t p2pSessionState;
    SteamNetworking()->GetP2PSessionState(steamIDGameServer, &p2pSessionState);
    m_unServerIP = p2pSessionState.m_nRemoteIP;
    m_usServerPort = p2pSessionState.m_nRemotePort;
    
    // set how to connect to the game server, using the Rich Presence API
    // this lets our friends connect to this game via their friends list
    updateRichPresenceConnectionInfo();
}

void NGSteamClientHelper::updateRichPresenceConnectionInfo()
{
    // connect string that will come back to us on the command line	when a friend tries to join our game
    char rgchConnectString[128];
    rgchConnectString[0] = 0;
    
    if (m_eConnectedStatus == k_EClientConnectedAndAuthenticated && m_unServerIP && m_usServerPort)
    {
        // game server connection method
        StringUtil::sprintf_safe(rgchConnectString, "+connect %d:%d", m_unServerIP, m_usServerPort);
    }
    
    SteamFriends()->SetRichPresence("connect", rgchConnectString);
}

void NGSteamClientHelper::updateState()
{
    m_iState = m_eConnectedStatus == k_EClientConnectedAndAuthenticated ? CLIENT_READY_TO_SAY_HELLO : (m_eConnectedStatus == k_EClientConnectionFailure || m_eConnectedStatus == k_EServerShuttingDown || m_eConnectedStatus == k_EServerIsNotAuthorized) ? CLIENT_AUTH_FAILED : CLIENT_NOT_READY_TO_SAY_HELLO;
}
