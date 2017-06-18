//
//  NGSteamServerHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NGSteamServerHelper.h"

#include "NGSteamAddress.h"

#include "NGSteamPacketHandler.h"
#include "NGSteamGameServices.h"
#include "StringUtil.h"
#include "FrameworkConstants.h"

#include <assert.h>

NGSteamServerHelper::NGSteamServerHelper(const char* inGameDir, const char* inVersionString, const char* inProductName, const char* inGameDescription, uint16 inPort, uint16 inAuthPort, uint16 inUpdaterPort, ProcessPacketFunc inProcessPacketFunc, HandleNoResponseFunc inHandleNoResponseFunc, HandleConnectionResetFunc inHandleConnectionResetFunc) : IServerHelper(new NGSteamPacketHandler(true, inProcessPacketFunc, inHandleNoResponseFunc, inHandleConnectionResetFunc)),
m_inGameDir(inGameDir),
m_serverSteamAddress(nullptr),
m_isConnectedToSteam(false)
{
    assert(NG_STEAM_GAME_SERVICES->init() == STEAM_INIT_SUCCESS);
    
    // Initialize the SteamGameServer interface, we tell it some info about us, and we request support
    // for both Authentication (making sure users own games) and secure mode, VAC running in our game
    // and kicking users who are VAC banned
    
    if (!SteamGameServer_Init(INADDR_ANY, inAuthPort, inPort, inUpdaterPort, eServerModeAuthenticationAndSecure, inVersionString))
    {
        LOG("SteamGameServer_Init call failed");
    }
    
    if (SteamGameServer())
    {
        // Set the "game dir".
        // This is currently required for all games.  However, soon we will be
        // using the AppID for most purposes, and this string will only be needed
        // for mods.  it may not be changed after the server has logged on
        SteamGameServer()->SetModDir(m_inGameDir);
        
        // These fields are currently required, but will go away soon.
        // See their documentation for more info
        SteamGameServer()->SetProduct(inProductName);
        SteamGameServer()->SetGameDescription(inGameDescription);
        
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
        
        m_serverSteamAddress = new NGSteamAddress(SteamGameServer()->GetSteamID());
        
        LOG("SteamGameServer() interface is valid");
    }
    else
    {
        LOG("SteamGameServer() interface is invalid");
    }
}

NGSteamServerHelper::~NGSteamServerHelper()
{
    if (m_serverSteamAddress)
    {
        delete m_serverSteamAddress;
    }
    
    // Notify Steam master server we are going offline
    SteamGameServer()->EnableHeartbeats(false);
    
    // Disconnect from the steam servers
    SteamGameServer()->LogOff();
    
    // release our reference to the steam client library
    SteamGameServer_Shutdown();
    
    NG_STEAM_GAME_SERVICES->deinit();
}

void NGSteamServerHelper::processIncomingPackets()
{
    INetworkHelper::processIncomingPackets();
    
    // Run any Steam Game Server API callbacks
    SteamGameServer_RunCallbacks();
    
    // Update our server details
    sendUpdatedServerDetailsToSteam();
}

IMachineAddress* NGSteamServerHelper::getServerAddress()
{
    return m_serverSteamAddress;
}

bool NGSteamServerHelper::isConnected()
{
    return m_isConnectedToSteam;
}

void NGSteamServerHelper::sendUpdatedServerDetailsToSteam()
{
    // Tell the Steam authentication servers about our game
    char rgchServerName[128];
    // If a client is running (should always be since we don't support a dedicated server)
    // then we'll form the name based off of it
    StringUtil::sprintf_safe(rgchServerName, "%s's game", SteamFriends()->GetPersonaName());
    m_serverName = std::string(rgchServerName);
    
    // These server state variables may be changed at any time. Note that there is no lnoger a mechanism
    // to send the player count. The player count is maintained by steam and you should use the player
    // creation/authentication functions to maintain your player count.
    SteamGameServer()->SetMaxPlayerCount(MAX_NUM_PLAYERS_PER_SERVER);
    SteamGameServer()->SetPasswordProtected(false); // TODO, will definitely have to revisit this
    SteamGameServer()->SetServerName(m_inGameDir);
    SteamGameServer()->SetBotPlayerCount(0); // optional, defaults to zero
    SteamGameServer()->SetMapName("Wasteland"); // TODO, do we need to specify a map name?
    
    // Update all the players names/scores
    for (uint32 i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        // TODO
//        if (m_rgClientData[i].m_bActive && m_rgpShips[i])
//        {
//            SteamGameServer()->BUpdateUserData(m_rgClientData[i].m_SteamIDUser, m_rgpShips[i]->GetPlayerName(), m_rguPlayerScores[i]);
//        }
    }
}

//-----------------------------------------------------------------------------
// Purpose: A new client that connected has had their authentication processed
//-----------------------------------------------------------------------------
void NGSteamServerHelper::onAuthCompleted(bool bAuthSuccessful, uint32 iPendingAuthIndex)
{
    LOG("onAuthCompleted");
    
    // TODO
//    if (!m_rgPendingClientData[iPendingAuthIndex].m_bActive)
//    {
//        LOG("Got auth completed callback for client who is not pending");
//        return;
//    }
//    
//    if (!bAuthSuccessful)
//    {
//        // Tell the GS the user is leaving the server
//        SteamGameServer()->EndAuthSession(m_rgPendingClientData[iPendingAuthIndex].m_SteamIDUser);
//        
//        // Send a deny for the client, and zero out the pending data
//        MsgServerFailAuthentication_t msg;
//        SteamGameServerNetworking()->SendP2PPacket(m_rgPendingClientData[iPendingAuthIndex].m_SteamIDUser, &msg, sizeof(msg), k_EP2PSendReliable);
//        memset(&m_rgPendingClientData[iPendingAuthIndex], 0, sizeof(ClientConnectionData_t));
//        return;
//    }
//    
//    bool bAddedOk = false;
//    for(uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i)
//    {
//        if (!m_rgClientData[i].m_bActive)
//        {
//            // copy over the data from the pending array
//            memcpy(&m_rgClientData[i], &m_rgPendingClientData[iPendingAuthIndex], sizeof(ClientConnectionData_t));
//            memset(&m_rgPendingClientData[iPendingAuthIndex], 0, sizeof(ClientConnectionData_t	));
//            m_rgClientData[i].m_ulTickCountLastData = m_pGameEngine->GetGameTickCount();
//            
//            // Add a new ship, make it dead immediately
//            AddPlayerShip(i);
//            m_rgpShips[i]->SetDisabled(true);
//            
//            MsgServerPassAuthentication_t msg;
//            msg.SetPlayerPosition(i);
//            BSendDataToClient(i, (char*)&msg, sizeof(msg));
//            
//            bAddedOk = true;
//            
//            break;
//        }
//    }
//    
    // If we just successfully added the player, check if they are #2 so we can restart the round
//    if (bAddedOk)
//    {
//        uint32 uPlayers = 0;
//        for (uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i)
//        {
//            if (m_rgClientData[i].m_bActive)
//            {
//                ++uPlayers;
//            }
//        }
//        
//        // If we just got the second player, immediately reset round as a draw.  This will prevent
//        // the existing player getting a win, and it will cause a new round to start right off
//        // so that the one player can't just float around not letting the new one get into the game.
//        if (uPlayers == 2)
//        {
//            if (m_eGameState != k_EServerWaitingForPlayers)
//            {
//                SetGameState(k_EServerDraw);
//            }
//        }
//    }
}

//-----------------------------------------------------------------------------
// Purpose: Take any action we need to on Steam notifying us we are now logged in
//-----------------------------------------------------------------------------
void NGSteamServerHelper::onSteamServersConnected(SteamServersConnected_t *pLogonSuccess)
{
    LOG("%s connected to Steam successfully", m_inGameDir);
    m_isConnectedToSteam = true;
    
    // log on is not finished until OnPolicyResponse() is called
    
    // Tell Steam about our server details
    sendUpdatedServerDetailsToSteam();
}

void NGSteamServerHelper::onSteamServersConnectFailure(SteamServerConnectFailure_t *pConnectFailure)
{
    LOG("%s failed to connect to Steam", m_inGameDir);
    
    m_isConnectedToSteam = false;
}

void NGSteamServerHelper::onSteamServersDisconnected(SteamServersDisconnected_t *pLoggedOff)
{
    LOG("%s got logged out of Steam", m_inGameDir);
    
    m_isConnectedToSteam = false;
}

void NGSteamServerHelper::onPolicyResponse(GSPolicyResponse_t *pPolicyResponse)
{
    // Check if we were able to go VAC secure or not
    if (SteamGameServer()->BSecure())
    {
        LOG("%s is VAC Secure!", m_inGameDir);
    }
    else
    {
        LOG("%s is not VAC Secure!", m_inGameDir);
    }
    
    char rgch[128];
    StringUtil::sprintf_safe(rgch, "Game server SteamID: %llu\n", SteamGameServer()->GetSteamID().ConvertToUint64());
    rgch[ sizeof(rgch) - 1 ] = 0;
    LOG(rgch);
}

void NGSteamServerHelper::onValidateAuthTicketResponse(ValidateAuthTicketResponse_t *pResponse)
{
    LOG("onValidateAuthTicketResponse");
    
    if (pResponse->m_eAuthSessionResponse == k_EAuthSessionResponseOK)
    {
        // This is the final approval, and means we should let the client play (find the pending auth by steamid)
        for (uint32 i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
        {
//            if (!m_rgPendingClientData[i].m_bActive)
//            {
//                continue;
//            }
//            else if (m_rgPendingClientData[i].m_SteamIDUser == pResponse->m_SteamID)
//            {
//                LOG("Auth completed for a client");
//                onAuthCompleted(true, i);
//                return;
//            }
        }
    }
    else
    {
        // Looks like we shouldn't let this user play, kick them
        for (uint32 i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
        {
//            if (!m_rgPendingClientData[i].m_bActive)
//            {
//                continue;
//            }
//            else if (m_rgPendingClientData[i].m_SteamIDUser == pResponse->m_SteamID)
//            {
//                LOG("Auth failed for a client");
//                onAuthCompleted(false, i);
//                return;
//            }
        }
    }
}

void NGSteamServerHelper::onP2PSessionRequest(P2PSessionRequest_t *pCallback)
{
    LOG("onP2PSessionRequest");
    
    // we'll accept a connection from anyone
    SteamGameServerNetworking()->AcceptP2PSessionWithUser(pCallback->m_steamIDRemote);
}

void NGSteamServerHelper::onP2PSessionConnectFail(P2PSessionConnectFail_t *pCallback)
{
    LOG("onP2PSessionConnectFail");
    
    // socket has closed, kick the user associated with it
    for (uint32 i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        // If there is no ship, skip
//        if (!m_rgClientData[i].m_bActive)
//        {
//            continue;
//        }
//        
//        if (m_rgClientData[i].m_SteamIDUser == pCallback->m_steamIDRemote)
//        {
//            LOG("Disconnected dropped user");
//            RemovePlayerFromServer(i);
//            break;
//        }
    }
}
