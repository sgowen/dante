//
//  DanteClient.cpp
//  dante
//
//  Created by Stephen Gowen on 6/13/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "DanteClient.h"

#include "DanteServer.h"
#include "Robot.h"
#include "P2PAuth.h"

#include "NGSteam.h"
#include "NGSteamGameServices.h"
#include "stdlib.h"
#include "time.h"
#include "StringUtil.h"
#include "Timing.h"

DanteClient::DanteClient()
{
	init();
}

DanteClient::~DanteClient()
{
    disconnectFromServer();
    
    if (m_pP2PAuthedGame)
    {
        m_pP2PAuthedGame->endGame();
        
        delete m_pP2PAuthedGame;
        m_pP2PAuthedGame = nullptr;
    }
    
    if (m_pServer)
    {
        delete m_pServer;
        m_pServer = nullptr;
    }
    
    for (uint32 i = 0; i < MAX_PLAYERS_PER_SERVER; ++i)
    {
        if (m_robots[i])
        {
            delete m_robots[i];
            m_robots[i] = nullptr;
        }
    }
}

void DanteClient::init()
{
	// On PC/OSX we always know the user has a SteamID and is logged in already,
	// as Steam enforces this before game launch.  On PS3 however the game must
	// initiate the logon and we need to check the state here and block the user
	// while Steam connects.
	if (SteamUser()->BLoggedOn())
	{
		m_SteamIDLocalUser = SteamUser()->GetSteamID();
		m_eGameState = k_EClientGameMenu;
	}

    m_ulStateTransitionTime = Timing::getInstance()->getFrameStartTime();
	m_ulLastNetworkDataReceivedTime = 0;
	m_pServer = NULL;
	m_uPlayerIndex = 0;
	m_eConnectedStatus = k_EClientNotConnected;
	m_bTransitionedGameState = true;
	m_unServerIP = 0;
	m_usServerPort = 0;
	m_ulPingSentTime = 0;

	// initialize P2P auth engine
    m_pP2PAuthedGame = new CP2PAuthedGame();
}

void DanteClient::disconnectFromServer()
{
	if (m_eConnectedStatus != k_EClientNotConnected)
	{
        if (m_hAuthTicket != k_HAuthTicketInvalid)
        {
            SteamUser()->CancelAuthTicket(m_hAuthTicket);
        }
        
        m_hAuthTicket = k_HAuthTicketInvalid;

		MsgClientLeavingServer_t msg;
		BSendServerData(&msg, sizeof(msg));
		m_eConnectedStatus = k_EClientNotConnected;
	}
    
	if (m_pP2PAuthedGame)
	{
		m_pP2PAuthedGame->endGame();
	}

	// forget the game server ID
	if (m_steamIDGameServer.IsValid())
	{
		SteamNetworking()->CloseP2PSessionWithUser(m_steamIDGameServer);
		m_steamIDGameServer = CSteamID();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Receive basic server info from the server after we initiate a connection
//-----------------------------------------------------------------------------
void DanteClient::onReceiveServerInfo(CSteamID steamIDGameServer, bool bVACSecure, const char *pchServerName)
{
	m_eConnectedStatus = k_EClientConnectedPendingAuthentication;
	m_steamIDGameServer = steamIDGameServer;

	// look up the servers IP and Port from the connection
	P2PSessionState_t p2pSessionState;
	SteamNetworking()->GetP2PSessionState(steamIDGameServer, &p2pSessionState);
	m_unServerIP = p2pSessionState.m_nRemoteIP;
	m_usServerPort = p2pSessionState.m_nRemotePort;

	// set how to connect to the game server, using the Rich Presence API
	// this lets our friends connect to this game via their friends list
	UpdateRichPresenceConnectionInfo();

    char rgchToken[1024];
    uint32 unTokenLen = 0;
    m_hAuthTicket = SteamUser()->GetAuthSessionTicket(rgchToken, sizeof(rgchToken), &unTokenLen);

	Steamworks_TestSecret();

	if (unTokenLen < 1)
    {
        LOG("Warning: Looks like GetAuthSessionTicket didn't give us a good ticket");
    }

	BSendServerData(&msg, sizeof(msg));
}

//-----------------------------------------------------------------------------
// Purpose: Receive an authentication response from the server
//-----------------------------------------------------------------------------
void DanteClient::onReceiveServerAuthenticationResponse(bool bSuccess, uint32 uPlayerPosition)
{
	if (!bSuccess)
	{
		LOG("Connection failure.\nMultiplayer authentication failed");
		SetGameState(k_EClientGameConnectionFailure);
		disconnectFromServer();
	}
	else
	{
		// Is this a duplicate message? If so ignore it...
		if (m_eConnectedStatus == k_EClientConnectedAndAuthenticated && m_uPlayerIndex == uPlayerPosition)
			return;

		m_uPlayerIndex = uPlayerPosition;
		m_eConnectedStatus = k_EClientConnectedAndAuthenticated;

		// set information so our friends can join the lobby
		UpdateRichPresenceConnectionInfo();

		// send a ping, to measure round-trip time
		m_ulPingSentTime = m_pGameEngine->Timing::getInstance()->getFrameStartTime();
		MsgClientPing_t msg;
		BSendServerData(&msg, sizeof(msg));
	}
}

//-----------------------------------------------------------------------------
// Purpose: Handles receiving a state update from the game server
//-----------------------------------------------------------------------------
void DanteClient::onReceiveServerUpdate(ServerDanteUpdateData_t *pUpdateData)
{
	// Update our client state based on what the server tells us
	
	switch(pUpdateData->GetServerGameState())
	{
	case k_EServerWaitingForPlayers:
		if (m_eGameState == k_EClientGameQuitMenu)
			break;
		else if (m_eGameState == k_EClientGameMenu)
			break;
		else if (m_eGameState == k_EClientGameExiting)
			break;

		SetGameState(k_EClientGameWaitingForPlayers);
		break;
	case k_EServerActive:
		if (m_eGameState == k_EClientGameQuitMenu)
			break;
		else if (m_eGameState == k_EClientGameMenu)
			break;
		else if (m_eGameState == k_EClientGameExiting)
			break;

		SetGameState(k_EClientGameActive);
		break;
	case k_EServerExiting:
		if (m_eGameState == k_EClientGameExiting)
			break;

		SetGameState(k_EClientGameMenu);
		break;
	}

	// Update scores
	for(int i=0; i < MAX_PLAYERS_PER_SERVER; ++i)
	{
		m_rguPlayerScores[i] = pUpdateData->GetPlayerScore(i);
	}

	// Update who won last
	m_uPlayerWhoWonGame = pUpdateData->GetPlayerWhoWon();

	if (m_pP2PAuthedGame)
	{
		// has the player list changed?
		if (m_pServer)
		{
			// if i am the server owner i need to auth everyone who wants to play
			// assume i am in slot 0, so start at slot 1
			for(uint32 i=1; i < MAX_PLAYERS_PER_SERVER; ++i)
			{
				CSteamID steamIDNew(pUpdateData->GetPlayerSteamID(i));
				if (steamIDNew == SteamUser()->GetSteamID())
				{
					LOG("Server player slot 0 is not server owner.");
				}
				else if (steamIDNew != m_rgSteamIDPlayers[i])
				{
					if (m_rgSteamIDPlayers[i].IsValid())
					{
						m_pP2PAuthedGame->PlayerDisconnect(i);
					}
					if (steamIDNew.IsValid())
					{
						m_pP2PAuthedGame->RegisterPlayer(i, steamIDNew);
					}
				}
			}
		}
		else
		{
			// i am just a client, i need to auth the game owner (slot 0)
			CSteamID steamIDNew(pUpdateData->GetPlayerSteamID(0));
			if (steamIDNew == SteamUser()->GetSteamID())
			{
				LOG("Server player slot 0 is not server owner.");
			}
			else if (steamIDNew != m_rgSteamIDPlayers[0])
			{
				if (m_rgSteamIDPlayers[0].IsValid())
				{
					LOG("Server player slot 0 has disconnected - but thats the server owner.");
					m_pP2PAuthedGame->PlayerDisconnect(0);
				}
				if (steamIDNew.IsValid())
				{
					m_pP2PAuthedGame->StartAuthPlayer(0, steamIDNew);
				}
			}
		}
	}

	// Update the players
	for(uint32 i=0; i < MAX_PLAYERS_PER_SERVER; ++i)
	{
		// Update steamid array with data from server
		m_rgSteamIDPlayers[i].SetFromUint64(pUpdateData->GetPlayerSteamID(i));

		if (pUpdateData->GetPlayerActive(i))
		{
			// Check if we have a ship created locally for this player slot, if not create it
			if (!m_robots[i])
			{
				ServerShipUpdateData_t *pShipData = pUpdateData->AccessShipUpdateData(i);
				m_robots[i] = new CShip(m_pGameEngine, false, pShipData->GetXPosition(), pShipData->GetYPosition(), g_rgPlayerColors[i]);
				if (i == m_uPlayerIndex)
				{
					// If this is our local ship, then setup key bindings appropriately
					m_robots[i]->SetVKBindingLeft(0x41); // A key
					m_robots[i]->SetVKBindingRight(0x44); // D key
					m_robots[i]->SetVKBindingForwardThrusters(0x57); // W key
					m_robots[i]->SetVKBindingReverseThrusters(0x53); // S key
					m_robots[i]->SetVKBindingFire(VK_SPACE); 
				}
			}

			if (i == m_uPlayerIndex)
				m_robots[i]->SetIsLocalPlayer(true);
			else
				m_robots[i]->SetIsLocalPlayer(false);

			m_robots[i]->onReceiveServerUpdate(pUpdateData->AccessShipUpdateData(i));
		}
		else
		{
			// Make sure we don't have a ship locally for this slot
			if (m_robots[i])
			{
				delete m_robots[i];
				m_robots[i] = NULL;
			}
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Used to transition game state
//-----------------------------------------------------------------------------
void DanteClient::SetGameState(EClientGameState eState)
{
	if (m_eGameState == eState)
		return;

	m_bTransitionedGameState = true;
	m_ulStateTransitionTime = m_pGameEngine->Timing::getInstance()->getFrameStartTime();
	m_eGameState = eState;

	// Let the stats handler check the state (so it can detect wins, losses, etc...)
	m_pStatsAndAchievements->OnGameStateChange(eState);

	// update any rich presence state
	UpdateRichPresenceConnectionInfo();
}

//-----------------------------------------------------------------------------
// Purpose: Send data to the current server
//-----------------------------------------------------------------------------
bool DanteClient::BSendServerData(const void *pData, uint32 nSizeOfData)
{
	if (!SteamNetworking()->SendP2PPacket(m_steamIDGameServer, pData, nSizeOfData, k_EP2PSendUnreliable))
	{
		LOG("Failed sending data to server");
		return false;
	}
	return true;
}

void DanteClient::initiateServerConnection(uint32 unServerAddress, const int32 nPort)
{
    SetGameState(k_EClientGameConnecting);

	// Update when we last retried the connection, as well as the last packet received time so we won't timeout too soon,
	// and so we will retry at appropriate intervals if packets drop
	m_ulLastNetworkDataReceivedTime = m_ulLastConnectionAttemptRetryTime = m_pGameEngine->Timing::getInstance()->getFrameStartTime();

	// ping the server to find out what it's steamID is
	m_unServerIP = unServerAddress;
	m_usServerPort = (uint16)nPort;
	m_GameServerPing.retrieveSteamIDFromGameServer(this, m_unServerIP, m_usServerPort);
}

void DanteClient::initiateServerConnection(CSteamID steamIDGameServer)
{
    SetGameState(k_EClientGameConnecting);

	m_steamIDGameServer = steamIDGameServer;

	// Update when we last retried the connection, as well as the last packet received time so we won't timeout too soon,
	// and so we will retry at appropriate intervals if packets drop
	m_ulLastNetworkDataReceivedTime = m_ulLastConnectionAttemptRetryTime = m_pGameEngine->Timing::getInstance()->getFrameStartTime();

	// send the packet to the server
	MsgClientInitiateConnection_t msg;
	BSendServerData(&msg, sizeof(msg));
}

//-----------------------------------------------------------------------------
// Purpose: Receives incoming network data
//-----------------------------------------------------------------------------
void DanteClient::ReceiveNetworkData()
{
	char rgchRecvBuf[1024];
	char *pchRecvBuf = rgchRecvBuf;
	uint32 cubMsgSize;
	for (;;)
	{
		// reset the receive buffer
		if (pchRecvBuf != rgchRecvBuf)
		{
			free(pchRecvBuf);
			pchRecvBuf = rgchRecvBuf;
		}

		// see if there is any data waiting on the socket
		if (!SteamNetworking()->IsP2PPacketAvailable(&cubMsgSize))
			break;

		// not enough space in default buffer
		// alloc custom size and try again
		if (cubMsgSize > sizeof(rgchRecvBuf))
		{
			pchRecvBuf = (char *)malloc(cubMsgSize);
		}
		CSteamID steamIDRemote;
		if (!SteamNetworking()->ReadP2PPacket(pchRecvBuf, cubMsgSize, &cubMsgSize, &steamIDRemote))
			break;

		// see if it's from the game server
		if (steamIDRemote == m_steamIDGameServer)
		{
			m_ulLastNetworkDataReceivedTime = m_pGameEngine->Timing::getInstance()->getFrameStartTime();

			// make sure we're connected
			if (m_eConnectedStatus == k_EClientNotConnected && m_eGameState != k_EClientGameConnecting)
				continue;

			if (cubMsgSize < sizeof(DWORD))
			{
				LOG("Got garbage on client socket, too short");
			}

			EMessage eMsg = (EMessage)LittleDWord(*(DWORD*)pchRecvBuf);
			switch (eMsg)
			{
			case k_EMsgServerSendInfo:
				{
					if (cubMsgSize != sizeof(MsgServerSendInfo_t))
					{
						OutputDebugString ("Bad server info msg");
						continue;
					}
					MsgServerSendInfo_t *pMsg = (MsgServerSendInfo_t*)pchRecvBuf;

					// pull the IP address of the user from the socket
					onReceiveServerInfo(CSteamID(pMsg->GetSteamIDServer()), pMsg->GetSecure(), pMsg->GetServerName());
				}
				break;
			case k_EMsgServerPassAuthentication:
				{
					if (cubMsgSize != sizeof(MsgServerPassAuthentication_t))
					{
						LOG("Bad accept connection msg");
						continue;
					}
					MsgServerPassAuthentication_t *pMsg = (MsgServerPassAuthentication_t*)pchRecvBuf;

					// Our game client doesn't really care about whether the server is secure, or what its 
					// steamID is, but if it did we would pass them in here as they are part of the accept message
					onReceiveServerAuthenticationResponse(true, pMsg->GetPlayerPosition());
				}
				break;
			case k_EMsgServerFailAuthentication:
				{
					onReceiveServerAuthenticationResponse(false, 0);
				}
				break;
			case k_EMsgServerUpdateWorld:
				{
					if (cubMsgSize != sizeof(MsgServerUpdateWorld_t))
					{
						LOG("Bad server world update msg");
						continue;
					}

					MsgServerUpdateWorld_t *pMsg = (MsgServerUpdateWorld_t*)pchRecvBuf;
					onReceiveServerUpdate(pMsg->AccessUpdateData());
				}
				break;
			case k_EMsgServerExiting:
				{
					if (cubMsgSize != sizeof(MsgServerExiting_t))
					{
						LOG("Bad server exiting msg");
					}
					onReceiveServerExiting();
				}
				break;
			case k_EMsgServerPingResponse:
				{
					uint64 ulTimePassedMS = m_pGameEngine->Timing::getInstance()->getFrameStartTime() - m_ulPingSentTime;
					char rgchT[256];
					StringUtil::sprintf_safe(rgchT, "Round-trip ping time to server %d ms\n", (int)ulTimePassedMS);
					rgchT[ sizeof(rgchT) - 1 ] = 0;
					OutputDebugString(rgchT);
					m_ulPingSentTime = 0;
				}
				break;
			default:
				LOG("Unhandled message from server");
				break;
			}
		}
		else 
		{
			// the message is from another player
			EMessage eMsg = (EMessage)LittleDWord(*(DWORD*)pchRecvBuf);

			if (m_pP2PAuthedGame->HandleMessage(eMsg, pchRecvBuf))
				continue; // this was a P2P auth message

			// Unhandled message
			LOG("Received unknown message on our listen socket");
		}
	}

	// if we're running a server, do that as well
	if (m_pServer)
	{
		m_pServer->ReceiveNetworkData();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Handle the server telling us it is exiting
//-----------------------------------------------------------------------------
void DanteClient::onReceiveServerExiting()
{
	if (m_pP2PAuthedGame)
    {
        m_pP2PAuthedGame->endGame();
    }

    if (m_hAuthTicket != k_HAuthTicketInvalid)
    {
        SteamUser()->CancelAuthTicket(m_hAuthTicket);
    }
    
    m_hAuthTicket = k_HAuthTicketInvalid;

	if (m_eGameState != k_EClientGameActive)
    {
        return;
    }
	
    m_eConnectedStatus = k_EClientNotConnected;

	LOG("Game server has exited.");
	SetGameState(k_EClientGameConnectionFailure);
}

//-----------------------------------------------------------------------------
// Purpose: does work on transitioning from one game state to another
//-----------------------------------------------------------------------------
void DanteClient::onGameStateChanged(EClientGameState eGameStateNew)
{
	if (m_eGameState == k_EClientFindInternetServers)
	{
		// If we are just opening the find servers screen, then start a refresh
		m_pServerBrowser->RefreshInternetServers();
		SteamFriends()->SetRichPresence("status", "Finding an internet game");
	}
	else if (m_eGameState == k_EClientFindLANServers)
	{
		m_pServerBrowser->RefreshLANServers();
		SteamFriends()->SetRichPresence("status", "Finding a LAN game");
	}
	else if (m_eGameState == k_EClientGameMenu)
	{
		// we've switched out to the main menu

		// Tell the server we have left if we are connected
		disconnectFromServer();

		// shut down any server we were running
		if (m_pServer)
		{
			delete m_pServer;
			m_pServer = NULL;
		}

		SteamFriends()->SetRichPresence("status", "Terminal");
	}
	else if (m_eGameState == k_EClientGameActive)
	{
		SteamFriends()->SetRichPresence("status", "Playing");
	}
}

//-----------------------------------------------------------------------------
// Purpose: Main frame function, updates the state of the world and performs rendering
//-----------------------------------------------------------------------------
void DanteClient::RunFrame()
{
    // Get any new data off the network to begin with
    ReceiveNetworkData();
    
    if (m_eConnectedStatus != k_EClientNotConnected && m_pGameEngine->Timing::getInstance()->getFrameStartTime() - m_ulLastNetworkDataReceivedTime > MILLISECONDS_CONNECTION_TIMEOUT)
    {
        LOG("Game server connection failure.");
        disconnectFromServer(); // cleanup on our side, even though server won't get our disconnect msg
        SetGameState(k_EClientGameConnectionFailure);
    }
    
    // Check if escape has been pressed, we'll use that info in a couple places below
    bool bEscapePressed = false;
    if (m_pGameEngine->BIsKeyDown(VK_ESCAPE) ||
        m_pGameEngine->BIsControllerActionActive(eControllerDigitalAction_PauseMenu))
    {
        static uint64 m_ulLastESCKeyTick = 0;
        uint64 ulCurrentTickCount = m_pGameEngine->Timing::getInstance()->getFrameStartTime();
        if (ulCurrentTickCount - 250 > m_ulLastESCKeyTick)
        {
            m_ulLastESCKeyTick = ulCurrentTickCount;
            bEscapePressed = true;
        }
    }
    
    NG_STEAM_GAME_SERVICES->update();
    
    // if we just transitioned state, perform on change handlers
    if (m_bTransitionedGameState)
    {
        m_bTransitionedGameState = false;
        onGameStateChanged(m_eGameState);
    }
    
    // Update state for everything
    switch (m_eGameState)
    {
        case k_EClientConnectingToSteam:
            m_pStarField->Render();
            m_pConnectingMenu->RunFrame();
            break;
        case k_EClientGameMenu:
            m_pStarField->Render();
            m_pMainMenu->RunFrame();
            break;
        case k_EClientFindInternetServers:
        case k_EClientFindLANServers:
            m_pStarField->Render();
            m_pServerBrowser->RunFrame();
            break;
        case k_EClientGameConnectionFailure:
            m_pStarField->Render();
            DrawConnectionFailureText();
            
            if (bEscapePressed)
            {
                SetGameState(k_EClientGameMenu);
            }
            
            break;
        case k_EClientGameConnecting:
            m_pStarField->Render();
            
            // Draw text telling the user a connection attempt is in progress
            DrawConnectionAttemptText();
            
            // Check if we've waited too long and should time out the connection
            if (m_pGameEngine->Timing::getInstance()->getFrameStartTime() - m_ulStateTransitionTime > MILLISECONDS_CONNECTION_TIMEOUT)
            {
                if (m_pP2PAuthedGame)
                {
                    m_pP2PAuthedGame->endGame();
                }
                
                if (m_eConnectedStatus == k_EClientConnectedAndAuthenticated)
                {
                    SteamUser()->TerminateGameConnection(m_unServerIP, m_usServerPort);
                }
                
                m_GameServerPing.cancelPing();
                LOG("Timed out connecting to game server");
                SetGameState(k_EClientGameConnectionFailure);
            }
            
            break;
        case k_EClientGameStartServer:
            m_pStarField->Render();
            if (!m_pServer)
            {
                m_pServer = new DanteServer(m_pGameEngine);
            }
            
            if (m_pServer && m_pServer->IsConnectedToSteam())
            {
                // server is ready, connect to it
                initiateServerConnection(m_pServer->GetSteamID());
            }
            break;
        case k_EClientGameWaitingForPlayers:
            m_pStarField->Render();
            
            // Update all the entities (this is client side interpolation)...
            m_pSun->RunFrame();
            for (uint32 i=0; i<MAX_PLAYERS_PER_SERVER; ++i)
            {
                if (m_robots[i])
                {
                    m_robots[i]->RunFrame();
                }
            }
            
            if (bEscapePressed)
            {
                SetGameState(k_EClientGameQuitMenu);
            }
            
            break;
            
        case k_EClientGameActive:
            // Update all the entities...
            
            if (bEscapePressed)
            {
                SetGameState(k_EClientGameQuitMenu);
            }
            
            break;
        case k_EClientGameExiting:
            disconnectFromServer();
            m_pGameEngine->Shutdown();
            return;
        default:
            LOG("Unhandled game state in CDante::RunFrame");
    }
    
    
    // Send an update on our local ship to the server
    if (m_eConnectedStatus == k_EClientConnectedAndAuthenticated &&  m_robots[ m_uPlayerIndex ])
    {
        MsgClientSendLocalUpdate_t msg;
        msg.SetShipPosition(m_uPlayerIndex);
        
        // If this fails, it probably just means its not time to send an update yet
        if (m_robots[ m_uPlayerIndex ]->BGetClientUpdateData(msg.AccessUpdateData()))
            BSendServerData(&msg, sizeof(msg));
    }
    
    if (m_pP2PAuthedGame)
    {
        if (m_pServer)
        {
            // Now if we are the owner of the game, lets make sure all of our players are legit.
            // if they are not, we tell the server to kick them off
            // Start at 1 to skip myself
            for (int i = 1; i < MAX_PLAYERS_PER_SERVER; i++)
            {
                if (m_pP2PAuthedGame->m_rgpP2PAuthPlayer[i] && !m_pP2PAuthedGame->m_rgpP2PAuthPlayer[i]->BIsAuthOk())
                {
                    m_pServer->kickPlayerOffServer(m_pP2PAuthedGame->m_rgpP2PAuthPlayer[i]->m_steamID);
                }
            }
        }
        else
        {
            // If we are not the owner of the game, lets make sure the game owner is legit
            // if he is not, we leave the game
            if (m_pP2PAuthedGame->m_rgpP2PAuthPlayer[0])
            {
                if (!m_pP2PAuthedGame->m_rgpP2PAuthPlayer[0]->BIsAuthOk())
                {
                    // leave the game
                    SetGameState(k_EClientGameMenu);
                }
            }
        }
    }
    
    // If we've started a local server run it
    if (m_pServer)
    {
        m_pServer->RunFrame();
    }
}

//-----------------------------------------------------------------------------
// Purpose: Updates what we show to friends about what we're doing and how to connect
//-----------------------------------------------------------------------------
void DanteClient::UpdateRichPresenceConnectionInfo()
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

void DanteClient::connectToServer(const char *serverAddress)
{
    if (serverAddress)
    {
        int32 octet0 = 0, octet1 = 0, octet2 = 0, octet3 = 0;
        int32 uPort = 0;
        int nConverted = sscanf(serverAddress, "%d.%d.%d.%d:%d", &octet0, &octet1, &octet2, &octet3, &uPort);
        
        if (nConverted == 5)
        {
            char rgchIPAddress[128];
            StringUtil::sprintf_safe(rgchIPAddress, "%d.%d.%d.%d", octet0, octet1, octet2, octet3);
            uint32 unIPAddress = (octet3) + (octet2 << 8) + (octet1 << 16) + (octet0 << 24);
            initiateServerConnection(unIPAddress, uPort);
        }
    }
}

#pragma mark STEAM_CALLBACK

void DanteClient::onGameJoinRequested(GameRichPresenceJoinRequested_t *pCallback)
{
    // parse out the connect
    const char *serverAddress, *pchLobbyID;
    extern void ParseCommandLine(const char *pchCmdLine, const char **pserverAddress, const char **ppchLobbyID);
    ParseCommandLine(pCallback->m_rgchConnect, &serverAddress, &pchLobbyID);
    
    // exec
    connectToServer(serverAddress);
}

void DanteClient::onSteamServersConnected(SteamServersConnected_t *callback)
{
    if (SteamUser()->BLoggedOn())
    {
        m_eGameState = k_EClientGameMenu;
    }
    else
    {
        LOG("Got SteamServersConnected_t, but not logged on?");
    }
}

void DanteClient::onSteamServersDisconnected(SteamServersDisconnected_t *callback)
{
    LOG("Got SteamServersDisconnected_t");
    
    SetGameState(k_EClientConnectingToSteam);
}

void DanteClient::onSteamServerConnectFailure(SteamServerConnectFailure_t *callback)
{
	LOG("SteamServerConnectFailure_t: %d", callback->m_eResult);
}

void DanteClient::OnGameOverlayActivated(GameOverlayActivated_t *callback)
{
    if (callback->m_bActive)
    {
        LOG("Steam overlay now active");
    }
    else
    {
        LOG("Steam overlay now inactive");
    }
}

//-----------------------------------------------------------------------------
// Purpose: steam callback, triggered when our connection to another client fails
//-----------------------------------------------------------------------------
void DanteClient::onP2PSessionConnectFail(P2PSessionConnectFail_t *pCallback)
{
    if (pCallback->m_steamIDRemote == m_steamIDGameServer)
    {
        // failed, error out
        LOG("Failed to make P2P connection, quiting server");
        onReceiveServerExiting();
    }
}

//-----------------------------------------------------------------------------
// Purpose: Handles notification of a steam ipc failure
// we may get multiple callbacks, one for each IPC operation we attempted
// since the actual failure, so protect ourselves from alerting more than once.
//-----------------------------------------------------------------------------
void DanteClient::onIPCFailure(IPCFailure_t *failure)
{
    static bool bExiting = false;
    if (!bExiting)
    {
        LOG("Steam IPC Failure, shutting down");

#if defined(_WIN32)
        ::MessageBoxA(NULL, "Connection to Steam Lost, Exiting", "Steam Connection Error", MB_OK);
#endif
        
        bExiting = true;
        
        // TODO
    }
}

//-----------------------------------------------------------------------------
// Purpose: Handles notification of a Steam shutdown request since a Windows
// user in a second concurrent session requests to play this game. Shutdown
// this process immediately if possible.
//-----------------------------------------------------------------------------
void DanteClient::onSteamShutdown(SteamShutdown_t *callback)
{
    static bool bExiting = false;
    if (!bExiting)
    {
        LOG("Steam shutdown request, shutting down");
        bExiting = true;
        
        // TODO
    }
}
