//
//  Direct3DTextureGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/30/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Direct3DTextureGpuProgramWrapper.h"

#include "Direct3DTextureProgram.h"
#include "Direct3DManager.h"
#include "GpuTextureWrapper.h"
#include "FrameworkConstants.h"

Direct3DTextureGpuProgramWrapper::Direct3DTextureGpuProgramWrapper() : GpuProgramWrapper(),
m_program(new Direct3DTextureProgram(TEXTURE_VERTEX_SHADER, TEXTURE_FRAGMENT_SHADER))
{
    // Empty
}

Direct3DTextureGpuProgramWrapper::~Direct3DTextureGpuProgramWrapper()
{
    delete m_program;
}

void Direct3DTextureGpuProgramWrapper::bind()
{
    D3DManager->useNormalBlending();

    m_program->bindShaders();
    
    m_program->bindNormalSamplerState();
    
	m_program->bindMatrix();

	m_program->mapVertices();
}

void Direct3DTextureGpuProgramWrapper::unbind()
{
	// Empty
}
