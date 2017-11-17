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
#define VERSION_MINOR 0
#define VERSION_BUILD 0
#define VERSION_RVISN 0

#define VERSION_STRING "1.0.0.0"

//// Sound Definitions ////

#define SOUND_ID_NONE 0
#define SOUND_ID_ROBOT_JUMP 1
#define SOUND_ID_EXPLOSION 2
#define SOUND_ID_DEATH 3
#define SOUND_ID_FIRE_ROCKET 4
#define SOUND_ID_ACTIVATE_SPRINT 5
#define SOUND_ID_HEADSHOT 6

#define SOUND_ROBOT_JUMP "sound_001"
#define SOUND_EXPLOSION "sound_002"
#define SOUND_DEATH "sound_003"
#define SOUND_FIRE_ROCKET "sound_004"
#define SOUND_ACTIVATE_SPRINT "sound_005"
#define SOUND_HEADSHOT "sound_006"

//// Music Definitions ////

#define MUSIC_DEMO "music_001"

//// Render Definitions ////

#define MAX_BATCH_SIZE 16384

//// Matrix Definitions ////

#define CAM_WIDTH 24.0f // meters
#define CAM_HEIGHT 10.0f // meters

//// World Definitions ////

#define GRID_CELL_SIZE 0.140625f

// these values are temporary, the real game will have dynamic widths and heights
#define GAME_WIDTH CAM_WIDTH * 3.0f // meters
#define GAME_HEIGHT 16.0f // meters

#define DEAD_ZONE_LEFT -10.0f // meters
#define DEAD_ZONE_RIGHT GAME_WIDTH + 10.0f // meters
#define DEAD_ZONE_BOTTOM -2.0f // meters

//// Network ////

#define NW_TYPE_Robot 'ROBT'
#define NW_TYPE_Projectile 'PRJC'
#define NW_TYPE_SpacePirate 'SPCP'
#define NW_TYPE_Ground 'GRND'
#define NW_TYPE_Crate 'CRAT'
#define NW_TYPE_SpacePirateChunk 'SPCH'

//// Non-Steam ////

// UDP port for the server to bind on
#define SERVER_PORT 9999

// This allows us to run both a debug and a release socket-based client on the same machine
#if defined(DEBUG) || defined(_DEBUG)
#define CLIENT_PORT 1339
#else
#define CLIENT_PORT 1337
#endif

//// Steam ////
#define STEAM_GAME_DIR "projectdante"
#define STEAM_PRODUCT_NAME "Project Dante"
#define STEAM_PRODUCT_DESC "A 4 Player Co-Op Dungeon Crawler set in a Grim Wasteland"

#endif /* defined(__dante__GameConstants__) */
