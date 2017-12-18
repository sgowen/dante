//
//  ShaderProgramLoaderFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/ShaderProgramLoaderFactory.h"

ShaderProgramLoaderFactory* ShaderProgramLoaderFactory::getInstance()
{
    static ShaderProgramLoaderFactory instance = ShaderProgramLoaderFactory();
    return &instance;
}

#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
#include "framework/graphics/opengl/OpenGLProgramLoader.h"
ShaderProgramLoader* ShaderProgramLoaderFactory::createShaderProgramLoader() { return new OpenGLProgramLoader(); }
#elif defined _WIN32
#include "framework/graphics/directx/DirectXProgramLoader.h"
ShaderProgramLoader* ShaderProgramLoaderFactory::createShaderProgramLoader() { return new DirectXProgramLoader(); }
#endif

ShaderProgramLoaderFactory::ShaderProgramLoaderFactory()
{
    // Hide Constructor for Singleton
}
