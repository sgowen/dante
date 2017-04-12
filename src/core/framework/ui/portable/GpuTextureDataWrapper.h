//
//  GpuTextureDataWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GpuTextureDataWrapper__
#define __noctisgames__GpuTextureDataWrapper__

#if defined __APPLE__ || defined __ANDROID__
#include "PngImageData.h"
#elif defined _WIN32
#include "pch.h"
#endif

struct GpuTextureDataWrapper
{
#if defined __APPLE__ || defined __ANDROID__
    PngImageData raw_image_data;
    
    GpuTextureDataWrapper(PngImageData raw_image_data_in) : raw_image_data(raw_image_data_in) {}
#elif defined _WIN32
    ID3D11ShaderResourceView* texture;

    GpuTextureDataWrapper(ID3D11ShaderResourceView* textureIn) : texture(textureIn) {}
#endif
};

#endif /* defined(__noctisgames__GpuTextureDataWrapper__) */
