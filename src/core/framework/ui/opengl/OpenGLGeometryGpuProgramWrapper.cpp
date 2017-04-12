//
//  OpenGLGeometryGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "OpenGLGeometryGpuProgramWrapper.h"

#include "OpenGLGeometryProgram.h"
#include "OpenGLManager.h"

OpenGLGeometryGpuProgramWrapper::OpenGLGeometryGpuProgramWrapper() : GpuProgramWrapper(), m_program(new OpenGLGeometryProgram("color_shader.vsh", "color_shader.fsh"))
{
    // Empty
}

OpenGLGeometryGpuProgramWrapper::~OpenGLGeometryGpuProgramWrapper()
{
    delete m_program;
}

void OpenGLGeometryGpuProgramWrapper::bind()
{
    OGLManager->useNormalBlending();
    
    m_program->bind();
}

void OpenGLGeometryGpuProgramWrapper::unbind()
{
    m_program->unbind();
}
