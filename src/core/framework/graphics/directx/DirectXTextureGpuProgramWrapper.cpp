//
//  DirectXTextureGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/30/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "DirectXTextureGpuProgramWrapper.h"

#include "DirectXTextureProgram.h"
#include "DirectXManager.h"
#include "GpuTextureWrapper.h"
#include "FrameworkConstants.h"

DirectXTextureGpuProgramWrapper::DirectXTextureGpuProgramWrapper() : GpuProgramWrapper(),
_program(new DirectXTextureProgram(TEXTURE_VERTEX_SHADER, TEXTURE_FRAGMENT_SHADER))
{
    // Empty
}

DirectXTextureGpuProgramWrapper::~DirectXTextureGpuProgramWrapper()
{
    delete _program;
}

void DirectXTextureGpuProgramWrapper::bind()
{
    D3DManager->useNormalBlending();

    _program->bindShaders();
    
	_program->bindMatrix();

	_program->mapVertices();
}

void DirectXTextureGpuProgramWrapper::unbind()
{
	// Empty
}
