//
//  OpenGLFramebufferToScreenGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "OpenGLFramebufferToScreenGpuProgramWrapper.h"

#include "OpenGLFramebufferToScreenProgram.h"
#include "OpenGLManager.h"
#include "FrameworkConstants.h"
#include "FrameworkConstants.h"
#include "GameConstants.h"

OpenGLFramebufferToScreenGpuProgramWrapper::OpenGLFramebufferToScreenGpuProgramWrapper() : GpuProgramWrapper(), m_program(new OpenGLFramebufferToScreenProgram(FRAMEBUFFER_TO_SCREEN_VERTEX_SHADER, FRAMEBUFFER_TO_SCREEN_FRAGMENT_SHADER))
{
    // Empty
}

OpenGLFramebufferToScreenGpuProgramWrapper::~OpenGLFramebufferToScreenGpuProgramWrapper()
{
    delete m_program;
}

void OpenGLFramebufferToScreenGpuProgramWrapper::bind()
{
    OGLManager->useScreenBlending();
    
    m_program->bind();
}

void OpenGLFramebufferToScreenGpuProgramWrapper::unbind()
{
    m_program->unbind();
}
