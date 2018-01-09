//
//  GameRenderer.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/graphics/portable/GameRenderer.h"

#include "framework/graphics/portable/TextureManager.h"
#include "framework/graphics/portable/Font.h"
#include "game/logic/World.h"
#include "framework/graphics/portable/Box2DDebugRenderer.h"
#include "framework/graphics/portable/SpriteBatcher.h"
#include "framework/graphics/portable/QuadBatcher.h"
#include "framework/graphics/portable/LineBatcher.h"
#include "framework/graphics/portable/CircleBatcher.h"
#include "framework/graphics/portable/NGShaderLoader.h"
#include "framework/graphics/portable/RendererHelper.h"
#include "framework/graphics/portable/NGShader.h"
#include "framework/entity/Entity.h"
#include "framework/graphics/portable/TextureRegion.h"
#include "framework/graphics/portable/Color.h"
#include "framework/math/NGRect.h"
#include "framework/graphics/portable/NGTexture.h"
#include "framework/graphics/portable/Box2DDebugRenderer.h"

#include "framework/graphics/portable/Assets.h"
#include "framework/graphics/portable/RendererHelper.h"
#include "game/logic/GameConstants.h"
#include "framework/math/NGRect.h"
#include "framework/math/Line.h"
#include "framework/graphics/portable/Color.h"
#include "framework/graphics/portable/SpriteBatcher.h"
#include "framework/graphics/portable/TextureRegion.h"
#include "framework/util/macros.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "framework/util/StringUtil.h"
#include "framework/util/WeightedTimedMovingAverage.h"
#include "framework/util/NGSTDUtil.h"
#include "game/logic/InstanceManager.h"
#include "game/logic/GameInputManager.h"
#include "framework/network/server/NetworkManagerServer.h"
#include "framework/network/portable/MachineAddress.h"
#include "framework/math/MathUtil.h"
#include "framework/audio/portable/NGAudioEngine.h"
#include "game/logic/Server.h"
#include "game/logic/TitleEngine.h"
#include "framework/util/FPSUtil.h"
#include "framework/graphics/portable/Color.h"
#include "framework/math/Circle.h"
#include "framework/graphics/portable/CircleBatcher.h"
#include "framework/graphics/portable/QuadBatcher.h"
#include "framework/graphics/portable/LineBatcher.h"
#include "framework/util/FlagUtil.h"
#include "framework/util/FrameworkConstants.h"
#include "framework/graphics/portable/TextureWrapper.h"
#include "framework/graphics/portable/TextureDataWrapper.h"
#include "framework/graphics/portable/TextureLoaderFactory.h"
#include "framework/graphics/portable/NGShaderLoaderFactory.h"
#include "framework/graphics/portable/RendererHelperFactory.h"
#include "framework/util/NGSTDUtil.h"
#include "framework/math/Circle.h"
#include "framework/math/Line.h"
#include <framework/graphics/portable/NGTextureShader.h>
#include <framework/graphics/portable/NGGeometryShader.h>
#include <framework/graphics/portable/NGFramebufferToScreenShader.h>
#include "framework/graphics/portable/NGTextureDesc.h"
#include "framework/graphics/portable/Assets.h"
#include <game/logic/GameEngine.h>
#include <game/logic/RobotController.h>

#ifdef NG_STEAM
#include "framework/network/steam/NGSteamGameServer.h"
#include "framework/network/steam/NGSteamGameServices.h"
#endif

#include <sstream>
#include <ctime> // rand
#include <string>
#include <assert.h>

GameRenderer::GameRenderer() : Renderer(),
_textureManager(new TextureManager("game_assets.cfg")),
_rendererHelper(RENDERER_HELPER_FACTORY->createRendererHelper()),
_spriteBatcher1(new SpriteBatcher(_rendererHelper)),
_spriteBatcher2(new SpriteBatcher(_rendererHelper)),
_spriteBatcher3(new SpriteBatcher(_rendererHelper)),
_fillQuadBatcher(new QuadBatcher(_rendererHelper, true)),
_boundsQuadBatcher(new QuadBatcher(_rendererHelper, false)),
_lineBatcher(new LineBatcher(_rendererHelper)),
_circleBatcher(new CircleBatcher(_rendererHelper)),
_box2DDebugRenderer(new Box2DDebugRenderer(*_fillQuadBatcher, *_boundsQuadBatcher, *_lineBatcher, *_circleBatcher)),
_shaderProgramLoader(SHADER_PROGRAM_LOADER_FACTORY->createNGShaderLoader()),
_textureNGShader(new NGTextureShader(*_rendererHelper, "shader_003_vert.ngs", "shader_003_frag.ngs")),
_colorNGShader(new NGGeometryShader(*_rendererHelper, "shader_001_vert.ngs", "shader_001_frag.ngs")),
_framebufferToScreenNGShader(new NGFramebufferToScreenShader(*_rendererHelper, "shader_002_vert.ngs", "shader_002_frag.ngs")),
_font(new Font("texture_001.ngt", 0, 0, 16, 64, 75, TEXTURE_SIZE_1024)),
_camBounds(new NGRect(0, 0, CAM_WIDTH, CAM_HEIGHT)),
_framebufferIndex(0)
{
    // Empty
}

GameRenderer::~GameRenderer()
{
    releaseDeviceDependentResources();
    
    delete _textureManager;
    delete _rendererHelper;
    delete _spriteBatcher1;
    delete _spriteBatcher2;
    delete _spriteBatcher3;
    delete _fillQuadBatcher;
    delete _boundsQuadBatcher;
    delete _lineBatcher;
    delete _circleBatcher;
    delete _box2DDebugRenderer;
    delete _shaderProgramLoader;
    delete _font;
    delete _textureNGShader;
    delete _colorNGShader;
    delete _framebufferToScreenNGShader;
}

void GameRenderer::createDeviceDependentResources()
{
    _rendererHelper->createDeviceDependentResources();
    _textureManager->createDeviceDependentResources();
    
    _textureNGShader->load(*_shaderProgramLoader);
    _colorNGShader->load(*_shaderProgramLoader);
    _framebufferToScreenNGShader->load(*_shaderProgramLoader);
}

void GameRenderer::createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight)
{
    _rendererHelper->createWindowSizeDependentResources(screenWidth, screenHeight, renderWidth, renderHeight);
}

void GameRenderer::releaseDeviceDependentResources()
{
    _rendererHelper->releaseDeviceDependentResources();
    _textureManager->releaseDeviceDependentResources();
    
    _textureNGShader->unload(*_shaderProgramLoader);
    _colorNGShader->unload(*_shaderProgramLoader);
    _framebufferToScreenNGShader->unload(*_shaderProgramLoader);
}

void GameRenderer::render(int flags)
{
    beginFrame();
    
    if (_textureManager->ensureTextures())
    {
        updateCamera();
        
        renderBackground();
        
        renderWorld(flags);
        
        renderUI(flags);
    }
    
    endFrame();
}

void GameRenderer::beginFrame()
{
    _textureManager->handleAsyncTextureLoads();
    
    setFramebuffer(0);
}

void GameRenderer::setFramebuffer(int framebufferIndex)
{
    assert(framebufferIndex >= 0);
    
    _framebufferIndex = framebufferIndex;
    
    _rendererHelper->bindToOffscreenFramebuffer(_framebufferIndex);
    _rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
}

void GameRenderer::updateCamera()
{
    _camBounds->getLowerLeft().set(0, 0);
    _camBounds->setWidth(CAM_WIDTH);
    _camBounds->setHeight(CAM_HEIGHT);
    
    if (NG_CLIENT && NG_CLIENT->getState() == NCS_Welcomed)
    {
        float left = 0;
        float right = 0;
        float bottom = 0;
        float top = 0;
        
        bool needsInit = true;
        
        for (Entity* entity : InstanceManager::getClientWorld()->getPlayers())
        {
            RobotController* robot = static_cast<RobotController*>(entity->getEntityController());
            
            if (NG_CLIENT->isPlayerIdLocal(robot->getPlayerId()))
            {
                // Adjust camera based on the player position
                
                float pX = entity->getPosition().x;
                float pY = entity->getPosition().y;
                
                if (needsInit)
                {
                    left = pX;
                    right = pX;
                    bottom = pY;
                    top = pY;
                    
                    needsInit = false;
                }
                else
                {
                    left = MIN(pX, left);
                    right = MAX(pX, right);
                    
                    bottom = MIN(pY, bottom);
                    top = MAX(pY, top);
                }
            }
        }
        
        if (!needsInit)
        {
            left -= CAM_WIDTH * 0.33f;
            right += CAM_WIDTH * 0.33f;
            bottom -= CAM_HEIGHT * 0.33f;
            top += CAM_HEIGHT * 0.33f;
            
            float pX = (left + right) / 2.0f;
            float pY = (bottom + top) / 2.0f;
            float w = right - left;
            float h = top - bottom;
            
            if (w < CAM_WIDTH)
            {
                w = CAM_WIDTH;
            }
            
            if (h < CAM_HEIGHT)
            {
                h = CAM_HEIGHT;
            }
            
            if (w > CAM_WIDTH)
            {
                h = w * (CAM_HEIGHT / CAM_WIDTH);
            }
            else if (h > CAM_HEIGHT)
            {
                w = h * (CAM_WIDTH / CAM_HEIGHT);
            }
            
            float x = pX - w * 0.5f;
            
            float y = pY - h * 0.5f;
            y = clamp(y, GAME_HEIGHT, 0);
            
            _camBounds->getLowerLeft().set(x, y);
            _camBounds->setWidth(w);
            _camBounds->setHeight(h);
        }
    }
}

void GameRenderer::renderBackground()
{
    // TODO
}

void GameRenderer::renderWorld(int flags)
{
    _rendererHelper->updateMatrix(_camBounds->getLeft(), _camBounds->getRight(), _camBounds->getBottom(), _camBounds->getTop());
    
    renderEntities(InstanceManager::getClientWorld(), false);
    
    if (Server::getInstance() && Server::getInstance()->isDisplaying())
    {
        renderEntities(InstanceManager::getServerWorld(), true);
    }
    
    if (isFlagSet(flags, GE_DISPLAY_BOX_2D))
    {
        _box2DDebugRenderer->setWorld(&InstanceManager::getClientWorld()->getWorld());
        _box2DDebugRenderer->render(*_colorNGShader);
        
        if (Server::getInstance() && Server::getInstance()->isDisplaying())
        {
            _box2DDebugRenderer->setWorld(&InstanceManager::getServerWorld()->getWorld());
            _box2DDebugRenderer->render(*_colorNGShader);
        }
    }
}

void GameRenderer::renderEntities(World* world, bool isServer)
{
    _spriteBatcher1->beginBatch();
    _spriteBatcher2->beginBatch();
    _spriteBatcher3->beginBatch();
    std::vector<Entity*> entities = world->getEntities();
    for (Entity* e : entities)
    {
        TextureRegion tr = ASSETS->findTextureRegion(e->getMapping(), e->getStateTime());
        Color c = Color::WHITE;
        
        EntityController* ec = e->getEntityController();
        if (ec->getRTTI().derivesFrom(RobotController::rtti))
        {
            RobotController* r = static_cast<RobotController*>(ec);
            
            uint8_t pId = r->getPlayerId();
            c = pId == 1 ? Color::WHITE : pId == 2 ? Color::RED : pId == 3 ? Color::GREEN : Color::BLUE;
            
            if (r->isSprinting())
            {
                c.alpha = (rand() % 50) * 0.01f + 0.25f;
            }
            
            if (isServer)
            {
                c.alpha /= 2.0f;
            }
            
            _spriteBatcher1->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), c, tr, e->isFacingLeft());
        }
        else
        {
            if (isServer)
            {
                c.alpha /= 2.0f;
            }
            
            _spriteBatcher3->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), c, tr, e->isFacingLeft());
        }
    }
    _spriteBatcher1->endBatch(_textureManager->getTextures()[1], *_textureNGShader);
    _spriteBatcher2->endBatch(_textureManager->getTextures()[2], *_textureNGShader);
    _spriteBatcher3->endBatch(_textureManager->getTextures()[3], *_textureNGShader);
}

void GameRenderer::renderUI(int flags)
{
    _rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
    
    _spriteBatcher1->beginBatch();
    
    if (NG_CLIENT->getState() == NCS_Welcomed)
    {
        int row = 1;
        static float padding = 0.5f;
        
        int fps = FPSUtil::getInstance()->getFPS();
        renderText(StringUtil::format("FPS %d", fps).c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::WHITE, FONT_ALIGN_RIGHT);
        
        float rttMS = NG_CLIENT->getAvgRoundTripTime().getValue() * 1000.f;
        renderText(StringUtil::format("RTT %d ms", static_cast<int>(rttMS)).c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::WHITE, FONT_ALIGN_RIGHT);
        
        const WeightedTimedMovingAverage& bpsIn = NG_CLIENT->getBytesReceivedPerSecond();
        int bpsInInt = static_cast<int>(bpsIn.getValue());
        renderText(StringUtil::format(" In %d Bps", bpsInInt).c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::WHITE, FONT_ALIGN_RIGHT);
        
        const WeightedTimedMovingAverage& bpsOut = NG_CLIENT->getBytesSentPerSecond();
        int bpsOutInt = static_cast<int>(bpsOut.getValue());
        renderText(StringUtil::format("Out %d Bps", bpsOutInt).c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::WHITE, FONT_ALIGN_RIGHT);
        
        // Controls
        ++row;
        
        renderText(StringUtil::format("'S'       Sound %s", NG_AUDIO_ENGINE->isSoundDisabled() ? "OFF" : " ON").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::WHITE, FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("'M'       Music %s", NG_AUDIO_ENGINE->isMusicDisabled() ? "OFF" : " ON").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::WHITE, FONT_ALIGN_RIGHT);
        
        if (Server::getInstance())
        {
            renderText(StringUtil::format("'O'     Objects %s", Server::getInstance()->isSpawningObjects() ? " ON" : "OFF").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::WHITE, FONT_ALIGN_RIGHT);
            renderText(StringUtil::format("'I'       DEBUG %s", Server::getInstance()->isDisplaying() ? " ON" : "OFF").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::WHITE, FONT_ALIGN_RIGHT);
            renderText(StringUtil::format("'P' BOX2D DEBUG %s", isFlagSet(flags, GE_DISPLAY_BOX_2D) ? " ON" : "OFF").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::WHITE, FONT_ALIGN_RIGHT);
        }
        
        if (InstanceManager::getClientWorld())
        {
            bool activePlayerIds[4] = {false};
            
            std::vector<Entity*> players = InstanceManager::getClientWorld()->getPlayers();
            for (Entity* entity : players)
            {
                RobotController* robot = static_cast<RobotController*>(entity->getEntityController());
                
                int playerId = robot->getPlayerId();
                if (playerId >= 1 && playerId <= 4)
                {
                    renderText(StringUtil::format("%i|%s - %i HP", playerId, robot->getPlayerName().c_str(), robot->getHealth()).c_str(), 0.5f, CAM_HEIGHT - (playerId * 0.5f), Color::WHITE);
                    activePlayerIds[playerId - 1] = true;
                }
            }
            
            for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
            {
                if (!activePlayerIds[i])
                {
                    renderText(StringUtil::format("%i|%s", (i + 1), "Connect a controller to join...").c_str(), 0.5f, CAM_HEIGHT - ((i + 1) * 0.5f), Color::WHITE);
                }
            }
        }
    }
    else
    {
        renderText(StringUtil::format("%s, 'ESC' to exit", "Joining Server...").c_str(), 0.5f, CAM_HEIGHT - 3.5f, Color::WHITE, FONT_ALIGN_LEFT);
    }
    
    _spriteBatcher1->endBatch(_textureManager->getTextures()[0], *_textureNGShader);
}

void GameRenderer::renderText(const char* inStr, float x, float y, const Color& inColor, int justification)
{
    Color fontColor = Color(inColor.red, inColor.green, inColor.blue, inColor.alpha);
    float fgWidth = CAM_WIDTH / 60;
    float fgHeight = fgWidth * 1.171875f;
    
    std::string text(inStr);
    
    _font->renderText(*_spriteBatcher1, text, x, y, fgWidth, fgHeight, fontColor, justification);
}

void GameRenderer::endFrame()
{
    assert(_framebufferIndex >= 0);
    
    _rendererHelper->bindToScreenFramebuffer();
    _rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
    
    static std::vector<SCREEN_VERTEX> screenVertices;
    screenVertices.clear();
    screenVertices.push_back(SCREEN_VERTEX(-1, -1));
    screenVertices.push_back(SCREEN_VERTEX(-1, 1));
    screenVertices.push_back(SCREEN_VERTEX(1, 1));
    screenVertices.push_back(SCREEN_VERTEX(1, -1));
    
    _framebufferToScreenNGShader->bind(&screenVertices, _rendererHelper->getFramebuffer(_framebufferIndex));
    _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
    _framebufferToScreenNGShader->unbind();
}
