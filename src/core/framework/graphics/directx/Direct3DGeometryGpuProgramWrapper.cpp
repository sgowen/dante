//
//  Direct3DGeometryGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/30/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Direct3DGeometryGpuProgramWrapper.h"

#include "Direct3DGeometryProgram.h"
#include "Direct3DManager.h"
#include "FrameworkConstants.h"

Direct3DGeometryGpuProgramWrapper::Direct3DGeometryGpuProgramWrapper() : GpuProgramWrapper(),
_program(new Direct3DGeometryProgram(COLOR_VERTEX_SHADER, COLOR_FRAGMENT_SHADER))
{
    // Empty
}

Direct3DGeometryGpuProgramWrapper::~Direct3DGeometryGpuProgramWrapper()
{
    delete _program;
}

void Direct3DGeometryGpuProgramWrapper::bind()
{
    D3DManager->useNormalBlending();

	_program->bindShaders();

	_program->bindMatrix();

    _program->mapVertices();
}

void Direct3DGeometryGpuProgramWrapper::unbind()
{
	// Empty
}
