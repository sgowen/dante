//
//  Direct3DSpriteBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Direct3DSpriteBatcher.h"

#include "TextureWrapper.h"
#include "GpuTextureWrapper.h"
#include "GpuProgramWrapper.h"
#include "TextureRegion.h"
#include "Color.h"

#include "Direct3DManager.h"

Direct3DSpriteBatcher::Direct3DSpriteBatcher() : SpriteBatcher()
{
    // Empty
}

void Direct3DSpriteBatcher::beginBatch()
{
	D3DManager->getTextureVertices().clear();
    
	_numSprites = 0;
}

void Direct3DSpriteBatcher::endBatch(TextureWrapper& textureWrapper, GpuProgramWrapper& gpuProgramWrapper)
{
	if (_numSprites > 0)
	{
		// tell the GPU which texture to use
		ID3D11DeviceContext* d3dContext = Direct3DManager::getD3dContext();
		d3dContext->PSSetShaderResources(0, 1, &textureWrapper.gpuTextureWrapper->texture);
        
        if (textureWrapper.repeatS)
        {
            bindWrapSamplerState();
        }
        else
        {
            bindNormalSamplerState();
        }

		d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		gpuProgramWrapper.bind();

		d3dContext->DrawIndexed(_numSprites * INDICES_PER_RECTANGLE, 0, 0);

		gpuProgramWrapper.unbind();

		ID3D11ShaderResourceView *pSRV[1] = { NULL };
		d3dContext->PSSetShaderResources(0, 1, pSRV);
	}
}

void Direct3DSpriteBatcher::addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a, float u, float v)
{
    D3DManager->addVertexCoordinate(x, y, z, r, g, b, a, u, v);
}

void Direct3DSpriteBatcher::bindNormalSamplerState()
{
    ID3D11DeviceContext* d3dContext = Direct3DManager::getD3dContext();
    
    d3dContext->PSSetSamplers(0, 1, D3DManager->getSbSamplerState().GetAddressOf());
}

void Direct3DSpriteBatcher::bindWrapSamplerState()
{
    ID3D11DeviceContext* d3dContext = Direct3DManager::getD3dContext();
    
    d3dContext->PSSetSamplers(0, 1, D3DManager->getSbWrapSamplerState().GetAddressOf());
}
