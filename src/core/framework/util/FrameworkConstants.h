//
//  FrameworkConstants.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__FrameworkConstants__
#define __noctisgames__FrameworkConstants__

//// Requested Action Definitions ////

#define REQUESTED_ACTION_UPDATE 0
#define REQUESTED_ACTION_EXIT -1

//// Network ////

#define NW_TYPE_Entity 'ENTT'

#define NW_PACKET_TYPE_HELLO 0x01u
#define NW_PACKET_TYPE_WELCOME 0x02u
#define NW_PACKET_TYPE_STATE 0x03u
#define NW_PACKET_TYPE_INPUT 0x04u
#define NW_PACKET_TYPE_ADD_LOCAL_PLAYER 0x05u
#define NW_PACKET_TYPE_DROP_LOCAL_PLAYER 0x06u
#define NW_PACKET_TYPE_LOCAL_PLAYER_ADDED 0x07u
#define NW_PACKET_TYPE_LOCAL_PLAYER_DENIED 0x08u
#define NW_PACKET_TYPE_CLIENT_EXIT 0x09u
#define NW_PACKET_TYPE_SERVER_EXIT 0x0Au
#define NW_PACKET_TYPE_COUNT 0x0Au

#define NW_CLIENT_TIME_BETWEEN_HELLOS 1.0
#define NW_CLIENT_TIMEOUT 3.0
#define NW_SERVER_TIMEOUT 5.0
#define NW_CONNECT_TO_SERVER_TIMEOUT 15.0
#define NW_ACK_TIMEOUT 0.5

#define MAX_NUM_PLAYERS_PER_SERVER 4
#define NW_MAX_NUM_PACKETS_PER_FRAME 10
#define NW_MAX_PACKET_SIZE 1500

#define INPUT_UNASSIGNED 255

//// Rendering ////

#define TEXTURE_SIZE_512 512, 512
#define TEXTURE_SIZE_1024 1024, 1024
#define TEXTURE_SIZE_2048 2048, 2048
#define TEXTURE_SIZE_4096 4096, 4096

#define MAX_BATCH_SIZE 16384
#define NUM_FRAMEBUFFERS 4

#define VERTICES_PER_LINE 2
#define VERTICES_PER_RECTANGLE 4
#define INDICES_PER_RECTANGLE 6

//// Misc ////

#define FRAME_RATE (1.0 / 60.0) // 60 frames per second

#endif /* defined(__noctisgames__FrameworkConstants__) */
