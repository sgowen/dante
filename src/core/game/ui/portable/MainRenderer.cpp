//
//  MainRenderer.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "MainRenderer.h"

#include "TextureWrapper.h"

#include "Assets.h"
#include "MainAssetsMapper.h"
#include "IRendererHelper.h"
#include "GameConstants.h"
#include "NGRect.h"
#include "Color.h"
#include "SpriteBatcher.h"
#include "TextureRegion.h"

MainRenderer::MainRenderer(int maxBatchSize) : Renderer(maxBatchSize),
m_samus(new TextureWrapper("samus"))
{
    ASSETS->init(new MainAssetsMapper());
    
    loadTextureSync(m_samus);
}

MainRenderer::~MainRenderer()
{
    unloadTexture(m_samus);
}

void MainRenderer::createDeviceDependentResources()
{
    Renderer::createDeviceDependentResources();
}

void MainRenderer::releaseDeviceDependentResources()
{
    Renderer::releaseDeviceDependentResources();
}

void MainRenderer::tempDraw(float stateTime, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
    m_rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
    
    if (ensureTexture(m_samus))
    {
        m_spriteBatcher->beginBatch();
        {
            TextureRegion tr = ASSETS->findTextureRegion("Samus_Running", stateTime);
            m_spriteBatcher->drawSprite(x1, y1, 1.173913043478261f, 1.5f, 0, tr);
        }
        {
            TextureRegion tr = ASSETS->findTextureRegion("Samus_Running", stateTime);
            m_spriteBatcher->drawSprite(x2, y2, 1.173913043478261f, 1.5f, 0, tr);
        }
        {
            TextureRegion tr = ASSETS->findTextureRegion("Samus_Running", stateTime);
            m_spriteBatcher->drawSprite(x3, y3, 1.173913043478261f, 1.5f, 0, tr);
        }
        {
            TextureRegion tr = ASSETS->findTextureRegion("Samus_Running", stateTime);
            m_spriteBatcher->drawSprite(x4, y4, 1.173913043478261f, 1.5f, 0, tr);
        }
        m_spriteBatcher->endBatch(*m_samus->gpuTextureWrapper, *m_textureGpuProgramWrapper);
    }
}
