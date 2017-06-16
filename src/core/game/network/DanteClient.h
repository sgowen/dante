//
//  DanteClient.h
//  dante
//
//  Created by Stephen Gowen on 6/13/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DanteClient__
#define __noctisgames__DanteClient__

#include "INGSteamClient.h"

#include "GameConstants.h"
#include "NGSteamGameServerPing.h"

class DanteServer;
class Robot;
class CP2PAuthPlayer;
class CP2PAuthedGame;

// Enum for various client connection states
enum EClientConnectionState
{
	k_EClientNotConnected,							// Initial state, not connected to a server
	k_EClientConnectedPendingAuthentication,		// We've established communication with the server, but it hasn't authed us yet
	k_EClientConnectedAndAuthenticated,				// Final phase, server has authed us, we are actually able to play on it
};

// a game server as shown in the find servers menu
struct ServerBrowserMenuData_t
{
	EClientGameState m_eStateToTransitionTo;
	CSteamID m_steamIDGameServer;
};

class DanteClient : public INGSteamClient
{
public:
	DanteClient();

    ~DanteClient();
    
    void init();

	// Run a game frame
	void RunFrame();

	// Checks for any incoming network data, then dispatches it
	void ReceiveNetworkData();

	// Connect to a server at a given IP address or game server steamID
	virtual void initiateServerConnection(CSteamID steamIDGameServer);
	virtual void initiateServerConnection(uint32 unServerAddress, const int32 nPort);

	// Send data to a client at the given ship index
	bool BSendServerData(const void *pData, uint32 nSizeOfData);

	// Menu callback handler (handles a bunch of menus that just change state with no extra data)
	void OnMenuSelection(EClientGameState eState) { SetGameState(eState); }

	// Menu callback handler (handles server browser selections with extra data)
	void OnMenuSelection(ServerBrowserMenuData_t selection) 
	{ 
		if (selection.m_eStateToTransitionTo == k_EClientGameConnecting)
		{
			initiateServerConnection(selection.m_steamIDGameServer);
		}
		else
		{
			SetGameState(selection.m_eStateToTransitionTo); 
		}
	}

	// Set game state
	void SetGameState(EClientGameState eState);
	EClientGameState GetGameState() { return m_eGameState; }

	// set failure text
	void SetConnectionFailureText(const char *pchErrorText);

	// Get the steam id for the local user at this client
	CSteamID GetLocalSteamID() { return m_SteamIDLocalUser; }

	// Get the local players name
	const char* GetLocalPlayerName() 
	{ 
		return SteamFriends()->GetFriendPersonaName(m_SteamIDLocalUser); 
	}

	void connectToServer(const char *serverAddress, const char *pchLobbyID);

private:

	// Receive a response from the server for a connection attempt
	void onReceiveServerInfo(CSteamID steamIDGameServer, bool bVACSecure, const char *pchServerName);

	// Receive a response from the server for a connection attempt
	void onReceiveServerAuthenticationResponse(bool bSuccess, uint32 uPlayerPosition);

	// Receive a state update from the server
	void onReceiveServerUpdate(ServerDanteUpdateData_t *pUpdateData);

	// Handle the server exiting
	void onReceiveServerExiting();

	// Disconnects from a server (telling it so) if we are connected
	void disconnectFromServer();

	// game state changes
	void onGameStateChanged(EClientGameState eGameStateNew);

	// Updates what we show to friends about what we're doing and how to connect
	void UpdateRichPresenceConnectionInfo();

	// Server we are connected to
	DanteServer *m_pServer;

	// SteamID for the local user on this client
	CSteamID m_SteamIDLocalUser;

	// Our ship position in the array below
	uint32 m_uPlayerIndex;

	// List of steamIDs for each player
	CSteamID m_rgSteamIDPlayers[MAX_PLAYERS_PER_SERVER];

	// Ships for players, doubles as a way to check for open slots (pointer is NULL meaning open)
	Robot *m_robots[MAX_PLAYERS_PER_SERVER];

	// Player scores
	uint32 m_rguPlayerScores[MAX_PLAYERS_PER_SERVER];

	// Who just won the game? Should be set if we go into the k_EGameWinner state
	uint32 m_uPlayerWhoWonGame;

	// Current game state
	EClientGameState m_eGameState;

	// true if we only just transitioned state
	bool m_bTransitionedGameState;

	// Time we started our last connection attempt
	uint64 m_ulLastConnectionAttemptRetryTime;

	// Time we last got data from the server
	uint64 m_ulLastNetworkDataReceivedTime;

	// Time when we sent our ping
	uint64 m_ulPingSentTime;

	// Server address data
	CSteamID m_steamIDGameServer;
	uint32 m_unServerIP;
	uint16 m_usServerPort;
	HAuthTicket m_hAuthTicket;

	NGSteamGameServerPing m_GameServerPing;

	// Track whether we are connected to a server (and what specific state that connection is in)
	EClientConnectionState m_eConnectedStatus;
    
    // p2p game auth manager
    CP2PAuthedGame *m_pP2PAuthedGame;

	// callback for when the lobby game server has started
	STEAM_CALLBACK(DanteClient, onGameJoinRequested, GameRichPresenceJoinRequested_t);

	// callbacks for Steam connection state
	STEAM_CALLBACK(DanteClient, onSteamServersConnected, SteamServersConnected_t);
	STEAM_CALLBACK(DanteClient, onSteamServersDisconnected, SteamServersDisconnected_t);
	STEAM_CALLBACK(DanteClient, onSteamServerConnectFailure, SteamServerConnectFailure_t);
	STEAM_CALLBACK(DanteClient, OnGameOverlayActivated, GameOverlayActivated_t);

	// connection handler
	STEAM_CALLBACK(DanteClient, onP2PSessionConnectFail, P2PSessionConnectFail_t);

	// ipc failure handler
	STEAM_CALLBACK(DanteClient, onIPCFailure, IPCFailure_t);

	// Steam wants to shut down, Game for Windows applications should shutdown too
	STEAM_CALLBACK(DanteClient, onSteamShutdown, SteamShutdown_t);
};

#endif /* defined(__noctisgames__DanteClient__) */
