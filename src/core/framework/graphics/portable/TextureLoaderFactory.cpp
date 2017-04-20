//
//  TextureLoaderFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "TextureLoaderFactory.h"

TextureLoaderFactory* TextureLoaderFactory::getInstance()
{
    static TextureLoaderFactory instance = TextureLoaderFactory();
    return &instance;
}

#if defined __APPLE__ || defined __ANDROID__
#include "OpenGLTextureLoader.h"
ITextureLoader* TextureLoaderFactory::createTextureLoader() { return new OpenGLTextureLoader(); }
#elif defined _WIN32
#include "Direct3DTextureLoader.h"
ITextureLoader* TextureLoaderFactory::createTextureLoader() { return new Direct3DTextureLoader(); }
#endif

TextureLoaderFactory::TextureLoaderFactory()
{
    // Hide Constructor for Singleton
}
