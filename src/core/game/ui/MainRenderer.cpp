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

#include "macros.h"
#include "NetworkManagerClient.h"
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
    
    m_rendererHelper->clearFramebufferWithColor(0.5f, 0.5f, 0.5f, 1);
    
    if (ensureTexture(m_demo)
        && ensureTexture(m_misc))
    {
        m_spriteBatcher->beginBatch();
        for (GameObjectPtr go : World::sInstance->GetGameObjects())
        {
            if (go->GetClassId() == 'RCAT')
            {
                TextureRegion tr = ASSETS->findTextureRegion("CharacterHoldingGun", stateTime);
                m_spriteBatcher->drawSprite(go->GetLocation().getX(), go->GetLocation().getY(), go->GetScale(), go->GetScale(), RADIANS_TO_DEGREES(go->GetRotation()) - 90, go->GetColor(), tr);
            }
        }
        m_spriteBatcher->endBatch(*m_demo->gpuTextureWrapper, *m_textureGpuProgramWrapper);
        
        m_spriteBatcher->beginBatch();
        RenderBandWidth();
        RenderRoundTripTime();
        m_spriteBatcher->endBatch(*m_misc->gpuTextureWrapper, *m_textureGpuProgramWrapper);
    }
}

#include "WeightedTimedMovingAverage.h"

void MainRenderer::RenderBandWidth()
{
    static Vector2 bandwidthOrigin = Vector2(4.f, 6.6f);
    
    const WeightedTimedMovingAverage& bpsIn = NetworkManagerClient::sInstance->GetBytesReceivedPerSecond();
    int bpsInInt = static_cast< int >(bpsIn.GetValue());
    
    const WeightedTimedMovingAverage& bpsOut = NetworkManagerClient::sInstance->GetBytesSentPerSecond();
    int bpsOutInt = static_cast< int >(bpsOut.GetValue());
    
    std::string bandwidth = StringUtils::Sprintf("In %d Bps, Out %d Bps", bpsInInt, bpsOutInt);
    
    static Color whiteColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
    
    RenderText(bandwidth, bandwidthOrigin, whiteColor);
}

void MainRenderer::RenderRoundTripTime()
{
    static Vector2 roundTripTimeOrigin = Vector2(5.f, 7.f);
    
    float rttMS = NetworkManagerClient::sInstance->GetAvgRoundTripTime().GetValue() * 1000.f;
    
    std::string roundTripTime = StringUtils::Sprintf("RTT %d ms", (int) rttMS);
    
    static Color whiteColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
    
    RenderText(roundTripTime, roundTripTimeOrigin, whiteColor);
}

void MainRenderer::RenderText(const std::string& inStr, const Vector2& origin, const Color& inColor)
{
    Color fontColor = Color(inColor.red, inColor.green, inColor.blue, inColor.alpha);
    float fgWidth = CAM_WIDTH / 60;
    float fgHeight = fgWidth * 1.171875f;
    
    std::stringstream ss;
    ss << inStr;
    std::string text = ss.str();
    
    m_font->renderText(*m_spriteBatcher, text, origin.getX(), origin.getY(), fgWidth, fgHeight, fontColor, false, false);
}
