//
//  LineBatcherFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/14/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "LineBatcherFactory.h"

LineBatcherFactory* LineBatcherFactory::getInstance()
{
    static LineBatcherFactory instance = LineBatcherFactory();
    return &instance;
}

#if defined __APPLE__ || defined __ANDROID__
#include "OpenGLLineBatcher.h"
LineBatcher* LineBatcherFactory::createLineBatcher() { return new OpenGLLineBatcher(); }
#elif defined _WIN32
#include "Direct3DLineBatcher.h"
LineBatcher* LineBatcherFactory::createLineBatcher() { return new Direct3DLineBatcher(); }
#endif

LineBatcherFactory::LineBatcherFactory()
{
    // Hide Constructor for Singleton
}
