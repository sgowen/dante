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

Direct3DGeometryGpuProgramWrapper::Direct3DGeometryGpuProgramWrapper() : GpuProgramWrapper(),
m_program(new Direct3DGeometryProgram(L"ColorVertexShader.cso", L"ColorPixelShader.cso"))
{
    // Empty
}

Direct3DGeometryGpuProgramWrapper::~Direct3DGeometryGpuProgramWrapper()
{
    delete m_program;
}

void Direct3DGeometryGpuProgramWrapper::bind()
{
    D3DManager->useNormalBlending();

	m_program->bindShaders();

	m_program->bindMatrix();

    m_program->mapVertices();
}

void Direct3DGeometryGpuProgramWrapper::unbind()
{
	// Empty
}
