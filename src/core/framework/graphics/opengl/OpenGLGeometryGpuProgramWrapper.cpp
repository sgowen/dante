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
#include "FrameworkConstants.h"
#include "FrameworkConstants.h"
#include "GameConstants.h"

OpenGLGeometryGpuProgramWrapper::OpenGLGeometryGpuProgramWrapper() : GpuProgramWrapper(), _program(new OpenGLGeometryProgram(COLOR_VERTEX_SHADER, COLOR_FRAGMENT_SHADER))
{
    // Empty
}

OpenGLGeometryGpuProgramWrapper::~OpenGLGeometryGpuProgramWrapper()
{
    delete _program;
}

void OpenGLGeometryGpuProgramWrapper::bind()
{
    OGLManager->useNormalBlending();
    
    _program->bind();
}

void OpenGLGeometryGpuProgramWrapper::unbind()
{
    _program->unbind();
}
