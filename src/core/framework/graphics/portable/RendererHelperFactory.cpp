//
//  RendererHelperFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "RendererHelperFactory.h"

RendererHelperFactory* RendererHelperFactory::getInstance()
{
    static RendererHelperFactory instance = RendererHelperFactory();
    return &instance;
}

#if defined __APPLE__ || defined __ANDROID__
#include "OpenGLRendererHelper.h"
IRendererHelper* RendererHelperFactory::createRendererHelper() { return new OpenGLRendererHelper(); }
#elif defined _WIN32
#include "Direct3DRendererHelper.h"
IRendererHelper* RendererHelperFactory::createRendererHelper() { return new Direct3DRendererHelper(); }
#endif

RendererHelperFactory::RendererHelperFactory()
{
    // Hide Constructor for Singleton
}
