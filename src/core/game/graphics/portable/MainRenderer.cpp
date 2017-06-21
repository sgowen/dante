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
#include "InputManager.h"
#include "NetworkManagerServer.h"
#include "IMachineAddress.h"
#include "MainEngineState.h"
#include "NGSteamGameServer.h"
#include "NGSteamGameServices.h"

#include <sstream>
#include <ctime> // rand

MainRenderer::MainRenderer(int maxBatchSize) : Renderer(maxBatchSize),
m_characters(new TextureWrapper("texture_001")),
m_misc(new TextureWrapper("texture_002")),
m_bg1(new TextureWrapper("texture_003", true)),
m_bg2(new TextureWrapper("texture_004", true)),
m_cover(new TextureWrapper("texture_005", true)),
m_font(new Font("texture_002", 0, 0, 16, 64, 75, TEXTURE_SIZE_1024))
{
    ASSETS->init(new MainAssetsMapper());
}

MainRenderer::~MainRenderer()
{
	releaseDeviceDependentResources();

	delete m_characters;
	delete m_misc;
    delete m_bg1;
    delete m_bg2;
    delete m_cover;
    delete m_font;
}

void MainRenderer::createDeviceDependentResources()
{
    Renderer::createDeviceDependentResources();
    
    loadTextureSync(m_characters);
    loadTextureSync(m_misc);
    loadTextureSync(m_bg1);
    loadTextureSync(m_bg2);
    loadTextureSync(m_cover);
}

void MainRenderer::releaseDeviceDependentResources()
{
    Renderer::releaseDeviceDependentResources();

	unloadTexture(m_characters);
	unloadTexture(m_misc);
    unloadTexture(m_bg1);
    unloadTexture(m_bg2);
    unloadTexture(m_cover);
}

void MainRenderer::renderWorld(int engineState)
{
    m_rendererHelper->clearFramebufferWithColor(0.5f, 0.5f, 0.5f, 1);
    
    if (ensureTexture(m_characters)
        && ensureTexture(m_misc))
    {
        {
            m_rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
            
            m_spriteBatcher->beginBatch();
            {
                static TextureRegion tr = ASSETS->findTextureRegion("Background1");
                m_spriteBatcher->drawSprite(CAM_WIDTH / 2, CAM_HEIGHT / 2, CAM_WIDTH, CAM_HEIGHT, 0, tr);
            }
            m_spriteBatcher->endBatch(*m_bg1->gpuTextureWrapper, *m_textureGpuProgramWrapper);
            
            m_spriteBatcher->beginBatch();
            {
                static TextureRegion tr = ASSETS->findTextureRegion("Background2");
                m_spriteBatcher->drawSprite(CAM_WIDTH / 2, CAM_HEIGHT * 0.2421875f / 2, CAM_WIDTH, CAM_HEIGHT * 0.2421875f, 0, tr);
            }
            {
                static TextureRegion tr = ASSETS->findTextureRegion("Background3");
                m_spriteBatcher->drawSprite(CAM_WIDTH / 2, CAM_HEIGHT * 0.125f / 2, CAM_WIDTH, CAM_HEIGHT * 0.125f, 0, tr);
            }
            m_spriteBatcher->endBatch(*m_bg2->gpuTextureWrapper, *m_textureGpuProgramWrapper);
        }
        
        if (InstanceManager::getClientWorld())
        {
            m_rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
            
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
            m_spriteBatcher->endBatch(*m_characters->gpuTextureWrapper, *m_textureGpuProgramWrapper);
        }
        
        m_rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
        m_spriteBatcher->beginBatch();
        {
            static TextureRegion tr = ASSETS->findTextureRegion("Cover");
            m_spriteBatcher->drawSprite(CAM_WIDTH / 2, CAM_HEIGHT / 2, CAM_WIDTH, CAM_HEIGHT, 0, tr);
        }
        m_spriteBatcher->endBatch(*m_cover->gpuTextureWrapper, *m_textureGpuProgramWrapper);
        
        m_rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
        m_spriteBatcher->beginBatch();
        switch (engineState)
        {
            case MAIN_ENGINE_STATE_MAIN_MENU_STEAM_OFF:
                renderMainMenuSteamOffText();
                break;
            case MAIN_ENGINE_STATE_MAIN_MENU_STEAM_ON:
                renderMainMenuSteamOnText();
                break;
            case MAIN_ENGINE_STATE_MAIN_MENU_STARTING_SERVER:
                renderStartingServerText();
                break;
            case MAIN_ENGINE_STATE_MAIN_MENU_ENTERING_USERNAME:
                renderEnterUsernameText();
                break;
            case MAIN_ENGINE_STATE_MAIN_MENU_JOINING_LOCAL_SERVER_BY_IP:
                renderJoiningLocalServerByIPText();
                break;
            case MAIN_ENGINE_STEAM_JOINING_SERVER:
                renderJoiningServerText();
                break;
            default:
                break;
        }
        m_spriteBatcher->endBatch(*m_misc->gpuTextureWrapper, *m_textureGpuProgramWrapper);
    }
}

void MainRenderer::renderMainMenuSteamOffText()
{
    {
        static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 1);
        std::string text = std::string("'A' to activate Steam");
        renderText(text, origin, Color::BLACK);
    }
    
    {
        static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 2);
        std::string text = std::string("'S' to start local server");
        renderText(text, origin, Color::BLACK);
    }
    
    {
        static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 3);
        std::string text = std::string("'J' to join server by IP");
        renderText(text, origin, Color::BLACK);
    }
    
    {
        static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 8);
        std::string text = std::string("'ESC' to exit game");
        renderText(text, origin, Color::BLACK);
    }
}

void MainRenderer::renderMainMenuSteamOnText()
{
    {
        static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 1);
        std::string text = std::string("'D' to deactivate Steam");
        renderText(text, origin, Color::BLACK);
    }
    
    {
        static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 2);
        std::string text = std::string("'S' to start steam server");
        renderText(text, origin, Color::BLACK);
    }
    
    {
        static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 3);
        std::string text = std::string("'L' to refresh list of LAN servers");
        renderText(text, origin, Color::BLACK);
    }
    
    {
        static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 4);
        std::string text = std::string("'I' to refresh list of Internet servers");
        renderText(text, origin, Color::BLACK);
    }
    
    std::vector<NGSteamGameServer> gameServers = NG_STEAM_GAME_SERVICES->getGameServers();
    int index = 0;
    for (NGSteamGameServer gameServer : gameServers)
    {
        Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 5.5f - (index * 0.5f));
        std::string text = StringUtil::format("'%i' %s", (++index), gameServer.getDisplayString());
        renderText(text, origin, Color::BLACK);
    }
    
    {
        static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 8);
        std::string text = std::string("'ESC' to exit game");
        renderText(text, origin, Color::BLACK);
    }
}

void MainRenderer::renderStartingServerText()
{
    static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 2);
    std::string text = std::string("Server starting, 'ESC' to exit");
    renderText(text, origin, Color::BLACK);
}

void MainRenderer::renderEnterUsernameText()
{
    {
        static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 2);
        std::string text = std::string("Enter Username to join, 'ESC' to exit");
        renderText(text, origin, Color::BLACK);
    }
    
    {
        static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 4);
        std::string text = InputManager::getInstance()->getLiveInputRef();
        renderText(text, origin, Color::BLACK);
    }
}

void MainRenderer::renderJoiningLocalServerByIPText()
{
    {
        static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 2);
        std::string text = std::string("Enter Server Address to join, 'ESC' to exit");
        renderText(text, origin, Color::BLACK);
    }
    
    {
        static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 4);
        std::string text = InputManager::getInstance()->getLiveInputRef();
        renderText(text, origin, Color::BLACK);
    }
}

void MainRenderer::renderJoiningServerText()
{
    {
        static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 2);
        std::string text = StringUtil::format("%s, 'ESC' to exit", NG_CLIENT->getState() == NCS_Welcomed ? "Server joined" : "Joining Server...");
        renderText(text, origin, Color::BLACK);
    }
    
    if (NG_CLIENT->getState() != NCS_Welcomed)
    {
        return;
    }
    
    {
        static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 0.5);
        float rttMS = NG_CLIENT->getAvgRoundTripTime().getValue() * 1000.f;
        std::string text = StringUtil::format("RTT %d ms", (int) rttMS);
        renderText(text, origin, Color::BLACK);
    }
    
    {
        static Vector2 origin = Vector2(CAM_WIDTH / 2, CAM_HEIGHT - 1);
        
        const WeightedTimedMovingAverage& bpsIn = NG_CLIENT->getBytesReceivedPerSecond();
        int bpsInInt = static_cast< int >(bpsIn.getValue());
        
        const WeightedTimedMovingAverage& bpsOut = NG_CLIENT->getBytesSentPerSecond();
        int bpsOutInt = static_cast< int >(bpsOut.getValue());
        
        std::string text = StringUtil::format("In %d Bps, Out %d Bps", bpsInInt, bpsOutInt);
        renderText(text, origin, Color::BLACK);
    }
    
    if (InstanceManager::getClientWorld())
    {
        std::vector<Entity*> entities = InstanceManager::getClientWorld()->getEntities();
        for (Entity* go : entities)
        {
            if (go->getNetworkType() == NETWORK_TYPE_Robot)
            {
                Robot* robot = static_cast<Robot*>(go);
                Vector2 origin = Vector2(0.5f, CAM_HEIGHT - (robot->getPlayerId() * 0.5f));
                std::string text = StringUtil::format("%i|%s", robot->getPlayerId(), robot->getPlayerName().c_str());
                renderText(text, origin, Color::BLACK, true);
            }
        }
    }
}

void MainRenderer::renderText(const std::string& inStr, const Vector2& origin, const Color& inColor, bool isLeftJustified)
{
    Color fontColor = Color(inColor.red, inColor.green, inColor.blue, inColor.alpha);
    float fgWidth = CAM_WIDTH / 60;
    float fgHeight = fgWidth * 1.171875f;
    
    std::stringstream ss;
    ss << inStr;
    std::string text = ss.str();
    
    m_font->renderText(*m_spriteBatcher, text, origin.getX(), origin.getY(), fgWidth, fgHeight, fontColor, !isLeftJustified, false);
}
