//
//  VertexProgramInput.h
//  noctisgames
//
//  Created by Stephen Gowen on 11/16/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__VertexProgramInput__
#define __noctisgames__VertexProgramInput__

typedef int int4[4];
typedef float float4[4];
typedef float4 mat4x4[4];

struct VERTEX_2D_TEXTURE
{
    float X, Y;
    float U, V;
    
    VERTEX_2D_TEXTURE() : X(0), Y(0), U(0), V(0) {}
    VERTEX_2D_TEXTURE(float inX, float inY, float inU, float inV) : X(inX), Y(inY), U(inU), V(inV) {}
};

struct VERTEX_2D
{
    float X, Y;
    
    VERTEX_2D() : X(0), Y(0) {}
    VERTEX_2D(float inX, float inY) : X(inX), Y(inY) {}
};

#endif /* defined(__noctisgames__VertexProgramInput__) */
