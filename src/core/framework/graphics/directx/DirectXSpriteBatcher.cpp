//
//  DirectXSpriteBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXSpriteBatcher.h"

#include "framework/graphics/portable/TextureWrapper.h"
#include "framework/graphics/portable/GpuTextureWrapper.h"
#include "framework/graphics/portable/GpuProgramWrapper.h"
#include "framework/graphics/portable/TextureRegion.h"
#include "framework/math/Color.h"

#include "framework/graphics/directx/DirectXManager.h"

DirectXSpriteBatcher::DirectXSpriteBatcher() : SpriteBatcher()
{
    // Empty
}

void DirectXSpriteBatcher::beginBatch()
{
	DXManager->getTextureVertices().clear();
    
	_numSprites = 0;
}

void DirectXSpriteBatcher::endBatch(TextureWrapper& textureWrapper, GpuProgramWrapper& gpuProgramWrapper)
{
	if (_numSprites > 0)
	{
		// tell the GPU which texture to use
		ID3D11DeviceContext* d3dContext = DirectXManager::getD3dContext();
		d3dContext->PSSetShaderResources(0, 1, &textureWrapper.gpuTextureWrapper->texture);
        d3dContext->PSSetSamplers(0, 1, textureWrapper._repeatS ? DXManager->getSbWrapSamplerState().GetAddressOf() : DXManager->getSbSamplerState().GetAddressOf());

		gpuProgramWrapper.bind();

        d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		d3dContext->DrawIndexed(_numSprites * INDICES_PER_RECTANGLE, 0, 0);

		gpuProgramWrapper.unbind();

		ID3D11ShaderResourceView *pSRV[1] = { NULL };
		d3dContext->PSSetShaderResources(0, 1, pSRV);
	}
}

void DirectXSpriteBatcher::addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a, float u, float v)
{
    DXManager->addVertexCoordinate(x, y, z, r, g, b, a, u, v);
}
