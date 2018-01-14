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

struct TEXTURE_VERTEX
{
    float X, Y;
    float R, G, B, A;
    float U, V;
    
    TEXTURE_VERTEX() : X(0), Y(0), R(0), G(0), B(0), A(0), U(0), V(0) {}
    TEXTURE_VERTEX(float inX, float inY, float inR, float inG, float inB, float inA, float inU, float inV) : X(inX), Y(inY), R(inR), G(inG), B(inB), A(inA), U(inU), V(inV) {}
};

struct COLOR_VERTEX
{
	float X, Y;
	float R, G, B, A;
    
    COLOR_VERTEX() : X(0), Y(0), R(0), G(0), B(0), A(0) {}
    COLOR_VERTEX(float inX, float inY, float inR, float inG, float inB, float inA) : X(inX), Y(inY), R(inR), G(inG), B(inB), A(inA) {}
};

struct SCREEN_VERTEX
{
    float X, Y;
    
    SCREEN_VERTEX() : X(0), Y(0) {}
    SCREEN_VERTEX(float inX, float inY) : X(inX), Y(inY) {}
};

#endif /* defined(__noctisgames__VertexProgramInput__) */
