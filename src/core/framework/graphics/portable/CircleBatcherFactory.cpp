//
//  CircleBatcherFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/14/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/CircleBatcherFactory.h"

CircleBatcherFactory* CircleBatcherFactory::getInstance()
{
    static CircleBatcherFactory instance = CircleBatcherFactory();
    return &instance;
}

#if defined __APPLE__ || defined __ANDROID__
#include "framework/graphics/opengl/OpenGLCircleBatcher.h"
CircleBatcher* CircleBatcherFactory::createCircleBatcher() { return new OpenGLCircleBatcher(); }
#elif defined _WIN32
#include "framework/graphics/directx/DirectXCircleBatcher.h"
CircleBatcher* CircleBatcherFactory::createCircleBatcher() { return new DirectXCircleBatcher(); }
#endif

CircleBatcherFactory::CircleBatcherFactory()
{
    // Hide Constructor for Singleton
}
