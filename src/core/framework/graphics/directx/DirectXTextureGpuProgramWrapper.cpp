//
//  DirectXTextureGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/30/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXTextureGpuProgramWrapper.h"

#include "framework/graphics/directx/DirectXTextureProgram.h"
#include "framework/graphics/directx/DirectXManager.h"
#include "framework/graphics/portable/GpuTextureWrapper.h"
#include "framework/util/FrameworkConstants.h"

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
    DXManager->useNormalBlending();

    _program->bindShaders();
    
	_program->bindMatrix();

	_program->mapVertices();
}

void DirectXTextureGpuProgramWrapper::unbind()
{
	// Empty
}
