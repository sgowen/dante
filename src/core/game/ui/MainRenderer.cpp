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

#include <sstream>

MainRenderer::MainRenderer(int maxBatchSize) : Renderer(maxBatchSize),
m_demo(new TextureWrapper("texture_001")),
m_misc(new TextureWrapper("texture_002")),
m_font(new Font("texture_002", 0, 0, 16, 64, 75, TEXTURE_SIZE_1024))
{
    ASSETS->init(new MainAssetsMapper());
    
    loadTextureSync(m_demo);
    loadTextureSync(m_misc);
}

MainRenderer::~MainRenderer()
{
    unloadTexture(m_demo);
    unloadTexture(m_misc);
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
                m_spriteBatcher->drawSprite(go->GetLocation().mX, go->GetLocation().mY, go->GetScale(), go->GetScale(), 0, tr);
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
                m_spriteBatcher->drawSprite(go->GetLocation().mX, go->GetLocation().mY, go->GetScale(), go->GetScale(), RADIANS_TO_DEGREES(go->GetRotation()) - 90, *c, tr);
            }
            else if (go->GetClassId() == 'YARN')
            {
                TextureRegion tr = ASSETS->findTextureRegion("Pellet");
                m_spriteBatcher->drawSprite(go->GetLocation().mX, go->GetLocation().mY, go->GetScale(), go->GetScale(), 0, tr);
            }
        }
        m_spriteBatcher->endBatch(*m_demo->gpuTextureWrapper, *m_textureGpuProgramWrapper);
        
        m_spriteBatcher->beginBatch();
        RenderBandWidth();
        RenderRoundTripTime();
        RenderScoreBoard();
        RenderHealth();
        m_spriteBatcher->endBatch(*m_misc->gpuTextureWrapper, *m_textureGpuProgramWrapper);
    }
}

void MainRenderer::RenderHealth()
{
    if (HUD::sInstance->mHealth > 0)
    {
        string healthString = StringUtils::Sprintf("Health %d", HUD::sInstance->mHealth);
        RenderText(healthString, HUD::sInstance->mHealthOffset, Colors::Red);
    }
}

void MainRenderer::RenderBandWidth()
{
    string bandwidth = StringUtils::Sprintf("In %d  Bps, Out %d Bps",
                                            static_cast< int >(NetworkManagerClient::sInstance->GetBytesReceivedPerSecond().GetValue()),
                                            static_cast< int >(NetworkManagerClient::sInstance->GetBytesSentPerSecond().GetValue()));
    RenderText(bandwidth, HUD::sInstance->mBandwidthOrigin, Colors::White);
}

void MainRenderer::RenderRoundTripTime()
{
    float rttMS = NetworkManagerClient::sInstance->GetAvgRoundTripTime().GetValue() * 1000.f;
    
    string roundTripTime = StringUtils::Sprintf("RTT %d ms", (int) rttMS);
    RenderText(roundTripTime, HUD::sInstance->mRoundTripTimeOrigin, Colors::White);
}

void MainRenderer::RenderScoreBoard()
{
    const vector< ScoreBoardManager::Entry >& entries = ScoreBoardManager::sInstance->GetEntries();
    Vector3 offset = HUD::sInstance->mScoreBoardOrigin;
    
    for (const auto& entry: entries)
    {
        RenderText(entry.GetFormattedNameScore(), offset, entry.GetColor());
        offset.mX += HUD::sInstance->mScoreOffset.mX;
        offset.mY += HUD::sInstance->mScoreOffset.mY;
    }
}

void MainRenderer::RenderText(const string& inStr, const Vector3& origin, const Vector3& inColor)
{
    Color fontColor = Color(inColor.mX, inColor.mY, inColor.mZ, 1);
    float fgWidth = CAM_WIDTH / 60;
    float fgHeight = fgWidth * 1.171875f;
    
    std::stringstream ss;
    ss << inStr;
    std::string text = ss.str();
    
    m_font->renderText(*m_spriteBatcher, text, origin.mX, origin.mY, fgWidth, fgHeight, fontColor, false, false);
}
