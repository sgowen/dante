//
//  MainRenderer.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "MainRenderer.h"

#include "Assets.h"
#include "MainAssetsMapper.h"

MainRenderer::MainRenderer(int maxBatchSize) : Renderer(maxBatchSize)
{
    ASSETS->init(new MainAssetsMapper());
}

MainRenderer::~MainRenderer()
{
    releaseDeviceDependentResources();
}

void MainRenderer::createDeviceDependentResources()
{
    Renderer::createDeviceDependentResources();
}

void MainRenderer::releaseDeviceDependentResources()
{
    Renderer::releaseDeviceDependentResources();
}

void MainRenderer::beginFrame()
{
    Renderer::beginFrame();
    
    setFramebuffer(0);
}

void MainRenderer::renderToScreen()
{
	/// Render everything to the screen
    
    renderFramebufferToScreen(m_iFramebufferIndex);
}
