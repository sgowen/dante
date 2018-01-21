//
//  GameConstants.h
//  dante
//
//  Created by Stephen Gowen on 5/13/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GameConstants__
#define __noctisgames__GameConstants__

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

#define SOUND_ROBOT_JUMP "sound_001.wav"
#define SOUND_EXPLOSION "sound_002.wav"
#define SOUND_DEATH "sound_003.wav"
#define SOUND_FIRE_ROCKET "sound_004.wav"
#define SOUND_ACTIVATE_SPRINT "sound_005.wav"
#define SOUND_HEADSHOT "sound_006.wav"

//// Music Definitions ////

#define MUSIC_DEMO "music_001.wav"

//// Matrix Definitions ////

#define CAM_WIDTH 64
#define CAM_HEIGHT 36

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

#endif /* defined(__noctisgames__GameConstants__) */
