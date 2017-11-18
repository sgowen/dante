//
//  DirectXLineBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/16/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "DirectXLineBatcher.h"

#include "Line.h"
#include "Vector2.h"
#include "DirectXProgramInput.h"
#include "DirectXManager.h"
#include "GpuProgramWrapper.h"
#include "DirectXGeometryGpuProgramWrapper.h"
#include "Color.h"

DirectXLineBatcher::DirectXLineBatcher() : LineBatcher()
{
	// Empty
}

void DirectXLineBatcher::beginBatch()
{
	D3DManager->getColorVertices().clear();
	_numLines = 0;
}

void DirectXLineBatcher::endBatch(GpuProgramWrapper &gpuProgramWrapper)
{
	if (_numLines > 0)
	{
		// set the primitive topology
		ID3D11DeviceContext* d3dContext = DirectXManager::getD3dContext();
		d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		gpuProgramWrapper.bind();

		d3dContext->Draw(_numLines * VERTICES_PER_LINE, 0);

		gpuProgramWrapper.unbind();
	}
}

void DirectXLineBatcher::renderLine(float originX, float originY, float endX, float endY, Color &color)
{
	D3DManager->addVertexCoordinate(originX, originY, 0, color.red, color.green, color.blue, color.alpha);
	D3DManager->addVertexCoordinate(endX, endY, 0, color.red, color.green, color.blue, color.alpha);

	_numLines++;
}
