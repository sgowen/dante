//
//  VertexProgramInput.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/16/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__VertexProgramInput__
#define __noctisgames__VertexProgramInput__

typedef float vec4[4];
typedef vec4 mat4x4[4];

struct SCREEN_VERTEX
{
    float X, Y;
};

struct TEXTURE_VERTEX
{
    float X, Y, Z;
    float R, G, B, A;
    float U, V;
};

struct COLOR_VERTEX
{
	float X, Y, Z;
	float R, G, B, A;
};

#endif /* defined(__noctisgames__VertexProgramInput__) */
