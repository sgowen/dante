//
//  OpenGLGeometryGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/opengl/OpenGLGeometryGpuProgramWrapper.h"

#include "framework/graphics/opengl/OpenGLGeometryProgram.h"
#include "framework/graphics/opengl/OpenGLManager.h"
#include "framework/util/FrameworkConstants.h"
#include "framework/util/FrameworkConstants.h"

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
