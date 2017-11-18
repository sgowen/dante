//
//  GpuProgramWrapperFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "GpuProgramWrapperFactory.h"

GpuProgramWrapperFactory* GpuProgramWrapperFactory::getInstance()
{
    static GpuProgramWrapperFactory instance = GpuProgramWrapperFactory();
    return &instance;
}

#if defined __APPLE__ || defined __ANDROID__

#include "OpenGLTextureGpuProgramWrapper.h"
#include "OpenGLGeometryGpuProgramWrapper.h"
#include "OpenGLFramebufferToScreenGpuProgramWrapper.h"

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

#include "DirectXTextureGpuProgramWrapper.h"
#include "DirectXGeometryGpuProgramWrapper.h"
#include "DirectXFramebufferToScreenGpuProgramWrapper.h"

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
