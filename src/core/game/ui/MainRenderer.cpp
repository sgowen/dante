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
#include "Robot.h"
#include "Projectile.h"
#include "StringUtil.h"
#include "WeightedTimedMovingAverage.h"

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

	delete m_demo;
	delete m_misc;
    delete m_font;
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

void MainRenderer::tempDraw()
{
    m_rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
    
    m_rendererHelper->clearFramebufferWithColor(0.5f, 0.5f, 0.5f, 1);
    
    if (ensureTexture(m_demo)
        && ensureTexture(m_misc))
    {
        {
            m_spriteBatcher->beginBatch();
            static TextureRegion tr = ASSETS->findTextureRegion("Mario_Backdrop");
            m_spriteBatcher->drawSprite(CAM_WIDTH / 2, CAM_HEIGHT / 2, CAM_WIDTH, CAM_HEIGHT, 0, tr);
            m_spriteBatcher->endBatch(*m_demo->gpuTextureWrapper, *m_textureGpuProgramWrapper);
        }
        
        m_spriteBatcher->beginBatch();
        for (Entity* go : World::getInstance()->getEntities())
        {
            if (go->getNetworkType() == NETWORK_TYPE_Robot)
            {
                Robot* robot = static_cast<Robot*>(go);
                bool isMoving = go->getVelocity().getX() < -0.5f || go->getVelocity().getX() > 0.5f;
                TextureRegion tr = ASSETS->findTextureRegion(
                                                             robot->isGrounded() ?
                                                             isMoving ? robot->isShooting() ? "Samus_Shooting" : "Samus_Running" : "Samus_Idle" :
                                                             go->getVelocity().getY() > 0 ? "Samus_Jumping" : "Samus_Falling", go->getStateTime());
                renderEntityWithColor(*go, tr, go->getColor(), robot->isFacingLeft());
            }
            else if (go->getNetworkType() == NETWORK_TYPE_Projectile)
            {
                Projectile* proj = static_cast<Projectile*>(go);
                TextureRegion tr = ASSETS->findTextureRegion("Projectile");
                renderEntityWithColor(*go, tr, go->getColor(), proj->isFacingLeft());
            }
        }
        m_spriteBatcher->endBatch(*m_demo->gpuTextureWrapper, *m_textureGpuProgramWrapper);
        
        m_spriteBatcher->beginBatch();
        RenderBandWidth();
        RenderRoundTripTime();
        m_spriteBatcher->endBatch(*m_misc->gpuTextureWrapper, *m_textureGpuProgramWrapper);
    }
}

void MainRenderer::RenderBandWidth()
{
    static Vector2 bandwidthOrigin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 1);
    
    const WeightedTimedMovingAverage& bpsIn = NetworkManagerClient::getInstance()->getBytesReceivedPerSecond();
    int bpsInInt = static_cast< int >(bpsIn.getValue());
    
    const WeightedTimedMovingAverage& bpsOut = NetworkManagerClient::getInstance()->getBytesSentPerSecond();
    int bpsOutInt = static_cast< int >(bpsOut.getValue());
    
    std::string bandwidth = StringUtil::sprintf("In %d Bps, Out %d Bps", bpsInInt, bpsOutInt);
    
    static Color whiteColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
    
    RenderText(bandwidth, bandwidthOrigin, whiteColor);
}

void MainRenderer::RenderRoundTripTime()
{
    static Vector2 roundTripTimeOrigin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 0.5);
    
    float rttMS = NetworkManagerClient::getInstance()->getAvgRoundTripTime().getValue() * 1000.f;
    
    std::string roundTripTime = StringUtil::sprintf("RTT %d ms", (int) rttMS);
    
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
    
    m_font->renderText(*m_spriteBatcher, text, origin.getX(), origin.getY(), fgWidth, fgHeight, fontColor, true, false);
}
