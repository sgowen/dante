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

#define SOUND_ROBOT_JUMP "sound_001"

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

#endif /* defined(__dante__GameConstants__) */
