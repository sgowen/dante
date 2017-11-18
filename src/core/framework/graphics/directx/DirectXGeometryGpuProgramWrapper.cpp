//
//  DirectXGeometryGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/30/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "DirectXGeometryGpuProgramWrapper.h"

#include "DirectXGeometryProgram.h"
#include "DirectXManager.h"
#include "FrameworkConstants.h"

DirectXGeometryGpuProgramWrapper::DirectXGeometryGpuProgramWrapper() : GpuProgramWrapper(),
_program(new DirectXGeometryProgram(COLOR_VERTEX_SHADER, COLOR_FRAGMENT_SHADER))
{
    // Empty
}

DirectXGeometryGpuProgramWrapper::~DirectXGeometryGpuProgramWrapper()
{
    delete _program;
}

void DirectXGeometryGpuProgramWrapper::bind()
{
    D3DManager->useNormalBlending();

	_program->bindShaders();

	_program->bindMatrix();

    _program->mapVertices();
}

void DirectXGeometryGpuProgramWrapper::unbind()
{
	// Empty
}
