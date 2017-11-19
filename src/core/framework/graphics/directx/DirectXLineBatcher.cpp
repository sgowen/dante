//
//  DirectXLineBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/16/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXLineBatcher.h"

#include "framework/math/Line.h"
#include "framework/math/Vector2.h"
#include "framework/graphics/directx/DirectXProgramInput.h"
#include "framework/graphics/directx/DirectXManager.h"
#include "framework/graphics/portable/GpuProgramWrapper.h"
#include "framework/graphics/directx/DirectXGeometryGpuProgramWrapper.h"
#include "framework/math/Color.h"

DirectXLineBatcher::DirectXLineBatcher() : LineBatcher()
{
	// Empty
}

void DirectXLineBatcher::beginBatch()
{
	DXManager->getColorVertices().clear();
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
	DXManager->addVertexCoordinate(originX, originY, 0, color.red, color.green, color.blue, color.alpha);
	DXManager->addVertexCoordinate(endX, endY, 0, color.red, color.green, color.blue, color.alpha);

	_numLines++;
}
