//
//  DanteServer.h
//  dante
//
//  Created by Stephen Gowen on 6/13/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DanteServer__
#define __noctisgames__DanteServer__

#include "GameConstants.h"
#include "NGSteam.h"

#include <string>

class DanteClient;
class Robot;

struct ClientConnectionData_t
{
	bool m_bActive;					// Is this slot in use? Or is it available for new connections?
	CSteamID m_SteamIDUser;			// What is the steamid of the player?
	uint64 m_ulTickCountLastData;	// What was the last time we got data from the player?
};

class DanteServer
{
public:
	DanteServer();

	~DanteServer();

	// Run a game frame
	void RunFrame();

	// Set game state
	void SetGameState(EServerGameState eState);

	// Checks for any incoming network data, then dispatches it
	void ReceiveNetworkData();

	// Reset player positions (occurs in between rounds as well as at the start of a new game)
	void ResetPlayerShips();

	// Checks various game objects for collisions and updates state appropriately if they have occurred
	void CheckForCollisions();

	// Kicks a given player off the server
	void kickPlayerOffServer(CSteamID steamID);

	// data accessors
	bool IsConnectedToSteam() { return m_bConnectedToSteam; }
	
    CSteamID GetSteamID();

private:
	//
	// Various callback functions that Steam will call to let us know about events related to our
	// connection to the Steam servers for authentication purposes.
	//


	// Tells us when we have successfully connected to Steam
	STEAM_GAMESERVER_CALLBACK(DanteServer, onSteamServersConnected, SteamServersConnected_t);

	// Tells us when there was a failure to connect to Steam
	STEAM_GAMESERVER_CALLBACK(DanteServer, onSteamServersConnectFailure, SteamServerConnectFailure_t);

	// Tells us when we have been logged out of Steam
	STEAM_GAMESERVER_CALLBACK(DanteServer, onSteamServersDisconnected, SteamServersDisconnected_t);

	// Tells us that Steam has set our security policy (VAC on or off)
	STEAM_GAMESERVER_CALLBACK(DanteServer, OnPolicyResponse, GSPolicyResponse_t);

	//
	// Various callback functions that Steam will call to let us know about whether we should
	// allow clients to play or we should kick/deny them.
	//

	// Tells us a client has been authenticated and approved to play by Steam (passes auth, license check, VAC status, etc...)
	STEAM_GAMESERVER_CALLBACK(DanteServer, OnValidateAuthTicketResponse, ValidateAuthTicketResponse_t);

	// client connection state
	STEAM_GAMESERVER_CALLBACK(DanteServer, OnP2PSessionRequest, P2PSessionRequest_t);
	STEAM_GAMESERVER_CALLBACK(DanteServer, onP2PSessionConnectFail, P2PSessionConnectFail_t);

	// Function to tell Steam about our servers details
	void SendUpdatedServerDetailsToSteam();

	// Receive updates from client
	void onReceiveClientUpdateData(uint32 uShipIndex, ClientDanteUpdateData_t *pUpdateData);

	// Send data to a client at the given ship index
	bool BSendDataToClient(uint32 uShipIndex, char *pData, uint32 nSizeOfData);

	// Send data to a client at the given pending index
	bool BSendDataToPendingClient(uint32 uShipIndex, char *pData, uint32 nSizeOfData);

	// Connect a client, will send a success/failure response to the client
	void OnClientBeginAuthentication(CSteamID steamIDClient, void *pToken, uint32 uTokenLen);

	// Handles authentication completing for a client
	void OnAuthCompleted(bool bAuthSuccess, uint32 iPendingAuthIndex);

	// Adds/initializes a new player ship at the given position
	void AddPlayerShip(uint32 uShipPosition);

	// Removes a player from the server
	void RemovePlayerFromServer(uint32 uShipPosition);

	// Send world update to all clients
	void SendUpdateDataToAllClients();

	// Track whether our server is connected to Steam ok (meaning we can restrict who plays based on 
	// ownership and VAC bans, etc...)
	bool m_bConnectedToSteam;

	// Ships for players, doubles as a way to check for open slots (pointer is NULL meaning open)
	Robot* m_robots[MAX_PLAYERS_PER_SERVER];

	// server name
	std::string m_sServerName;

	// Last time state changed
	uint64 m_ulStateTransitionTime;

	// Last time we sent clients an update
	uint64 m_ulLastServerUpdateTick;

	// Current game state
	EServerGameState m_eGameState;

	// Vector to keep track of client connections
	ClientConnectionData_t m_rgClientData[MAX_PLAYERS_PER_SERVER];

	// Vector to keep track of client connections which are pending auth
	ClientConnectionData_t m_rgPendingClientData[MAX_PLAYERS_PER_SERVER];
};

#endif /* defined(__noctisgames__DanteServer__) */
