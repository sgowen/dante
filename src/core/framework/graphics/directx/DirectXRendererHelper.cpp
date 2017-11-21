//
//  DirectXRendererHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXRendererHelper.h"

#include "framework/graphics/portable/TextureWrapper.h"
#include "framework/graphics/portable/GpuTextureWrapper.h"
#include "framework/graphics/directx/DirectXManager.h"

DirectXRendererHelper::DirectXRendererHelper() : RendererHelper(), _framebufferIndex(0), _isBoundToScreen(false)
{
	DirectXManager::create();
}

DirectXRendererHelper::~DirectXRendererHelper()
{
	DirectXManager::destroy();
}

void DirectXRendererHelper::createDeviceDependentResources(int maxBatchSize)
{
	DXManager->createDeviceDependentResources(maxBatchSize);
}

void DirectXRendererHelper::createWindowSizeDependentResources(int renderWidth, int renderHeight, int numFramebuffers)
{
	DXManager->createWindowSizeDependentResources(renderWidth, renderHeight, numFramebuffers);
}

void DirectXRendererHelper::releaseDeviceDependentResources()
{
	DXManager->releaseDeviceDependentResources();
}

void DirectXRendererHelper::beginFrame()
{
    // Empty
}

void DirectXRendererHelper::endFrame()
{
    // Empty
}

TextureWrapper* DirectXRendererHelper::getFramebuffer(int index)
{
    _framebuffer->gpuTextureWrapper = DXManager->getFramebuffers().at(index);
    
    return _framebuffer;
}

void DirectXRendererHelper::updateMatrix(float left, float right, float bottom, float top)
{
    DXManager->updateMatrix(left, right, bottom, top);
}

void DirectXRendererHelper::bindToOffscreenFramebuffer(int index)
{
	ID3D11DeviceContext* d3dContext = DirectXManager::getD3dContext();

	d3dContext->OMSetRenderTargets(1, &DXManager->getOffscreenRenderTargetViews().at(index), NULL);
    
	_framebufferIndex = index;
    _isBoundToScreen = false;
}

void DirectXRendererHelper::clearFramebufferWithColor(float r, float g, float b, float a)
{
    float color[] = { r, g, b, a };

    ID3D11RenderTargetView * targets[1] = {};
    if (_isBoundToScreen)
    {
		ID3D11RenderTargetView* d3dRenderTargetView = DirectXManager::getD3dRenderTargetView();
        targets[0] = d3dRenderTargetView;
    }
    else
    {
        targets[0] = DXManager->getOffscreenRenderTargetViews().at(_framebufferIndex);
    }
    
	ID3D11DeviceContext* d3dContext = DirectXManager::getD3dContext();

	d3dContext->ClearRenderTargetView(targets[0], color);
}

void DirectXRendererHelper::bindToScreenFramebuffer()
{
	ID3D11DeviceContext* d3dContext = DirectXManager::getD3dContext();

	ID3D11RenderTargetView* d3dRenderTargetView = DirectXManager::getD3dRenderTargetView();

    ID3D11RenderTargetView *const targets[1] = { d3dRenderTargetView };
	d3dContext->OMSetRenderTargets(1, targets, NULL);
    
    _isBoundToScreen = true;
}

void DirectXRendererHelper::destroyTexture(GpuTextureWrapper& textureWrapper)
{
    if (textureWrapper.texture)
    {
        textureWrapper.texture->Release();
    }
}