//
//  Direct3DFramebufferToScreenGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/7/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Direct3DFramebufferToScreenGpuProgramWrapper.h"

#include "Direct3DTextureProgram.h"
#include "Direct3DManager.h"
#include "GpuTextureWrapper.h"
#include "FrameworkConstants.h"

Direct3DFramebufferToScreenGpuProgramWrapper::Direct3DFramebufferToScreenGpuProgramWrapper() : GpuProgramWrapper(),
m_program(new Direct3DTextureProgram(FRAMEBUFFER_TO_SCREEN_VERTEX_SHADER, FRAMEBUFFER_TO_SCREEN_FRAGMENT_SHADER))
{
    // Empty
}

Direct3DFramebufferToScreenGpuProgramWrapper::~Direct3DFramebufferToScreenGpuProgramWrapper()
{
    delete m_program;
}

void Direct3DFramebufferToScreenGpuProgramWrapper::bind()
{
    D3DManager->useScreenBlending();

    m_program->bindShaders();
    
    m_program->bindNormalSamplerState();

	m_program->mapVertices();
}

void Direct3DFramebufferToScreenGpuProgramWrapper::unbind()
{
	// Empty
}
