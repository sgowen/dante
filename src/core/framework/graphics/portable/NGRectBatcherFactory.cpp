//
//  NGRectBatcherFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/14/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NGRectBatcherFactory.h"

NGRectBatcherFactory* NGRectBatcherFactory::getInstance()
{
    static NGRectBatcherFactory instance = NGRectBatcherFactory();
    return &instance;
}

#if defined __APPLE__ || defined __ANDROID__
#include "OpenGLNGRectBatcher.h"
NGRectBatcher* NGRectBatcherFactory::createNGRectBatcher(bool isFill) { return new OpenGLNGRectBatcher(isFill); }
#elif defined _WIN32
#include "DirectXNGRectBatcher.h"
NGRectBatcher* NGRectBatcherFactory::createNGRectBatcher(bool isFill) { return new DirectXNGRectBatcher(isFill); }
#endif

NGRectBatcherFactory::NGRectBatcherFactory()
{
    // Hide Constructor for Singleton
}
