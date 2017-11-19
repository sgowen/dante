//
//  DirectXNGRectBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXNGRectBatcher.h"

#include "framework/math/NGRect.h"
#include "framework/math/Vector2.h"
#include "framework/graphics/directx/DirectXProgramInput.h"
#include "framework/graphics/directx/DirectXManager.h"
#include "framework/graphics/portable/GpuProgramWrapper.h"
#include "framework/graphics/directx/DirectXGeometryGpuProgramWrapper.h"
#include "framework/math/Color.h"

DirectXNGRectBatcher::DirectXNGRectBatcher(bool isFill) : NGRectBatcher(isFill)
{
    // Empty
}

void DirectXNGRectBatcher::beginBatch()
{
	DXManager->getColorVertices().clear();
	_numNGRects = 0;
}

void DirectXNGRectBatcher::endBatch(GpuProgramWrapper &gpuProgramWrapper)
{
	if (_numNGRects > 0)
	{
		// set the primitive topology
		ID3D11DeviceContext* d3dContext = DirectXManager::getD3dContext();
		d3dContext->IASetPrimitiveTopology(_isFill ? D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST : D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		gpuProgramWrapper.bind();

		d3dContext->DrawIndexed(_numNGRects * INDICES_PER_RECTANGLE, 0, 0);

		gpuProgramWrapper.unbind();
	}
}

void DirectXNGRectBatcher::renderNGRect(NGRect &rectangle, Color &color)
{
	float x1 = rectangle.getLeft();
	float y1 = rectangle.getBottom();
	float x2 = x1 + rectangle.getWidth();
	float y2 = y1 + rectangle.getHeight();

	renderNGRect(x1, y1, x2, y2, color);
}

void DirectXNGRectBatcher::renderNGRect(float x1, float y1, float x2, float y2, Color &color)
{
	DXManager->addVertexCoordinate(x1, y1, 0, color.red, color.green, color.blue, color.alpha);
	DXManager->addVertexCoordinate(x1, y2, 0, color.red, color.green, color.blue, color.alpha);
	DXManager->addVertexCoordinate(x2, y2, 0, color.red, color.green, color.blue, color.alpha);
	DXManager->addVertexCoordinate(x2, y1, 0, color.red, color.green, color.blue, color.alpha);

	_numNGRects++;
}
