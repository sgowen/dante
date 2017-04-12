//
//  OpenGLTextureGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "OpenGLTextureGpuProgramWrapper.h"

#include "OpenGLTextureProgram.h"
#include "OpenGLManager.h"

OpenGLTextureGpuProgramWrapper::OpenGLTextureGpuProgramWrapper() : GpuProgramWrapper(), m_program(new OpenGLTextureProgram("texture_shader.vsh", "texture_shader.fsh"))
{
    // Empty
}

OpenGLTextureGpuProgramWrapper::~OpenGLTextureGpuProgramWrapper()
{
    delete m_program;
}

void OpenGLTextureGpuProgramWrapper::bind()
{
    OGLManager->useNormalBlending();
    
    m_program->bind();
}

void OpenGLTextureGpuProgramWrapper::unbind()
{
    m_program->unbind();
}
