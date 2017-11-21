//
//  SpriteBatcherFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/14/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/SpriteBatcherFactory.h"

SpriteBatcherFactory* SpriteBatcherFactory::getInstance()
{
    static SpriteBatcherFactory instance = SpriteBatcherFactory();
    return &instance;
}

#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
#include "framework/graphics/opengl/OpenGLSpriteBatcher.h"
SpriteBatcher* SpriteBatcherFactory::createSpriteBatcher() { return new OpenGLSpriteBatcher(); }
#elif defined _WIN32
#include "framework/graphics/directx/DirectXSpriteBatcher.h"
SpriteBatcher* SpriteBatcherFactory::createSpriteBatcher() { return new DirectXSpriteBatcher(); }
#endif

SpriteBatcherFactory::SpriteBatcherFactory()
{
    // Hide Constructor for Singleton
}
