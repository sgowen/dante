//
//  DirectXFramebufferToScreenGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/7/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "DirectXFramebufferToScreenGpuProgramWrapper.h"

#include "DirectXTextureProgram.h"
#include "DirectXManager.h"
#include "GpuTextureWrapper.h"
#include "FrameworkConstants.h"

DirectXFramebufferToScreenGpuProgramWrapper::DirectXFramebufferToScreenGpuProgramWrapper() : GpuProgramWrapper(),
_program(new DirectXTextureProgram(FRAMEBUFFER_TO_SCREEN_VERTEX_SHADER, FRAMEBUFFER_TO_SCREEN_FRAGMENT_SHADER))
{
    // Empty
}

DirectXFramebufferToScreenGpuProgramWrapper::~DirectXFramebufferToScreenGpuProgramWrapper()
{
    delete _program;
}

void DirectXFramebufferToScreenGpuProgramWrapper::bind()
{
    D3DManager->useScreenBlending();

    _program->bindShaders();

	_program->mapVertices();
}

void DirectXFramebufferToScreenGpuProgramWrapper::unbind()
{
	// Empty
}
