//
//  RendererHelperFactory.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/RendererHelperFactory.h>

#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
#include <framework/graphics/opengl/OpenGLRendererHelper.h>
#elif defined _WIN32
#include <framework/graphics/directx/DirectXRendererHelper.h>
#endif

RendererHelperFactory* RendererHelperFactory::getInstance()
{
    static RendererHelperFactory instance = RendererHelperFactory();
    return &instance;
}

RendererHelper* RendererHelperFactory::createRendererHelper()
{
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    return new OpenGLRendererHelper();
#elif defined _WIN32
    return new DirectXRendererHelper();
#endif
}

RendererHelperFactory::RendererHelperFactory()
{
    // Hide Constructor for Singleton
}
