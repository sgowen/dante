//
//  DirectXGeometryGpuProgramWrapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/30/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXGeometryGpuProgramWrapper.h"

#include "framework/graphics/directx/DirectXGeometryProgram.h"
#include "framework/graphics/directx/DirectXRendererHelper.h"
#include "framework/util/FrameworkConstants.h"

DirectXGeometryGpuProgramWrapper::DirectXGeometryGpuProgramWrapper(DirectXRendererHelper* inRendererHelper) : GpuProgramWrapper(),
_program(new DirectXGeometryProgram(inRendererHelper, COLOR_VERTEX_SHADER, COLOR_FRAGMENT_SHADER))
{
    // Empty
}

DirectXGeometryGpuProgramWrapper::~DirectXGeometryGpuProgramWrapper()
{
    delete _program;
}

void DirectXGeometryGpuProgramWrapper::bind()
{
    _program->bindShaders();

	_program->bindMatrix();

    _program->mapVertices();
}

void DirectXGeometryGpuProgramWrapper::unbind()
{
	// Empty
}
