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
_program(new Direct3DTextureProgram(TEXTURE_VERTEX_SHADER, TEXTURE_FRAGMENT_SHADER))
{
    // Empty
}

Direct3DTextureGpuProgramWrapper::~Direct3DTextureGpuProgramWrapper()
{
    delete _program;
}

void Direct3DTextureGpuProgramWrapper::bind()
{
    D3DManager->useNormalBlending();

    _program->bindShaders();
    
	_program->bindMatrix();

	_program->mapVertices();
}

void Direct3DTextureGpuProgramWrapper::unbind()
{
	// Empty
}
