//
//  GpuTextureWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/25/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GpuTextureWrapper__
#define __noctisgames__GpuTextureWrapper__

#if defined __APPLE__ || defined __ANDROID__
#include "platform_gl.h"
#elif defined _WIN32
#include "pch.h"
#endif

struct GpuTextureWrapper
{
#if defined __APPLE__ || defined __ANDROID__
    GLuint texture;
    
    GpuTextureWrapper(GLuint textureIn) : texture(textureIn) {}
#elif defined _WIN32
    ID3D11ShaderResourceView* texture;
    
    GpuTextureWrapper(ID3D11ShaderResourceView* textureIn) : texture(textureIn) {}
#endif
};

#endif /* defined(__noctisgames__GpuTextureWrapper__) */
