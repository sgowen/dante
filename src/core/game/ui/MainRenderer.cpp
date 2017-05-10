//
//  MainRenderer.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

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

#include "World.h"
#include "macros.h"

MainRenderer::MainRenderer(int maxBatchSize) : Renderer(maxBatchSize),
m_demo(new TextureWrapper("texture_001"))
{
    ASSETS->init(new MainAssetsMapper());
    
    loadTextureSync(m_demo);
}

MainRenderer::~MainRenderer()
{
    unloadTexture(m_demo);
}

void MainRenderer::createDeviceDependentResources()
{
    Renderer::createDeviceDependentResources();
}

void MainRenderer::releaseDeviceDependentResources()
{
    Renderer::releaseDeviceDependentResources();
}

void MainRenderer::tempDraw(float stateTime)
{
    m_rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
    
    if (ensureTexture(m_demo))
    {
        m_spriteBatcher->beginBatch();
        
        const auto& gameObjects = World::sInstance->GetGameObjects();
        
        for (GameObjectPtr go : gameObjects)
        {
            if (go->GetClassId() == 'MOUS')
            {
                TextureRegion tr = ASSETS->findTextureRegion("TopSecretFolder");
                m_spriteBatcher->drawSprite(go->GetLocation().mX, go->GetLocation().mY, go->GetScale(), go->GetScale(), 0, tr);
            }
            else if (go->GetClassId() == 'RCAT')
            {
                static Color c1 = Color(1, 1, 1, 1);
                static Color c2 = Color(1, 0, 0, 1);
                static Color c3 = Color(0, 1, 0, 1);
                static Color c4 = Color(0, 0, 1, 1);
                
                Color* c = &c1;
                switch (go->GetNetworkId())
                {
                    case 21:
                        c = &c1;
                        break;
                    case 22:
                        c = &c2;
                        break;
                    case 23:
                        c = &c3;
                        break;
                    case 24:
                        c = &c4;
                        break;
                    default:
                        break;
                }
                
                TextureRegion tr = ASSETS->findTextureRegion("CharacterHoldingGun", stateTime);
                m_spriteBatcher->drawSprite(go->GetLocation().mX, go->GetLocation().mY, go->GetScale(), go->GetScale(), RADIANS_TO_DEGREES(go->GetRotation()) - 90, *c, tr);
            }
            else if (go->GetClassId() == 'YARN')
            {
                TextureRegion tr = ASSETS->findTextureRegion("Pellet");
                m_spriteBatcher->drawSprite(go->GetLocation().mX, go->GetLocation().mY, go->GetScale(), go->GetScale(), 0, tr);
            }
        }
        m_spriteBatcher->endBatch(*m_demo->gpuTextureWrapper, *m_textureGpuProgramWrapper);
    }
}
