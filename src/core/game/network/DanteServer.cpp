//
//  DanteServer.cpp
//  dante
//
//  Created by Stephen Gowen on 6/13/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "DanteServer.h"

#include "DanteClient.h"
#include "Robot.h"

#include "NGSteam.h"
#include "Network.h"
#include "stdlib.h"
#include "time.h"
#include "StringUtil.h"
#include "Timing.h"

#include <math.h>

DanteServer::DanteServer()
{
	m_bConnectedToSteam = false;

	const char *pchGameDir = "projectdante";
	uint32 unIP = INADDR_ANY;
	uint16 usMasterServerUpdaterPort = DANTE_MASTER_SERVER_UPDATER_PORT;

    EServerMode eMode = eServerModeAuthenticationAndSecure;
    
	// Initialize the SteamGameServer interface, we tell it some info about us, and we request support
	// for both Authentication (making sure users own games) and secure mode, VAC running in our game
	// and kicking users who are VAC banned

	// !FIXME! We need a way to pass the dedicated server flag here!

	if (!SteamGameServer_Init(unIP, DANTE_AUTHENTICATION_PORT, DANTE_SERVER_PORT, usMasterServerUpdaterPort, eMode, DANTE_SERVER_VERSION))
	{
		LOG("SteamGameServer_Init call failed");
	}

	if (SteamGameServer())
	{
		// Set the "game dir".
		// This is currently required for all games.  However, soon we will be
		// using the AppID for most purposes, and this string will only be needed
		// for mods.  it may not be changed after the server has logged on
		SteamGameServer()->SetModDir(pchGameDir);

		// These fields are currently required, but will go away soon.
		// See their documentation for more info
		SteamGameServer()->SetProduct("Project Dante");
		SteamGameServer()->SetGameDescription("Project Dante");

		// Initiate Anonymous logon.
		// Coming soon: Logging into authenticated, persistent game server account
		SteamGameServer()->LogOnAnonymous();

		// We want to actively update the master server with our presence so players can
		// find us via the steam matchmaking/server browser interfaces
		SteamGameServer()->EnableHeartbeats(true);
	}
	else
	{
		LOG("SteamGameServer() interface is invalid");
	}

	m_eGameState = k_EServerWaitingForPlayers;

	m_ulStateTransitionTime = m_pGameEngine->Timing::getInstance()->getFrameStartTime();
	m_ulLastServerUpdateTick = 0;

	// zero the client connection data
	memset(&m_rgClientData, 0, sizeof(m_rgClientData));
	memset(&m_rgPendingClientData, 0, sizeof(m_rgPendingClientData));
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
DanteServer::~DanteServer()
{
    // Notify Steam master server we are going offline
    SteamGameServer()->EnableHeartbeats(false);

	for (uint32 i=0; i < MAX_PLAYERS_PER_SERVER; ++i)
	{
		if (m_robots[i])
		{
			// Tell this client we are exiting
			MsgServerExiting_t msg;
			BSendDataToClient(i, (char*)&msg, sizeof(msg));

			delete m_robots[i];
			m_robots[i] = NULL;
		}
	}

	// Disconnect from the steam servers
	SteamGameServer()->LogOff();

	// release our reference to the steam client library
	SteamGameServer_Shutdown();
}

//-----------------------------------------------------------------------------
// Purpose: Handle clients connecting
//-----------------------------------------------------------------------------
void DanteServer::OnP2PSessionRequest(P2PSessionRequest_t *pCallback)
{
	// we'll accept a connection from anyone
	SteamGameServerNetworking()->AcceptP2PSessionWithUser(pCallback->m_steamIDRemote);
}

//-----------------------------------------------------------------------------
// Purpose: Handle clients disconnecting
//-----------------------------------------------------------------------------
void DanteServer::onP2PSessionConnectFail(P2PSessionConnectFail_t *pCallback)
{
	// socket has closed, kick the user associated with it
	for(uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i)
	{
		// If there is no ship, skip
		if (!m_rgClientData[i].m_bActive)
			continue;

		if (m_rgClientData[i].m_SteamIDUser == pCallback->m_steamIDRemote)
		{
			LOG("Disconnected dropped user");
			RemovePlayerFromServer(i);
			break;
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Handle sending data to a client at a given index
//-----------------------------------------------------------------------------
bool DanteServer::BSendDataToClient(uint32 uShipIndex, char *pData, uint32 nSizeOfData)
{
	// Validate index
	if (uShipIndex >= MAX_PLAYERS_PER_SERVER)
		return false;

	if (!SteamGameServerNetworking()->SendP2PPacket(m_rgClientData[uShipIndex].m_SteamIDUser, pData, nSizeOfData, k_EP2PSendUnreliable))
	{
		LOG("Failed sending data to a client");
		return false;
	}
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Handle sending data to a pending client at a given index
//-----------------------------------------------------------------------------
bool DanteServer::BSendDataToPendingClient(uint32 uShipIndex, char *pData, uint32 nSizeOfData)
{
	// Validate index
	if (uShipIndex >= MAX_PLAYERS_PER_SERVER)
		return false;

	if (!SteamGameServerNetworking()->SendP2PPacket(m_rgPendingClientData[uShipIndex].m_SteamIDUser, pData, nSizeOfData, k_EP2PSendReliable))
	{
		LOG("Failed sending data to a pending client");
		return false;
	}
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Handle a new client connecting
//-----------------------------------------------------------------------------
void DanteServer::OnClientBeginAuthentication(CSteamID steamIDClient, void *pToken, uint32 uTokenLen)
{
	// First, check this isn't a duplicate and we already have a user logged on from the same steamid
	for(uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i) 
	{
		if (m_rgClientData[i].m_SteamIDUser == steamIDClient)
		{
			// We already logged them on... (should maybe tell them again incase they don't know?)
			return;
		}
	}

	// Second, do we have room?
	uint32 nPendingOrActivePlayerCount = 0;
	for (uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i)
	{
		if (m_rgPendingClientData[i].m_bActive)
			++nPendingOrActivePlayerCount;
		
		if (m_rgClientData[i].m_bActive)
			++nPendingOrActivePlayerCount;
	}

	// We are full (or will be if the pending players auth), deny new login
	if (nPendingOrActivePlayerCount >=  MAX_PLAYERS_PER_SERVER)
	{
		MsgServerFailAuthentication_t msg;
		SteamGameServerNetworking()->SendP2PPacket(steamIDClient, &msg, sizeof(msg), k_EP2PSendReliable);
	}

	// If we get here there is room, add the player as pending
	for(uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i) 
	{
		if (!m_rgPendingClientData[i].m_bActive)
		{
			m_rgPendingClientData[i].m_ulTickCountLastData = m_pGameEngine->Timing::getInstance()->getFrameStartTime();
            
            // authenticate the user with the Steam back-end servers
            if (k_EBeginAuthSessionResultOK != SteamGameServer()->BeginAuthSession(pToken, uTokenLen, steamIDClient))
            {
                MsgServerFailAuthentication_t msg;
                SteamGameServerNetworking()->SendP2PPacket(steamIDClient, &msg, sizeof(msg), k_EP2PSendReliable);
                break;
            }
            
            m_rgPendingClientData[i].m_SteamIDUser = steamIDClient;
            m_rgPendingClientData[i].m_bActive = true;
            break;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: A new client that connected has had their authentication processed
//-----------------------------------------------------------------------------
void DanteServer::OnAuthCompleted(bool bAuthSuccessful, uint32 iPendingAuthIndex)
{
	if (!m_rgPendingClientData[iPendingAuthIndex].m_bActive)
	{
		LOG("Got auth completed callback for client who is not pending");
		return;
	}

	if (!bAuthSuccessful)
	{
        // Tell the GS the user is leaving the server
        SteamGameServer()->EndAuthSession(m_rgPendingClientData[iPendingAuthIndex].m_SteamIDUser);
        
		// Send a deny for the client, and zero out the pending data
		MsgServerFailAuthentication_t msg;
		SteamGameServerNetworking()->SendP2PPacket(m_rgPendingClientData[iPendingAuthIndex].m_SteamIDUser, &msg, sizeof(msg), k_EP2PSendReliable);
		memset(&m_rgPendingClientData[iPendingAuthIndex], 0, sizeof(ClientConnectionData_t));
		return;
	}


	bool bAddedOk = false;
	for(uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i) 
	{
		if (!m_rgClientData[i].m_bActive)
		{
			// copy over the data from the pending array
			memcpy(&m_rgClientData[i], &m_rgPendingClientData[iPendingAuthIndex], sizeof(ClientConnectionData_t));
			memset(&m_rgPendingClientData[iPendingAuthIndex], 0, sizeof(ClientConnectionData_t	));
			m_rgClientData[i].m_ulTickCountLastData = m_pGameEngine->Timing::getInstance()->getFrameStartTime();

			// Add a new ship, make it dead immediately
			AddPlayerShip(i);
			m_robots[i]->SetDisabled(true);

			MsgServerPassAuthentication_t msg;
			msg.SetPlayerPosition(i);
			BSendDataToClient(i, (char*)&msg, sizeof(msg));

			bAddedOk = true;

			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Removes a player at the given position
//-----------------------------------------------------------------------------
void DanteServer::RemovePlayerFromServer(uint32 uShipPosition)
{
	if (uShipPosition >= MAX_PLAYERS_PER_SERVER)
	{
		LOG("Trying to remove ship at invalid position");
		return;
	}

	if (!m_robots[uShipPosition])
	{
		LOG("Trying to remove a ship that does not exist");
		return;
	}

	LOG("Removing a ship");
	delete m_robots[uShipPosition];
	m_robots[uShipPosition] = NULL;
	m_rguPlayerScores[uShipPosition] = 0;

    // Tell the GS the user is leaving the server
    SteamGameServer()->EndAuthSession(m_rgClientData[uShipPosition].m_SteamIDUser);
    
	memset(&m_rgClientData[uShipPosition], 0, sizeof(ClientConnectionData_t));
}

//-----------------------------------------------------------------------------
// Purpose: Used to transition game state
//-----------------------------------------------------------------------------
void DanteServer::SetGameState(EServerGameState eState)
{
	if (m_eGameState == eState)
		return;

	// If we were in waiting for players and are now going active clear old scores
	if (m_eGameState == k_EServerWaitingForPlayers && eState == k_EServerActive)
	{
		ResetPlayerShips();
	}

	m_ulStateTransitionTime = m_pGameEngine->Timing::getInstance()->getFrameStartTime();
	m_eGameState = eState;
}


//-----------------------------------------------------------------------------
// Purpose: Receives incoming network data
//-----------------------------------------------------------------------------
void DanteServer::ReceiveNetworkData()
{
	char *pchRecvBuf = NULL;
	uint32 cubMsgSize;
	CSteamID steamIDRemote;
	while (SteamGameServerNetworking()->IsP2PPacketAvailable(&cubMsgSize))
	{
		// free any previous receive buffer
		if (pchRecvBuf)
			free(pchRecvBuf);

		// alloc a new receive buffer of the right size
		pchRecvBuf = (char *)malloc(cubMsgSize);

		// see if there is any data waiting on the socket
		if (!SteamGameServerNetworking()->ReadP2PPacket(pchRecvBuf, cubMsgSize, &cubMsgSize, &steamIDRemote))
			break;

		if (cubMsgSize < sizeof(DWORD))
		{
			LOG("Got garbage on server socket, too short");
			continue;
		}

		EMessage eMsg = (EMessage)LittleDWord(*(DWORD*)pchRecvBuf);
		switch (eMsg)
		{
		case k_EMsgClientInitiateConnection:
			{
				// We always let clients do this without even checking for room on the server since we reserve that for 
				// the authentication phase of the connection which comes next
				MsgServerSendInfo_t msg;
				msg.SetSteamIDServer(SteamGameServer()->GetSteamID().ConvertToUint64());
                
                // You can only make use of VAC when using the Steam authentication system
                msg.SetSecure(SteamGameServer()->BSecure());
                
				msg.SetServerName(m_sServerName.c_str());
				SteamGameServerNetworking()->SendP2PPacket(steamIDRemote, &msg, sizeof(MsgServerSendInfo_t), k_EP2PSendReliable);
			}
			break;
		case k_EMsgClientBeginAuthentication:
			{
				if (cubMsgSize != sizeof(MsgClientBeginAuthentication_t))
				{
					LOG("Bad connection attempt msg");
					continue;
				}
				MsgClientBeginAuthentication_t *pMsg = (MsgClientBeginAuthentication_t*)pchRecvBuf;

                OnClientBeginAuthentication(steamIDRemote, (void*)pMsg->GetTokenPtr(), pMsg->GetTokenLen());
			}
			break;
		case k_EMsgClientSendLocalUpdate:
			{
				if (cubMsgSize != sizeof(MsgClientSendLocalUpdate_t))
				{
					LOG("Bad client update msg");
					continue;
				}

				// Find the connection that should exist for this users address
				bool bFound = false;
				for(uint32 i=0; i<MAX_PLAYERS_PER_SERVER; ++i)
				{
					if (m_rgClientData[i].m_SteamIDUser == steamIDRemote) 
					{
						bFound = true;
						MsgClientSendLocalUpdate_t *pMsg = (MsgClientSendLocalUpdate_t*)pchRecvBuf;
						onReceiveClientUpdateData(i, pMsg->AccessUpdateData());
						break;
					}
				}
				if (!bFound)
					LOG("Got a client data update, but couldn't find a matching client");
			}
			break;
		case k_EMsgClientPing:
			{
				// send back a response
				MsgServerPingResponse_t msg;
				SteamGameServerNetworking()->SendP2PPacket(steamIDRemote, &msg, sizeof(msg), k_EP2PSendUnreliable);
			}
			break;
		case k_EMsgClientLeavingServer:
			{
				if (cubMsgSize != sizeof(MsgClientLeavingServer_t))
				{
					LOG("Bad leaving server msg");
					continue;
				}
				// Find the connection that should exist for this users address
				bool bFound = false;
				for(uint32 i=0; i<MAX_PLAYERS_PER_SERVER; ++i)
				{
					if (m_rgClientData[i].m_SteamIDUser == steamIDRemote)
					{
						bFound = true;
						RemovePlayerFromServer(i);
						break;
					}

					// Also check for pending connections that may match
					if (m_rgPendingClientData[i].m_SteamIDUser == steamIDRemote)
					{
                        // Tell the GS the user is leaving the server
                        SteamGameServer()->SendUserDisconnect(m_rgPendingClientData[i].m_SteamIDUser);
                        
						// Clear our data on the user
						memset(&m_rgPendingClientData[i], 0 , sizeof(ClientConnectionData_t));
						break;
					}
				}
				if (!bFound)
					LOG("Got a client leaving server msg, but couldn't find a matching client");
			}
		default:
			char rgch[128];
			StringUtil::sprintf_safe(rgch, "Invalid message %x\n", eMsg);
			rgch[ sizeof(rgch) - 1 ] = 0;
			OutputDebugString(rgch);
		}
	}

	if (pchRecvBuf)
		free(pchRecvBuf);
}

//-----------------------------------------------------------------------------
// Purpose: Main frame function, updates the state of the world and performs rendering
//-----------------------------------------------------------------------------
void DanteServer::RunFrame()
{
	// Run any Steam Game Server API callbacks
	SteamGameServer_RunCallbacks();

	// Update our server details
	SendUpdatedServerDetailsToSteam();

	// Timeout stale player connections, also update player count data
	uint32 uPlayerCount = 0;
	for(uint32 i=0; i < MAX_PLAYERS_PER_SERVER; ++i)
	{
		// If there is no ship, skip
		if (!m_rgClientData[i].m_bActive)
			continue;

		if (m_pGameEngine->Timing::getInstance()->getFrameStartTime() - m_rgClientData[i].m_ulTickCountLastData > SERVER_TIMEOUT_MILLISECONDS)
		{
			LOG("Timing out player connection");
			RemovePlayerFromServer(i);
		}
		else
		{
			++uPlayerCount;
		}
	}

	switch (m_eGameState)
	{
	case k_EServerWaitingForPlayers:
		// Wait a few seconds (so everyone can join if a lobby just started this server)
		if (m_pGameEngine->Timing::getInstance()->getFrameStartTime() - m_ulStateTransitionTime >= MILLISECONDS_BETWEEN_ROUNDS)
		{
			// Just keep waiting until at least one ship is active
			for(uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i)
			{
				if (m_rgClientData[i].m_bActive)
				{
					// Transition to active
					LOG("Server going active after waiting for players");
					SetGameState(k_EServerActive);
				}
			}
		}
		break;
	case k_EServerDraw:
	case k_EServerWinner:
		// Update all the entities...
		m_pSun->RunFrame();
		for(uint32 i=0; i<MAX_PLAYERS_PER_SERVER; ++i)
		{
			if (m_robots[i])
				m_robots[i]->RunFrame();
		}

		// NOTE: no collision detection, because the round is really over, objects are now invulnerable

		// After 5 seconds start the next round
		if (m_pGameEngine->Timing::getInstance()->getFrameStartTime() - m_ulStateTransitionTime >= MILLISECONDS_BETWEEN_ROUNDS)
		{
			ResetPlayerShips();
			SetGameState(k_EServerActive);
		}

		break;

	case k_EServerActive:
		// Update all the entities...
		m_pSun->RunFrame();
		for(uint32 i=0; i<MAX_PLAYERS_PER_SERVER; ++i)
		{
			if (m_robots[i])
				m_robots[i]->RunFrame();
		}

		// Check for collisions which could lead to a winner this round
		CheckForCollisions();

		break;
	case k_EServerExiting:
		break;
	default:
		LOG("Unhandled game state in DanteServer::RunFrame");
	}

	// Send client updates (will internal limit itself to the tick rate desired)
	SendUpdateDataToAllClients();
}


//-----------------------------------------------------------------------------
// Purpose: Sends updates to all connected clients
//-----------------------------------------------------------------------------
void DanteServer::SendUpdateDataToAllClients()
{
	// Limit the rate at which we update, even if our internal frame rate is higher
	if (m_pGameEngine->Timing::getInstance()->getFrameStartTime() - m_ulLastServerUpdateTick < 1000.0f/SERVER_UPDATE_SEND_RATE)
		return;

	m_ulLastServerUpdateTick = m_pGameEngine->Timing::getInstance()->getFrameStartTime();

	MsgServerUpdateWorld_t msg;

	msg.AccessUpdateData()->SetServerGameState(m_eGameState);
	for(int i=0; i<MAX_PLAYERS_PER_SERVER; ++i)
	{
		msg.AccessUpdateData()->SetPlayerActive(i, m_rgClientData[i].m_bActive);
		msg.AccessUpdateData()->SetPlayerScore(i, m_rguPlayerScores[i] );
		msg.AccessUpdateData()->SetPlayerSteamID(i, m_rgClientData[i].m_SteamIDUser.ConvertToUint64());

		if (m_robots[i])
		{
			m_robots[i]->BuildServerUpdate(msg.AccessUpdateData()->AccessShipUpdateData(i));
		}
	}

	msg.AccessUpdateData()->SetPlayerWhoWon(m_uPlayerWhoWonGame);
	
	for(int i=0; i<MAX_PLAYERS_PER_SERVER; ++i)
	{
		if (!m_rgClientData[i].m_bActive) 
			continue;

		BSendDataToClient(i, (char*)&msg, sizeof(msg));
	}
}

//-----------------------------------------------------------------------------
// Purpose: Receives update data from clients
//-----------------------------------------------------------------------------
void DanteServer::onReceiveClientUpdateData(uint32 uShipIndex, ClientDanteUpdateData_t *pUpdateData)
{
	if (m_rgClientData[uShipIndex].m_bActive && m_robots[uShipIndex])
	{
		m_rgClientData[uShipIndex].m_ulTickCountLastData = m_pGameEngine->Timing::getInstance()->getFrameStartTime();
		m_robots[uShipIndex]->onReceiveClientUpdate(pUpdateData);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Take any action we need to on Steam notifying us we are now logged in
//-----------------------------------------------------------------------------
void DanteServer::onSteamServersConnected(SteamServersConnected_t *pLogonSuccess)
{
	LOG("DanteServer connected to Steam successfully");
	m_bConnectedToSteam = true;

	// log on is not finished until OnPolicyResponse() is called

	// Tell Steam about our server details
	SendUpdatedServerDetailsToSteam();
}

//-----------------------------------------------------------------------------
// Purpose: Callback from Steam when logon is fully completed and VAC secure policy is set
//-----------------------------------------------------------------------------
void DanteServer::OnPolicyResponse(GSPolicyResponse_t *pPolicyResponse)
{
    // Check if we were able to go VAC secure or not
    if (SteamGameServer()->BSecure())
    {
        LOG("DanteServer is VAC Secure!");
    }
    else
    {
        LOG("DanteServer is not VAC Secure!");
    }
    
    char rgch[128];
    StringUtil::sprintf_safe(rgch, "Game server SteamID: %llu\n", SteamGameServer()->GetSteamID().ConvertToUint64());
    rgch[sizeof(rgch) - 1] = 0;
    OutputDebugString(rgch);
}

//-----------------------------------------------------------------------------
// Purpose: Called when we were previously logged into steam but get logged out
//-----------------------------------------------------------------------------
void DanteServer::onSteamServersDisconnected(SteamServersDisconnected_t *pLoggedOff)
{
	m_bConnectedToSteam = false;
	LOG("DanteServer got logged out of Steam");
}

//-----------------------------------------------------------------------------
// Purpose: Called when an attempt to login to Steam fails
//-----------------------------------------------------------------------------
void DanteServer::onSteamServersConnectFailure(SteamServerConnectFailure_t *pConnectFailure)
{
	m_bConnectedToSteam = false;
	LOG("DanteServer failed to connect to Steam");
}

//-----------------------------------------------------------------------------
// Purpose: Called once we are connected to Steam to tell it about our details
//-----------------------------------------------------------------------------
void DanteServer::SendUpdatedServerDetailsToSteam()
{
	// Tell the Steam authentication servers about our game
	char rgchServerName[128];
	if (DanteClient())
	{
		// If a client is running (should always be since we don't support a dedicated server)
		// then we'll form the name based off of it
		StringUtil::sprintf_safe(rgchServerName, "%s's game", DanteClient()->GetLocalPlayerName());
	}
	else
	{
		StringUtil::sprintf_safe(rgchServerName, "%s", "Spacewar!");
	}
	m_sServerName = rgchServerName;

	//
	// Set state variables, relevant to any master server updates or client pings
	//

	// These server state variables may be changed at any time.  Note that there is no lnoger a mechanism
	// to send the player count.  The player count is maintained by steam and you should use the player
	// creation/authentication functions to maintain your player count.
	SteamGameServer()->SetMaxPlayerCount(4);
	SteamGameServer()->SetPasswordProtected(false);
	SteamGameServer()->SetServerName(m_sServerName.c_str());

    // Update all the players names/scores
    for (uint32 i=0; i < MAX_PLAYERS_PER_SERVER; ++i)
    {
        if (m_rgClientData[i].m_bActive && m_robots[i])
        {
            SteamGameServer()->BUpdateUserData(m_rgClientData[i].m_SteamIDUser, m_robots[i]->GetPlayerName(), m_rguPlayerScores[i]);
        }
    }
}

//-----------------------------------------------------------------------------
// Purpose: Tells us Steam3 (VAC and newer license checking) has accepted the user connection
//-----------------------------------------------------------------------------
void DanteServer::OnValidateAuthTicketResponse(ValidateAuthTicketResponse_t *pResponse)
{
	if (pResponse->m_eAuthSessionResponse == k_EAuthSessionResponseOK)
	{
		// This is the final approval, and means we should let the client play (find the pending auth by steamid)
		for (uint32 i = 0; i<MAX_PLAYERS_PER_SERVER; ++i)
		{
			if (!m_rgPendingClientData[i].m_bActive)
				continue;
			else if (m_rgPendingClientData[i].m_SteamIDUser == pResponse->m_SteamID)
			{
				LOG("Auth completed for a client");
				OnAuthCompleted(true, i);
				return;
			}
		}
	}
	else
	{
		// Looks like we shouldn't let this user play, kick them
		for (uint32 i = 0; i<MAX_PLAYERS_PER_SERVER; ++i)
		{
			if (!m_rgPendingClientData[i].m_bActive)
            {
                continue;
            }
			else if (m_rgPendingClientData[i].m_SteamIDUser == pResponse->m_SteamID)
			{
				LOG("Auth failed for a client");
				OnAuthCompleted(false, i);
				return;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Returns the SteamID of the game server
//-----------------------------------------------------------------------------
CSteamID DanteServer::GetSteamID()
{
    return SteamGameServer()->GetSteamID();
}

//-----------------------------------------------------------------------------
// Purpose: Kicks a player off the server
//-----------------------------------------------------------------------------
void DanteServer::kickPlayerOffServer(CSteamID steamID)
{
	uint32 uPlayerCount = 0;
	for(uint32 i=0; i < MAX_PLAYERS_PER_SERVER; ++i)
	{
		// If there is no ship, skip
		if (!m_rgClientData[i].m_bActive)
        {
            continue;
        }

		if (m_rgClientData[i].m_SteamIDUser == steamID)
		{
			LOG("Kicking player");
			RemovePlayerFromServer(i);
			// send him a kick message
			MsgServerFailAuthentication_t msg;
			SteamGameServerNetworking()->SendP2PPacket(steamID, &msg, sizeof(msg), k_EP2PSendReliable);
		}
		else
		{
			++uPlayerCount;
		}
	}
}
