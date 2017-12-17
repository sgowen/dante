//
//  GpuProgramFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/GpuProgramFactory.h"
#include "framework/graphics/portable/RendererHelper.h"
#include "framework/util/FrameworkConstants.h"

GpuProgramFactory* GpuProgramFactory::getInstance()
{
    static GpuProgramFactory instance = GpuProgramFactory();
    return &instance;
}

#if defined __APPLE__ || defined __ANDROID__ || defined __linux__

#include "framework/graphics/opengl/OpenGLRendererHelper.h"
#include "framework/graphics/opengl/OpenGLTextureProgram.h"
#include "framework/graphics/opengl/OpenGLGeometryProgram.h"
#include "framework/graphics/opengl/OpenGLFramebufferToScreenProgram.h"

GpuProgram* GpuProgramFactory::createTextureGpuProgram(RendererHelper* inRendererHelper)
{
    return new OpenGLTextureProgram(static_cast<OpenGLRendererHelper*>(inRendererHelper), TEXTURE_VERTEX_SHADER, TEXTURE_FRAGMENT_SHADER);
}

GpuProgram* GpuProgramFactory::createColorGpuProgram(RendererHelper* inRendererHelper)
{
    return new OpenGLGeometryProgram(static_cast<OpenGLRendererHelper*>(inRendererHelper), COLOR_VERTEX_SHADER, COLOR_FRAGMENT_SHADER);
}

GpuProgram* GpuProgramFactory::createFramebufferToScreenGpuProgram(RendererHelper* inRendererHelper)
{
    return new OpenGLFramebufferToScreenProgram(static_cast<OpenGLRendererHelper*>(inRendererHelper), FRAMEBUFFER_TO_SCREEN_VERTEX_SHADER, FRAMEBUFFER_TO_SCREEN_FRAGMENT_SHADER);
}
#elif defined _WIN32

#include "framework/graphics/directx/DirectXRendererHelper.h"
#include "framework/graphics/directx/DirectXTextureProgram.h"
#include "framework/graphics/directx/DirectXGeometryProgram.h"
#include "framework/graphics/directx/DirectXFramebufferToScreenProgram.h"

GpuProgram* GpuProgramFactory::createTextureGpuProgram(RendererHelper* inRendererHelper)
{
    return new DirectXTextureProgram(static_cast<DirectXRendererHelper*>(inRendererHelper), TEXTURE_VERTEX_SHADER, TEXTURE_FRAGMENT_SHADER);
}

GpuProgram* GpuProgramFactory::createColorGpuProgram(RendererHelper* inRendererHelper)
{
    return new DirectXGeometryProgram(static_cast<DirectXRendererHelper*>(inRendererHelper), COLOR_VERTEX_SHADER, COLOR_FRAGMENT_SHADER);
}

GpuProgram* GpuProgramFactory::createFramebufferToScreenGpuProgram(RendererHelper* inRendererHelper)
{
    return new DirectXFramebufferToScreenProgram(static_cast<DirectXRendererHelper*>(inRendererHelper), FRAMEBUFFER_TO_SCREEN_VERTEX_SHADER, FRAMEBUFFER_TO_SCREEN_FRAGMENT_SHADER);
}
#endif

GpuProgramFactory::GpuProgramFactory()
{
    // Hide Constructor for Singleton
}
