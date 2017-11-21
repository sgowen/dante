//
//  GpuProgramWrapperFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/GpuProgramWrapperFactory.h"

GpuProgramWrapperFactory* GpuProgramWrapperFactory::getInstance()
{
    static GpuProgramWrapperFactory instance = GpuProgramWrapperFactory();
    return &instance;
}

#if defined __APPLE__ || defined __ANDROID__ || defined __linux__

#include "framework/graphics/opengl/OpenGLTextureGpuProgramWrapper.h"
#include "framework/graphics/opengl/OpenGLGeometryGpuProgramWrapper.h"
#include "framework/graphics/opengl/OpenGLFramebufferToScreenGpuProgramWrapper.h"

GpuProgramWrapper* GpuProgramWrapperFactory::createTextureGpuProgramWrapper()
{
    return new OpenGLTextureGpuProgramWrapper();
}

GpuProgramWrapper* GpuProgramWrapperFactory::createColorGpuProgramWrapper()
{
    return new OpenGLGeometryGpuProgramWrapper();
}

GpuProgramWrapper* GpuProgramWrapperFactory::createFramebufferToScreenGpuProgramWrapper()
{
    return new OpenGLFramebufferToScreenGpuProgramWrapper();
}
#elif defined _WIN32

#include "framework/graphics/directx/DirectXTextureGpuProgramWrapper.h"
#include "framework/graphics/directx/DirectXGeometryGpuProgramWrapper.h"
#include "framework/graphics/directx/DirectXFramebufferToScreenGpuProgramWrapper.h"

GpuProgramWrapper* GpuProgramWrapperFactory::createTextureGpuProgramWrapper()
{
    return new DirectXTextureGpuProgramWrapper();
}

GpuProgramWrapper* GpuProgramWrapperFactory::createColorGpuProgramWrapper()
{
    return new DirectXGeometryGpuProgramWrapper();
}

GpuProgramWrapper* GpuProgramWrapperFactory::createFramebufferToScreenGpuProgramWrapper()
{
    return new DirectXFramebufferToScreenGpuProgramWrapper();
}
#endif

GpuProgramWrapperFactory::GpuProgramWrapperFactory()
{
    // Hide Constructor for Singleton
}
