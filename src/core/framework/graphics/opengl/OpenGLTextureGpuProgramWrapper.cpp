//
//  OpenGLTextureGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/opengl/OpenGLTextureGpuProgramWrapper.h"

#include <framework/graphics/portable/RendererHelper.h>
#include "framework/graphics/opengl/OpenGLTextureProgram.h"

#include "framework/util/FrameworkConstants.h"

OpenGLTextureGpuProgramWrapper::OpenGLTextureGpuProgramWrapper(OpenGLRendererHelper* inRendererHelper) : GpuProgramWrapper(), _program(new OpenGLTextureProgram(inRendererHelper, TEXTURE_VERTEX_SHADER, TEXTURE_FRAGMENT_SHADER))
{
    // Empty
}

OpenGLTextureGpuProgramWrapper::~OpenGLTextureGpuProgramWrapper()
{
    delete _program;
}

void OpenGLTextureGpuProgramWrapper::bind()
{
    _program->bind();
}

void OpenGLTextureGpuProgramWrapper::unbind()
{
    _program->unbind();
}
