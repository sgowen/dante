//
//  Constants.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/2/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Constants__
#define __noctisgames__Constants__

//// Requested Action Definitions ////

#define REQUESTED_ACTION_UPDATE 0
#define REQUESTED_ACTION_EXIT -1

//// Network ////

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
#define NW_MAX_PACKET_SIZE 1200

#define INPUT_UNASSIGNED 7

#define  LAYOUT_ENTITY_ID_BEGIN 0x00000000u
#define  LAYOUT_ENTITY_ID_END   0x0000FFFFu
#define NETWORK_ENTITY_ID_BEGIN 0x00010000u
#define NETWORK_ENTITY_ID_END   0xFFFFFFFFu

#define MAX_NUM_DYNAMIC_ENTITIES 24
#define MAX_USER_NAME_LENGTH 16

//// Entity Message Definitions ////

#define ENTITY_MESSAGE_DAMAGE 0x01u

//// Rendering ////

#define MAX_BATCH_SIZE 16384
#define NUM_FRAMEBUFFERS 10
#define NUM_TEXTURE_VERTEX_BUFFERS 11
#define NUM_VERTEX_BUFFERS 11
#define INDEX_LAST_TEXTURE_VERTEX_BUFFER 10
#define INDEX_LAST_VERTEX_BUFFERS 10

#define VERTICES_PER_LINE 2
#define VERTICES_PER_RECTANGLE 4
#define VERTICES_PER_TRIANGLE 3
#define INDICES_PER_RECTANGLE 6

//// Misc ////

#define FRAME_RATE (1.0 / 60.0) // 60 frames per second

#endif /* defined(__noctisgames__Constants__) */
