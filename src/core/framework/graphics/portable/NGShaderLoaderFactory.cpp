//
//  NGShaderLoaderFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/NGShaderLoaderFactory.h"

NGShaderLoaderFactory* NGShaderLoaderFactory::getInstance()
{
    static NGShaderLoaderFactory instance = NGShaderLoaderFactory();
    return &instance;
}

#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
#include "framework/graphics/opengl/OpenGLProgramLoader.h"
NGShaderLoader* NGShaderLoaderFactory::createNGShaderLoader() { return new OpenGLProgramLoader(); }
#elif defined _WIN32
#include "framework/graphics/directx/DirectXProgramLoader.h"
NGShaderLoader* NGShaderLoaderFactory::createNGShaderLoader() { return new DirectXProgramLoader(); }
#endif

NGShaderLoaderFactory::NGShaderLoaderFactory()
{
    // Hide Constructor for Singleton
}
