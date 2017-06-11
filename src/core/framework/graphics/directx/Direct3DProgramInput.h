//
//  Direct3DProgramInput.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/16/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Direct3DProgramInput__
#define __noctisgames__Direct3DProgramInput__

struct COLOR_VERTEX
{
	float X, Y, Z; // vertex position
	float R, G, B, A; // vertex color
};

struct TEXTURE_VERTEX
{
	float X, Y, Z; // vertex position
	float R, G, B, A; // vertex color
	float U, V;    // texture coordinates
};

#endif /* defined(__noctisgames__Direct3DProgramInput__) */
