//
//  SpriteBatcherFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/14/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SpriteBatcherFactory.h"

SpriteBatcherFactory* SpriteBatcherFactory::getInstance()
{
    static SpriteBatcherFactory instance = SpriteBatcherFactory();
    return &instance;
}

#if defined __APPLE__ || defined __ANDROID__
#include "OpenGLSpriteBatcher.h"
SpriteBatcher* SpriteBatcherFactory::createSpriteBatcher() { return new OpenGLSpriteBatcher(); }
#elif defined _WIN32
#include "Direct3DSpriteBatcher.h"
SpriteBatcher* SpriteBatcherFactory::createSpriteBatcher() { return new Direct3DSpriteBatcher(); }
#endif

SpriteBatcherFactory::SpriteBatcherFactory()
{
    // Hide Constructor for Singleton
}
