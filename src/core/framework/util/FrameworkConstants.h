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

//// Shader Definitions ////

#define COLOR_VERTEX_SHADER "shader_001_vert.ngs"
#define COLOR_FRAGMENT_SHADER "shader_001_frag.ngs"
#define FRAMEBUFFER_TO_SCREEN_VERTEX_SHADER "shader_002_vert.ngs"
#define FRAMEBUFFER_TO_SCREEN_FRAGMENT_SHADER "shader_002_frag.ngs"
#define TEXTURE_VERTEX_SHADER "shader_003_vert.ngs"
#define TEXTURE_FRAGMENT_SHADER "shader_003_frag.ngs"

//// Network ////

#define NW_TYPE_Entity 'ENTT'

#define NW_PACKET_TYPE_HELLO 'HELO'
#define NW_PACKET_TYPE_WELCOME 'WLCM'
#define NW_PACKET_TYPE_STATE 'STAT'
#define NW_PACKET_TYPE_INPUT 'INPT'
#define NW_PACKET_TYPE_ADD_LOCAL_PLAYER 'ADLP'
#define NW_PACKET_TYPE_DROP_LOCAL_PLAYER 'DPLP'
#define NW_PACKET_TYPE_LOCAL_PLAYER_ADDED 'LPAD'
#define NW_PACKET_TYPE_LOCAL_PLAYER_DENIED 'LPDE'
#define NW_PACKET_TYPE_CLIENT_EXIT 'CEXI'
#define NW_PACKET_TYPE_SERVER_EXIT 'SEXI'

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

#define MAX_BATCH_SIZE 16384
#define NUM_FRAMEBUFFERS 4
#define VERTICES_PER_LINE 2
#define VERTICES_PER_RECTANGLE 4
#define INDICES_PER_RECTANGLE 6

//// Misc ////

#define FRAME_RATE (1.0 / 60.0) // 60 frames per second

#endif /* defined(__noctisgames__FrameworkConstants__) */
