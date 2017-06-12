//========= Copyright � 1996-2008, Valve LLC, All rights reserved. ============
//
// Purpose: Shared definitions for the communication between the server/client
//
// $NoKeywords: $
//=============================================================================

#ifndef SPACEWAR_H
#define SPACEWAR_H

#include "stdafx.h"

// Current game server version
#define DANTE_SERVER_VERSION "0.0.1.0"

// UDP port for the spacewar server to do authentication on (ie, talk to Steam on)
#define SPACEWAR_AUTHENTICATION_PORT 8766

// UDP port for the spacewar server to listen on
#define SPACEWAR_SERVER_PORT 27015

// UDP port for the master server updater to listen on
#define SPACEWAR_MASTER_SERVER_UPDATER_PORT 27016

// How long to wait for a client to send an update before we drop its connection server side
#define SERVER_TIMEOUT_MILLISECONDS 5000

// Maximum number of players who can join a server and play simultaneously
#define MAX_PLAYERS_PER_SERVER 4

// Time to pause wait after a round ends before starting a new one
#define MILLISECONDS_BETWEEN_ROUNDS 4000

// Time to timeout a connection attempt in
#define MILLISECONDS_CONNECTION_TIMEOUT 30000

// How many times a second does the server send world updates to clients
#define SERVER_UPDATE_SEND_RATE 60

enum EClientGameState
{
	k_EClientGameStartServer,
	k_EClientGameActive,
	k_EClientGameWaitingForPlayers,
	k_EClientGameMenu,
	k_EClientGameQuitMenu,
	k_EClientGameExiting,
	k_EClientGameConnecting,
	k_EClientGameConnectionFailure,
	k_EClientFindInternetServers,
	k_EClientFindLANServers,
	k_EClientRemoteStorage,
	k_EClientConnectingToSteam,
	k_EClientLinkSteamAccount,
	k_EClientAutoCreateAccount,
	k_EClientRetrySteamConnection,
};

enum EServerGameState
{
	k_EServerWaitingForPlayers,
	k_EServerActive,
	k_EServerExiting
};

#endif // SPACEWAR_H
