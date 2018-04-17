//
//  TextureLoaderFactory.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/TextureLoaderFactory.h>

TextureLoaderFactory* TextureLoaderFactory::getInstance()
{
    static TextureLoaderFactory instance = TextureLoaderFactory();
    return &instance;
}

#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
#include <framework/graphics/opengl/OpenGLTextureLoader.h>
TextureLoader* TextureLoaderFactory::createTextureLoader() { return new OpenGLTextureLoader(); }
#elif defined _WIN32
#include <framework/graphics/directx/DirectXTextureLoader.h>
TextureLoader* TextureLoaderFactory::createTextureLoader() { return new DirectXTextureLoader(); }
#endif

TextureLoaderFactory::TextureLoaderFactory()
{
    // Hide Constructor for Singleton
}
