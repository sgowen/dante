//
//  DirectXCircleBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/16/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXCircleBatcher.h"

#include "framework/math/Circle.h"
#include "framework/graphics/portable/GpuProgramWrapper.h"
#include "framework/math/Color.h"

#include "framework/graphics/directx/DirectXProgramInput.h"
#include "framework/graphics/directx/DirectXManager.h"

DirectXCircleBatcher::DirectXCircleBatcher() : CircleBatcher()
{
	// Empty
}

void DirectXCircleBatcher::clearVertices()
{
    DXManager->getColorVertices().clear();
}

void DirectXCircleBatcher::addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a)
{
    DXManager->addVertexCoordinate(x, y, z, r, g, b, a);
    
    _numPoints++;
}

void DirectXCircleBatcher::endBatch(GpuProgramWrapper &gpuProgramWrapper)
{
    gpuProgramWrapper.bind();

    ID3D11DeviceContext* d3dContext = DirectXManager::getD3dContext();
    d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    d3dContext->Draw(_numPoints, 0);

	gpuProgramWrapper.unbind();
}
