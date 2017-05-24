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
#include "Font.h"
#include "RoboMath.h"

#include "Assets.h"
#include "MainAssetsMapper.h"
#include "IRendererHelper.h"
#include "GameConstants.h"
#include "NGRect.h"
#include "Color.h"
#include "SpriteBatcher.h"
#include "TextureRegion.h"

#include "macros.h"
#include "NetworkManagerClient.h"
#include "HUD.h"
#include "World.h"
#include "RoboCat.h"
#include "StringUtils.h"

#include <sstream>

MainRenderer::MainRenderer(int maxBatchSize) : Renderer(maxBatchSize),
m_demo(new TextureWrapper("texture_001")),
m_misc(new TextureWrapper("texture_002")),
m_font(new Font("texture_002", 0, 0, 16, 64, 75, TEXTURE_SIZE_1024))
{
    ASSETS->init(new MainAssetsMapper());
}

MainRenderer::~MainRenderer()
{
	releaseDeviceDependentResources();

	delete m_font;

	delete m_demo;
	delete m_misc;
}

void MainRenderer::createDeviceDependentResources()
{
    Renderer::createDeviceDependentResources();
    
    loadTextureSync(m_demo);
    loadTextureSync(m_misc);
}

void MainRenderer::releaseDeviceDependentResources()
{
    Renderer::releaseDeviceDependentResources();

	unloadTexture(m_demo);
	unloadTexture(m_misc);
}

void MainRenderer::tempDraw(float stateTime)
{
    m_rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
    
    if (ensureTexture(m_demo)
        && ensureTexture(m_misc))
    {
        m_spriteBatcher->beginBatch();
        
        const auto& gameObjects = World::sInstance->GetGameObjects();
        
        for (GameObjectPtr go : gameObjects)
        {
            if (go->GetClassId() == 'MOUS')
            {
                TextureRegion tr = ASSETS->findTextureRegion("TopSecretFolder");
                m_spriteBatcher->drawSprite(go->GetLocation().m_fX, go->GetLocation().m_fY, go->GetScale(), go->GetScale(), 0, tr);
            }
            else if (go->GetClassId() == 'RCAT')
            {
                static Color c1 = Color(1, 1, 1, 1);
                static Color c2 = Color(1, 0, 0, 1);
                static Color c3 = Color(0, 1, 0, 1);
                static Color c4 = Color(0, 0, 1, 1);
                
                Color* c = &c1;
                switch (go->GetAsCat()->GetPlayerId())
                {
                    case 1:
                        c = &c1;
                        break;
                    case 2:
                        c = &c2;
                        break;
                    case 3:
                        c = &c3;
                        break;
                    case 4:
                        c = &c4;
                        break;
                    default:
                        break;
                }
                
                TextureRegion tr = ASSETS->findTextureRegion("CharacterHoldingGun", stateTime);
                m_spriteBatcher->drawSprite(go->GetLocation().m_fX, go->GetLocation().m_fY, go->GetScale(), go->GetScale(), RADIANS_TO_DEGREES(go->GetRotation()) - 90, *c, tr);
            }
            else if (go->GetClassId() == 'YARN')
            {
                TextureRegion tr = ASSETS->findTextureRegion("Pellet");
                m_spriteBatcher->drawSprite(go->GetLocation().m_fX, go->GetLocation().m_fY, go->GetScale(), go->GetScale(), 0, tr);
            }
        }
        m_spriteBatcher->endBatch(*m_demo->gpuTextureWrapper, *m_textureGpuProgramWrapper);
        
        m_spriteBatcher->beginBatch();
        RenderBandWidth();
        RenderRoundTripTime();
        RenderHealth();
        m_spriteBatcher->endBatch(*m_misc->gpuTextureWrapper, *m_textureGpuProgramWrapper);
    }
}

void MainRenderer::RenderHealth()
{
    if (HUD::sInstance->m_iHealth > 0)
    {
        std::string healthString = StringUtils::Sprintf("Health %d", HUD::sInstance->m_iHealth);
        RenderText(healthString, HUD::sInstance->m_iHealthOffset, Colors::Red);
    }
}

void MainRenderer::RenderBandWidth()
{
    std::string bandwidth = StringUtils::Sprintf("In %d  Bps, Out %d Bps",
                                            static_cast< int >(NetworkManagerClient::sInstance->GetBytesReceivedPerSecond().GetValue()),
                                            static_cast< int >(NetworkManagerClient::sInstance->GetBytesSentPerSecond().GetValue()));
    RenderText(bandwidth, HUD::sInstance->mBandwidthOrigin, Colors::White);
}

void MainRenderer::RenderRoundTripTime()
{
    float rttMS = NetworkManagerClient::sInstance->GetAvgRoundTripTime().GetValue() * 1000.f;
    
    std::string roundTripTime = StringUtils::Sprintf("RTT %d ms", (int) rttMS);
    RenderText(roundTripTime, HUD::sInstance->mRoundTripTimeOrigin, Colors::White);
}

void MainRenderer::RenderText(const std::string& inStr, const Vector3& origin, const Vector3& inColor)
{
    Color fontColor = Color(inColor.m_fX, inColor.m_fY, inColor.m_fZ, 1);
    float fgWidth = CAM_WIDTH / 60;
    float fgHeight = fgWidth * 1.171875f;
    
    std::stringstream ss;
    ss << inStr;
    std::string text = ss.str();
    
    m_font->renderText(*m_spriteBatcher, text, origin.m_fX, origin.m_fY, fgWidth, fgHeight, fontColor, false, false);
}
