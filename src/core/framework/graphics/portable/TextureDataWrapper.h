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
#include <framework/graphics/portable/NGGraphics.h>

struct OpenGLPngImageData
{
    const int _width;
    const int _height;
    const int _size;
    const GLenum _glColorFormat;
    const void* _data;
    
    OpenGLPngImageData(const int width, const int height, const int size, const GLenum glColorFormat, const void* data) : _width(width), _height(height), _size(size), _glColorFormat(glColorFormat), _data(data) {}
};

#elif defined _WIN32
#include "pch.h"
#endif

struct TextureDataWrapper
{
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    OpenGLPngImageData* _rawImageData;

    TextureDataWrapper(OpenGLPngImageData* rawImageData) : _rawImageData(rawImageData) {}
#elif defined _WIN32
    ID3D11ShaderResourceView* texture;

    TextureDataWrapper(ID3D11ShaderResourceView* textureIn) : texture(textureIn) {}
#endif
};

#endif /* defined(__noctisgames__TextureDataWrapper__) */
