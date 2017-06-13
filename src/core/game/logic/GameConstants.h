//
//  GameConstants.h
//  dante
//
//  Created by Stephen Gowen on 5/13/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __dante__GameConstants__
#define __dante__GameConstants__

//// VERSION ////

#define VERSION_MAJOR 1
#define VERSION_MINOR 2
#define VERSION_BUILD 0

//// Sound Definitions ////

#define SOUND_ID_NONE 0
#define SOUND_ID_ROBOT_JUMP 1
#define SOUND_ID_EXPLOSION 2
#define SOUND_ID_DEATH 3
#define SOUND_ID_FIRE_ROCKET 4
#define SOUND_ID_ACTIVATE_SPRINT 5

#define SOUND_ROBOT_JUMP "sound_001"
#define SOUND_EXPLOSION "sound_002"
#define SOUND_DEATH "sound_003"
#define SOUND_FIRE_ROCKET "sound_004"
#define SOUND_ACTIVATE_SPRINT "sound_005"

//// Music Definitions ////

#define MUSIC_DEMO "music_001"

//// Render Definitions ////

#define MAX_BATCH_SIZE 16384

//// Matrix Definitions ////

#define CAM_WIDTH 24.0f // in meters
#define CAM_HEIGHT 10.0f // in meters

//// World Definitions ////

#define GRID_CELL_SIZE 0.140625f

//// Render Definitions ////

#define NUM_FRAMEBUFFERS 4

//// Misc ////

#define FRAME_RATE 0.01666666666667f // 60 frames per second

//// Network ////

#define NETWORK_TYPE_Robot 'ROBT'
#define NETWORK_TYPE_Projectile 'PRJC'
#define NETWORK_TYPE_SpacePirate 'SPCP'

// Current game server version
#define DANTE_SERVER_VERSION "0.0.1.0"

// UDP port for the project dante server to do authentication on (ie, talk to Steam on)
#define DANTE_AUTHENTICATION_PORT 8766

// UDP port for the project dante server to listen on
#define DANTE_SERVER_PORT 27015

// UDP port for the master server updater to listen on
#define DANTE_MASTER_SERVER_UPDATER_PORT 27016

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
    k_EClientConnectingToSteam
};

enum EServerGameState
{
    k_EServerWaitingForPlayers,
    k_EServerActive,
    k_EServerExiting
};

#endif /* defined(__dante__GameConstants__) */
