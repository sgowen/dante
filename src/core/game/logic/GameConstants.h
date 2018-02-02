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

//// Matrix Definitions ////

#define CAM_WIDTH 64
#define CAM_HEIGHT 36

//// Entity Message Definitions ////

#define ENTITY_MESSAGE_DAMAGE 0x01u

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
