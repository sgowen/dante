//
//  OpenGLFramebufferToScreenGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/opengl/OpenGLFramebufferToScreenGpuProgramWrapper.h"

#include <framework/graphics/portable/RendererHelper.h>
#include "framework/graphics/opengl/OpenGLFramebufferToScreenProgram.h"

#include "framework/util/FrameworkConstants.h"

OpenGLFramebufferToScreenGpuProgramWrapper::OpenGLFramebufferToScreenGpuProgramWrapper(OpenGLRendererHelper* inRendererHelper) : GpuProgramWrapper(), _program(new OpenGLFramebufferToScreenProgram(inRendererHelper, FRAMEBUFFER_TO_SCREEN_VERTEX_SHADER, FRAMEBUFFER_TO_SCREEN_FRAGMENT_SHADER))
{
    // Empty
}

OpenGLFramebufferToScreenGpuProgramWrapper::~OpenGLFramebufferToScreenGpuProgramWrapper()
{
    delete _program;
}

void OpenGLFramebufferToScreenGpuProgramWrapper::bind()
{
    _program->bind();
}

void OpenGLFramebufferToScreenGpuProgramWrapper::unbind()
{
    _program->unbind();
}
