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

//// Requested Action Definitions ////

#define REQUESTED_ACTION_UPDATE 0

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

#endif /* defined(__dante__GameConstants__) */
