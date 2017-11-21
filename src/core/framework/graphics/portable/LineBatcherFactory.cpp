//
//  LineBatcherFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/14/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/LineBatcherFactory.h"

LineBatcherFactory* LineBatcherFactory::getInstance()
{
    static LineBatcherFactory instance = LineBatcherFactory();
    return &instance;
}

#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
#include "framework/graphics/opengl/OpenGLLineBatcher.h"
LineBatcher* LineBatcherFactory::createLineBatcher() { return new OpenGLLineBatcher(); }
#elif defined _WIN32
#include "framework/graphics/directx/DirectXLineBatcher.h"
LineBatcher* LineBatcherFactory::createLineBatcher() { return new DirectXLineBatcher(); }
#endif

LineBatcherFactory::LineBatcherFactory()
{
    // Hide Constructor for Singleton
}
