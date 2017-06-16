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
#include "SpacePirate.h"
#include "StringUtil.h"
#include "WeightedTimedMovingAverage.h"
#include "NGSTDUtil.h"
#include "InstanceManager.h"

#include <sstream>
#include <ctime> // rand

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

void MainRenderer::tempDraw(int engineState)
{
    m_rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
    
    m_rendererHelper->clearFramebufferWithColor(0.5f, 0.5f, 0.5f, 1);
    
    if (ensureTexture(m_demo)
        && ensureTexture(m_misc))
    {
        {
            m_spriteBatcher->beginBatch();
            static TextureRegion tr = ASSETS->findTextureRegion("Background_Merged");
            m_spriteBatcher->drawSprite(CAM_WIDTH / 2, CAM_HEIGHT / 2, CAM_WIDTH, CAM_HEIGHT, 0, tr);
            m_spriteBatcher->endBatch(*m_demo->gpuTextureWrapper, *m_textureGpuProgramWrapper);
        }
        
        m_spriteBatcher->beginBatch();
        std::vector<Entity*> entities = InstanceManager::getClientWorld()->getEntities();
        for (Entity* go : entities)
        {
            if (go->getNetworkType() == NETWORK_TYPE_Robot)
            {
                Robot* robot = static_cast<Robot*>(go);
                
                Color c = robot->getColor();
                if (robot->isSprinting())
                {
                    c = Color();
                    c.red = (rand() % 100) * 0.01f;
                    c.green = (rand() % 100) * 0.01f;
                    c.blue = (rand() % 100) * 0.01f;
                    c.alpha = 0.75f;
                }
                
                bool isMoving = go->getVelocity().getX() < -0.5f || go->getVelocity().getX() > 0.5f;
                TextureRegion tr = ASSETS->findTextureRegion(
                                                             robot->isGrounded() ?
                                                             isMoving ? robot->isShooting() ? "Samus_Shooting" : (robot->isSprinting() ? "Samus_Running_Fast" : "Samus_Running") : "Samus_Idle" :
                                                             go->getVelocity().getY() > 0 ? "Samus_Jumping" : "Samus_Falling", go->getStateTime());
                renderEntityWithColor(*robot, tr, c, robot->isFacingLeft());
            }
            else if (go->getNetworkType() == NETWORK_TYPE_Projectile)
            {
                Projectile* proj = static_cast<Projectile*>(go);
                bool isActive = proj->getState() == Projectile::ProjectileState::ProjectileState_Active;
                TextureRegion tr = isActive ? ASSETS->findTextureRegion("Projectile") : ASSETS->findTextureRegion("Explosion", proj->getStateTime());
                renderEntityWithColor(*proj, tr, proj->getColor(), proj->isFacingLeft());
            }
            else if (go->getNetworkType() == NETWORK_TYPE_SpacePirate)
            {
                SpacePirate* sp = static_cast<SpacePirate*>(go);
                TextureRegion tr = ASSETS->findTextureRegion("Space_Pirate_Walking", sp->getStateTime());
                renderEntityWithColor(*sp, tr, sp->getColor(), sp->isFacingLeft());
            }
        }
        m_spriteBatcher->endBatch(*m_demo->gpuTextureWrapper, *m_textureGpuProgramWrapper);
        
        m_spriteBatcher->beginBatch();
        if (engineState == 5)
        {
            renderRoundTripTime();
            renderBandWidth();
            renderServerJoinedInstructions();
        }
        else if (engineState == 1)
        {
            renderServerStartedInstructions();
        }
        else
        {
            renderInstructions();
        }
        m_spriteBatcher->endBatch(*m_misc->gpuTextureWrapper, *m_textureGpuProgramWrapper);
    }
}

void MainRenderer::renderServerJoinedInstructions()
{
    static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 2);
    
    std::string roundTripTime = std::string("Server joined, 'ESC' to exit");
    
    static Color c = Color(0.0f, 0.0f, 0.0f, 1.0f);
    
    renderText(roundTripTime, origin, c);
}

void MainRenderer::renderServerStartedInstructions()
{
    static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 2);
    
    std::string roundTripTime = std::string("Server started, 'J' to join it");
    
    static Color c = Color(0.0f, 0.0f, 0.0f, 1.0f);
    
    renderText(roundTripTime, origin, c);
}

void MainRenderer::renderInstructions()
{
    static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 2);
    
    std::string roundTripTime = std::string("'S' to start server, 'J' to join first available");
    
    static Color c = Color(0.0f, 0.0f, 0.0f, 1.0f);
    
    renderText(roundTripTime, origin, c);
}

void MainRenderer::renderRoundTripTime()
{
    static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 0.5);
    
    float rttMS = NG_CLIENT->getAvgRoundTripTime().getValue() * 1000.f;
    
    std::string roundTripTime = StringUtil::format("RTT %d ms", (int) rttMS);
    
    static Color c = Color(0.0f, 0.0f, 0.0f, 1.0f);
    
    renderText(roundTripTime, origin, c);
}

void MainRenderer::renderBandWidth()
{
    static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 1);
    
    const WeightedTimedMovingAverage& bpsIn = NG_CLIENT->getBytesReceivedPerSecond();
    int bpsInInt = static_cast< int >(bpsIn.getValue());
    
    const WeightedTimedMovingAverage& bpsOut = NG_CLIENT->getBytesSentPerSecond();
    int bpsOutInt = static_cast< int >(bpsOut.getValue());
    
    std::string bandwidth = StringUtil::format("In %d Bps, Out %d Bps", bpsInInt, bpsOutInt);
    
    static Color c = Color(0.0f, 0.0f, 0.0f, 1.0f);
    
    renderText(bandwidth, origin, c);
}

void MainRenderer::renderText(const std::string& inStr, const Vector2& origin, const Color& inColor)
{
    Color fontColor = Color(inColor.red, inColor.green, inColor.blue, inColor.alpha);
    float fgWidth = CAM_WIDTH / 60;
    float fgHeight = fgWidth * 1.171875f;
    
    std::stringstream ss;
    ss << inStr;
    std::string text = ss.str();
    
    m_font->renderText(*m_spriteBatcher, text, origin.getX(), origin.getY(), fgWidth, fgHeight, fontColor, true, false);
}
