//
//  TextureDataWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/16.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__TextureDataWrapper__
#define __noctisgames__TextureDataWrapper__

#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
#include "framework/graphics/opengl/OpenGLPngImageData.h"
#elif defined _WIN32
#include "pch.h"
#endif

struct TextureDataWrapper
{
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    OpenGLPngImageData raw_image_data;

    TextureDataWrapper(OpenGLPngImageData raw_image_data_in) : raw_image_data(raw_image_data_in) {}
#elif defined _WIN32
    ID3D11ShaderResourceView* texture;

    TextureDataWrapper(ID3D11ShaderResourceView* textureIn) : texture(textureIn) {}
#endif
};

#endif /* defined(__noctisgames__TextureDataWrapper__) */
