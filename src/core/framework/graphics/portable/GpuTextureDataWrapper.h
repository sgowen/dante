//
//  GpuTextureDataWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GpuTextureDataWrapper__
#define __noctisgames__GpuTextureDataWrapper__

#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
#include "framework/graphics/opengl/OpenGLPngImageData.h"
#elif defined _WIN32
#include "pch.h"
#endif

struct GpuTextureDataWrapper
{
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    OpenGLPngImageData raw_image_data;

    GpuTextureDataWrapper(OpenGLPngImageData raw_image_data_in) : raw_image_data(raw_image_data_in) {}
    
    int getWidth() { return raw_image_data.width; }
    int getHeight() { return raw_image_data.height; }
#elif defined _WIN32
    ID3D11ShaderResourceView* texture;

    GpuTextureDataWrapper(ID3D11ShaderResourceView* textureIn) : texture(textureIn) {}
    
    int getWidth() { return texture->getWidth(); }
    int getHeight() { return texture->getHeight(); }
#endif
};

#endif /* defined(__noctisgames__GpuTextureDataWrapper__) */
