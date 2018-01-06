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
#include "game/logic/Robot.h"
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
#include "game/logic/Crate.h"
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
#include <game/logic/Ground.h>

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
_spriteBatcher(new SpriteBatcher(_rendererHelper)),
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
    delete _spriteBatcher;
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
            Robot* player = static_cast<Robot*>(entity);
            
            if (NG_CLIENT->isPlayerIdLocal(player->getPlayerId()))
            {
                // Adjust camera based on the player position
                
                float pX = player->getPosition().x;
                float pY = player->getPosition().y;
                
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
    for (int i = 0; i < 3; ++i)
    {
        _rendererHelper->updateMatrix(0, GAME_WIDTH, 0, CAM_HEIGHT);

        _spriteBatcher->beginBatch();
        {
            static TextureRegion tr = ASSETS->findTextureRegion("Background1");
            tr.initX(_camBounds->getLeft() * 128.0f / 3);
            tr.initY(384 - _camBounds->getBottom() * 32);
            _spriteBatcher->renderSprite(i * CAM_WIDTH + CAM_WIDTH / 2, CAM_HEIGHT / 2, CAM_WIDTH, CAM_HEIGHT, 0, tr);
        }
        _spriteBatcher->endBatch(_textureManager->getTextures()[2], *_textureNGShader);

        _spriteBatcher->beginBatch();
        {
            static TextureRegion tr = ASSETS->findTextureRegion("Background2");
            tr.initX(_camBounds->getLeft() * 128.0f / 2);
            tr.initY(644 - _camBounds->getBottom() * 48);
            _spriteBatcher->renderSprite(i * CAM_WIDTH + CAM_WIDTH / 2, CAM_HEIGHT * 0.3875f / 2, CAM_WIDTH, CAM_HEIGHT * 0.3875f, 0, tr);
        }
        _spriteBatcher->endBatch(_textureManager->getTextures()[3], *_textureNGShader);
    }
}

void GameRenderer::renderWorld(int flags)
{
    _rendererHelper->updateMatrix(_camBounds->getLeft(), _camBounds->getRight(), _camBounds->getBottom(), _camBounds->getTop());
    
    renderEntities(InstanceManager::getClientWorld(), false);
    
    if (Server::getInstance() && Server::getInstance()->isDisplaying())
    {
        renderEntities(InstanceManager::getServerWorld(), true);
    }
    
    if (FlagUtil::isFlagSet(flags, GE_DISPLAY_BOX_2D))
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
    _spriteBatcher->beginBatch();
    std::vector<Entity*> entities = world->getEntities();
    for (Entity* go : entities)
    {
        Color c = Color(go->getColor().red, go->getColor().green, go->getColor().blue, go->getColor().alpha);
        if (isServer)
        {
            c.alpha = 0.5f;
        }
        
        if (go->getRTTI().derivesFrom(Ground::rtti))
        {
            TextureRegion tr = ASSETS->findTextureRegion("Map_Ground", go->getStateTime());
            
            _spriteBatcher->renderSprite(go->getPosition().x, go->getPosition().y, go->getWidth(), go->getHeight(), go->getAngle(), c, tr);
        }
        else if (go->getRTTI().derivesFrom(Crate::rtti))
        {
            TextureRegion tr = ASSETS->findTextureRegion("Map_Crate", go->getStateTime());
            
            _spriteBatcher->renderSprite(go->getPosition().x, go->getPosition().y, go->getWidth(), go->getHeight(), go->getAngle(), c, tr);
        }
    }
    
    std::vector<Entity*> players = world->getPlayers();
    for (Entity* entity : players)
    {
        Robot* r = static_cast<Robot*>(entity);
        
        Color c = r->getColor();
        if (r->isSprinting())
        {
            c = Color(c.red, c.green, c.blue, c.alpha);
            c.alpha = (rand() % 50) * 0.01f + 0.25f;
        }
        
        bool isMoving = r->getVelocity().x < -0.5f || r->getVelocity().x > 0.5f;
        TextureRegion tr = ASSETS->findTextureRegion(r->isGrounded() && r->getNumJumps() == 0 ? r->isMainAction() ? "Bot_Punching" : !isMoving ? "Bot_Idle" : r->isSprinting() ? "Bot_Running_Fast" : "Bot_Running" : "Bot_Jumping", r->getStateTime());
        
        if (isServer)
        {
            c.alpha /= 2.0f;
        }
        
        _spriteBatcher->renderSprite(r->getPosition().x, r->getPosition().y, r->getWidth(), r->getHeight(), r->getAngle(), c, tr, r->isFacingLeft());
    }
    
    _spriteBatcher->endBatch(_textureManager->getTextures()[1], *_textureNGShader);
}

void GameRenderer::renderUI(int flags)
{
    _rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
    
    _spriteBatcher->beginBatch();
    
    if (NG_CLIENT->getState() == NCS_Welcomed)
    {
        int row = 1;
        static float padding = 0.5f;
        
        int fps = FPSUtil::getInstance()->getFPS();
        renderText(StringUtil::format("FPS %d", fps).c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::BLACK, FONT_ALIGN_RIGHT);
        
        float rttMS = NG_CLIENT->getAvgRoundTripTime().getValue() * 1000.f;
        renderText(StringUtil::format("RTT %d ms", static_cast<int>(rttMS)).c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::BLACK, FONT_ALIGN_RIGHT);
        
        const WeightedTimedMovingAverage& bpsIn = NG_CLIENT->getBytesReceivedPerSecond();
        int bpsInInt = static_cast<int>(bpsIn.getValue());
        renderText(StringUtil::format(" In %d Bps", bpsInInt).c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::BLACK, FONT_ALIGN_RIGHT);
        
        const WeightedTimedMovingAverage& bpsOut = NG_CLIENT->getBytesSentPerSecond();
        int bpsOutInt = static_cast<int>(bpsOut.getValue());
        renderText(StringUtil::format("Out %d Bps", bpsOutInt).c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::BLACK, FONT_ALIGN_RIGHT);
        
        // Controls
        ++row;
        
        renderText(StringUtil::format("'S'       Sound %s", NG_AUDIO_ENGINE->isSoundDisabled() ? "OFF" : " ON").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::BLACK, FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("'M'       Music %s", NG_AUDIO_ENGINE->isMusicDisabled() ? "OFF" : " ON").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::BLACK, FONT_ALIGN_RIGHT);
        
        if (Server::getInstance())
        {
            renderText(StringUtil::format("'E'     Enemies %s", Server::getInstance()->isSpawningEnemies() ? " ON" : "OFF").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::BLACK, FONT_ALIGN_RIGHT);
            renderText(StringUtil::format("'O'     Objects %s", Server::getInstance()->isSpawningObjects() ? " ON" : "OFF").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::BLACK, FONT_ALIGN_RIGHT);
            renderText(StringUtil::format("'I'       DEBUG %s", Server::getInstance()->isDisplaying() ? " ON" : "OFF").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::BLACK, FONT_ALIGN_RIGHT);
            renderText(StringUtil::format("'P' BOX2D DEBUG %s", FlagUtil::isFlagSet(flags, GE_DISPLAY_BOX_2D) ? " ON" : "OFF").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), Color::BLACK, FONT_ALIGN_RIGHT);
        }
        
        if (InstanceManager::getClientWorld())
        {
            bool activePlayerIds[4] = {false};
            
            int enemyCount = 0;
            int objectCount = 0;
            std::vector<Entity*> entities = InstanceManager::getClientWorld()->getEntities();
            for (Entity* go : entities)
            {
//                if (go->getNetworkType() == NW_TYPE_Enemy)
//                {
//                    ++enemyCount;
//                }
                if (go->getRTTI().derivesFrom(Crate::rtti))
                {
                    ++objectCount;
                }
            }
            
            std::vector<Entity*> players = InstanceManager::getClientWorld()->getPlayers();
            for (Entity* entity : players)
            {
                Robot* robot = static_cast<Robot*>(entity);
                
                int playerId = robot->getPlayerId();
                if (playerId >= 1 && playerId <= 4)
                {
                    renderText(StringUtil::format("%i|%s - %i HP", playerId, robot->getPlayerName().c_str(), robot->getHealth()).c_str(), 0.5f, CAM_HEIGHT - (playerId * 0.5f), Color::BLACK);
                    activePlayerIds[playerId - 1] = true;
                }
            }
            
            for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
            {
                if (!activePlayerIds[i])
                {
                    renderText(StringUtil::format("%i|%s", (i + 1), "Connect a controller to join...").c_str(), 0.5f, CAM_HEIGHT - ((i + 1) * 0.5f), Color::BLACK);
                }
            }
            
            renderText(StringUtil::format("Enemies: %i", enemyCount).c_str(), 0.5f, CAM_HEIGHT - 4.0f, Color::BLACK, FONT_ALIGN_LEFT);
            renderText(StringUtil::format("Objects: %i", objectCount).c_str(), 0.5f, CAM_HEIGHT - 4.5f, Color::BLACK, FONT_ALIGN_LEFT);
        }
    }
    else
    {
        renderText(StringUtil::format("%s, 'ESC' to exit", "Joining Server...").c_str(), 0.5f, CAM_HEIGHT - 3.5f, Color::BLACK, FONT_ALIGN_LEFT);
    }
    
    _spriteBatcher->endBatch(_textureManager->getTextures()[0], *_textureNGShader);
}

void GameRenderer::renderText(const char* inStr, float x, float y, const Color& inColor, int justification)
{
    Color fontColor = Color(inColor.red, inColor.green, inColor.blue, inColor.alpha);
    float fgWidth = CAM_WIDTH / 60;
    float fgHeight = fgWidth * 1.171875f;
    
    std::string text(inStr);
    
    _font->renderText(*_spriteBatcher, text, x, y, fgWidth, fgHeight, fontColor, justification);
}

void GameRenderer::endFrame()
{
    assert(_framebufferIndex >= 0);
    
    _rendererHelper->bindToScreenFramebuffer();
    _rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
    
    _rendererHelper->clearScreenVertices();
    _rendererHelper->addVertexCoordinate(-1, -1);
    _rendererHelper->addVertexCoordinate(-1, 1);
    _rendererHelper->addVertexCoordinate(1, 1);
    _rendererHelper->addVertexCoordinate(1, -1);
    
    _framebufferToScreenNGShader->bind(_rendererHelper->getFramebuffer(_framebufferIndex));
    _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
    _framebufferToScreenNGShader->unbind();
}
