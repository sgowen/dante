//
//  GpuProgramWrapperFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/GpuProgramWrapperFactory.h"
#include "framework/graphics/portable/RendererHelper.h"

GpuProgramWrapperFactory* GpuProgramWrapperFactory::getInstance()
{
    static GpuProgramWrapperFactory instance = GpuProgramWrapperFactory();
    return &instance;
}

#if defined __APPLE__ || defined __ANDROID__ || defined __linux__

#include "framework/graphics/opengl/OpenGLRendererHelper.h"
#include "framework/graphics/opengl/OpenGLTextureGpuProgramWrapper.h"
#include "framework/graphics/opengl/OpenGLGeometryGpuProgramWrapper.h"
#include "framework/graphics/opengl/OpenGLFramebufferToScreenGpuProgramWrapper.h"

GpuProgramWrapper* GpuProgramWrapperFactory::createTextureGpuProgramWrapper(RendererHelper* inRendererHelper)
{
    return new OpenGLTextureGpuProgramWrapper(static_cast<OpenGLRendererHelper*>(inRendererHelper));
}

GpuProgramWrapper* GpuProgramWrapperFactory::createColorGpuProgramWrapper(RendererHelper* inRendererHelper)
{
    return new OpenGLGeometryGpuProgramWrapper(static_cast<OpenGLRendererHelper*>(inRendererHelper));
}

GpuProgramWrapper* GpuProgramWrapperFactory::createFramebufferToScreenGpuProgramWrapper(RendererHelper* inRendererHelper)
{
    return new OpenGLFramebufferToScreenGpuProgramWrapper(static_cast<OpenGLRendererHelper*>(inRendererHelper));
}
#elif defined _WIN32

#include "framework/graphics/directx/DirectXRendererHelper.h"
#include "framework/graphics/directx/DirectXTextureGpuProgramWrapper.h"
#include "framework/graphics/directx/DirectXGeometryGpuProgramWrapper.h"
#include "framework/graphics/directx/DirectXFramebufferToScreenGpuProgramWrapper.h"

GpuProgramWrapper* GpuProgramWrapperFactory::createTextureGpuProgramWrapper(RendererHelper* inRendererHelper)
{
    return new DirectXTextureGpuProgramWrapper(static_cast<DirectXRendererHelper*>(inRendererHelper));
}

GpuProgramWrapper* GpuProgramWrapperFactory::createColorGpuProgramWrapper(RendererHelper* inRendererHelper)
{
    return new DirectXGeometryGpuProgramWrapper(static_cast<DirectXRendererHelper*>(inRendererHelper));
}

GpuProgramWrapper* GpuProgramWrapperFactory::createFramebufferToScreenGpuProgramWrapper(RendererHelper* inRendererHelper)
{
    return new DirectXFramebufferToScreenGpuProgramWrapper(static_cast<DirectXRendererHelper*>(inRendererHelper));
}
#endif

GpuProgramWrapperFactory::GpuProgramWrapperFactory()
{
    // Hide Constructor for Singleton
}
