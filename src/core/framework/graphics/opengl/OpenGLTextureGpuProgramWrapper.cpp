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
#include "FrameworkConstants.h"
#include "FrameworkConstants.h"
#include "GameConstants.h"

OpenGLTextureGpuProgramWrapper::OpenGLTextureGpuProgramWrapper() : GpuProgramWrapper(), _program(new OpenGLTextureProgram(TEXTURE_VERTEX_SHADER, TEXTURE_FRAGMENT_SHADER))
{
    // Empty
}

OpenGLTextureGpuProgramWrapper::~OpenGLTextureGpuProgramWrapper()
{
    delete _program;
}

void OpenGLTextureGpuProgramWrapper::bind()
{
    OGLManager->useNormalBlending();
    
    _program->bind();
}

void OpenGLTextureGpuProgramWrapper::unbind()
{
    _program->unbind();
}
