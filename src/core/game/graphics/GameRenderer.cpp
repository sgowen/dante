//
//  GameRenderer.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/graphics/GameRenderer.h"

#include "framework/graphics/portable/TextureManager.h"
#include "framework/graphics/portable/Font.h"
#include "game/logic/World.h"
#include "framework/graphics/portable/Box2DDebugRenderer.h"
#include "framework/graphics/portable/SpriteBatcher.h"
#include "framework/graphics/portable/PolygonBatcher.h"
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
#include <framework/graphics/portable/NGLightingShader.h>

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
#include "framework/graphics/portable/PolygonBatcher.h"
#include "framework/graphics/portable/LineBatcher.h"
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
#include <game/logic/PlayerController.h>
#include <framework/util/Config.h>
#include <framework/graphics/portable/VertexProgramInput.h>

#ifdef NG_STEAM
#include "framework/network/steam/NGSteamGameServer.h"
#include "framework/network/steam/NGSteamGameServices.h"
#endif

#include <sstream>
#include <string>
#include <assert.h>
#include <vector>
#include <cfloat>

GameRenderer::GameRenderer() : Renderer(),
_textureManager(new TextureManager("game_assets.cfg")),
_rendererHelper(RENDERER_HELPER_FACTORY->createRendererHelper()),
_fillPolygonBatcher(new PolygonBatcher(_rendererHelper, true)),
_boundsPolygonBatcher(new PolygonBatcher(_rendererHelper, false)),
_lineBatcher(new LineBatcher(_rendererHelper)),
_circleBatcher(new CircleBatcher(_rendererHelper)),
_box2DDebugRenderer(new Box2DDebugRenderer(*_fillPolygonBatcher, *_boundsPolygonBatcher, *_lineBatcher, *_circleBatcher)),
_shaderProgramLoader(SHADER_PROGRAM_LOADER_FACTORY->createNGShaderLoader()),
_textureNGShader(new NGTextureShader(*_rendererHelper, "shader_003_vert.ngs", "shader_003_frag.ngs")),
_colorNGShader(new NGGeometryShader(*_rendererHelper, "shader_001_vert.ngs", "shader_001_frag.ngs")),
_lightingNGShader(new NGLightingShader(*_rendererHelper, "shader_004_vert.ngs", "shader_004_frag.ngs")),
_framebufferToScreenNGShader(new NGFramebufferToScreenShader(*_rendererHelper, "shader_002_vert.ngs", "shader_002_frag.ngs")),
_font(new Font("texture_000.ngt", 0, 0, 16, 64, 75, 1024, 1024)),
_fbIndex(0),
_engine(NULL),
_engineState(0)
{
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        _spriteBatchers[i] = new SpriteBatcher(_rendererHelper);
    }
    
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        _camBounds[i] = new NGRect(0, 0, CAM_WIDTH, CAM_HEIGHT);
    }
}

GameRenderer::~GameRenderer()
{
    releaseDeviceDependentResources();
    
    delete _textureManager;
    delete _rendererHelper;
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        delete _spriteBatchers[i];
    }
    delete _fillPolygonBatcher;
    delete _boundsPolygonBatcher;
    delete _lineBatcher;
    delete _circleBatcher;
    delete _box2DDebugRenderer;
    delete _shaderProgramLoader;
    delete _font;
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        delete _camBounds[i];
    }
    delete _textureNGShader;
    delete _colorNGShader;
    delete _lightingNGShader;
    delete _framebufferToScreenNGShader;
}

void GameRenderer::createDeviceDependentResources()
{
    _rendererHelper->createDeviceDependentResources();
    _textureManager->createDeviceDependentResources();
    
    _textureNGShader->load(*_shaderProgramLoader);
    _colorNGShader->load(*_shaderProgramLoader);
    _lightingNGShader->load(*_shaderProgramLoader);
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
    _lightingNGShader->unload(*_shaderProgramLoader);
    _framebufferToScreenNGShader->unload(*_shaderProgramLoader);
}

void GameRenderer::render()
{
    _engineState = _engine->_state;
    
    setFramebuffer(0);
    
    if (_textureManager->ensureTextures())
    {
        renderWorld();
        
        if (_engineState & GameEngineState_DisplayBox2D)
        {
            renderBox2D();
        }
        
        renderUI();
    }
    
    endFrame();
}

void GameRenderer::setEngine(GameEngine* inValue)
{
    _engine = inValue;
}

void GameRenderer::updateCamera()
{
    _lightingNGShader->resetLights();
    
    // Adjust camera based on the player position
    float x = 0;
    float y = 0;
    
    if (NG_CLIENT && NG_CLIENT->getState() == NCS_Welcomed)
    {
        bool isCamInitialized = false;
        int numLights = 0;
        
        for (Entity* entity : InstanceManager::getClientWorld()->getPlayers())
        {
            PlayerController* robot = static_cast<PlayerController*>(entity->getController());
            
            float pX = entity->getPosition().x;
            float pY = entity->getPosition().y;
            
            _lightingNGShader->configLight(numLights++, pX, pY + entity->getHeight() / 4);
            
            if (!isCamInitialized && robot->isLocalPlayer())
            {
                x = pX;
                y = pY;
                
                x -= CAM_WIDTH * 0.5f;
                y -= CAM_HEIGHT * 0.5f;
                
                x = clamp(x, FLT_MAX, 0);
                y = clamp(y, FLT_MAX, 0);
                
                isCamInitialized = true;
            }
        }
    }
    
    _camBounds[3]->getLowerLeft().set(x, y);
    _camBounds[2]->getLowerLeft().set(x / 2, y / 2);
    _camBounds[1]->getLowerLeft().set(x / 4, y / 4);
    _camBounds[0]->getLowerLeft().set(x / 8, y / 8);
}

void GameRenderer::setFramebuffer(int framebufferIndex, float r, float g, float b, float a)
{
    assert(framebufferIndex >= 0);
    
    _fbIndex = framebufferIndex;
    
    _rendererHelper->bindToOffscreenFramebuffer(_fbIndex);
    _rendererHelper->clearFramebufferWithColor(r, g, b, a);
}

void GameRenderer::renderWorld()
{
    _rendererHelper->useNormalBlending();
    setFramebuffer(0);
    renderLayers(InstanceManager::getClientWorld(), false);
    if (_engineState & GameEngineState_Lighting)
    {
        setFramebuffer(_fbIndex + 1);
        renderLayers(InstanceManager::getClientWorld(), true);
    }
    
    _rendererHelper->updateMatrix(_camBounds[3]->getLeft(), _camBounds[3]->getRight(), _camBounds[3]->getBottom(), _camBounds[3]->getTop());
    setFramebuffer(_fbIndex + 1, 0, 0, 0, 0);
    _rendererHelper->useScreenBlending();
    renderEntities(InstanceManager::getClientWorld(), false, false);
    if (Server::getInstance() && Server::getInstance()->isDisplaying())
    {
        renderEntities(InstanceManager::getServerWorld(), false, true);
    }
    if (_engineState & GameEngineState_Lighting)
    {
        setFramebuffer(_fbIndex + 1, 0, 0, 0, 0);
        _rendererHelper->useScreenBlending();
        renderEntities(InstanceManager::getClientWorld(), true, false);
        if (Server::getInstance() && Server::getInstance()->isDisplaying())
        {
            renderEntities(InstanceManager::getServerWorld(), true, true);
        }
    }
    
    int fbBegin = 0;
    
    if (_engineState & GameEngineState_Lighting)
    {
        static TextureRegion tr = TextureRegion("framebuffer", 0, 0, 1, 1, 1, 1);
        
        float x = _camBounds[3]->getLeft() + _camBounds[3]->getWidth() / 2;
        float y = _camBounds[3]->getBottom() + _camBounds[3]->getHeight() / 2;
        
        _rendererHelper->useScreenBlending();
        
        fbBegin = _fbIndex + 1;
        
        {
            _lightingNGShader->configZ(_engine->_lightZ * NG_CFG->getFloat("backgroundLightZFactor"));
            setFramebuffer(_fbIndex + 1, 0, 0, 0, 0);
            _spriteBatchers[0]->beginBatch();
            _spriteBatchers[0]->renderSprite(x, y, _camBounds[3]->getWidth(), _camBounds[3]->getHeight(), 0, tr);
            _spriteBatchers[0]->endBatch(_lightingNGShader, _rendererHelper->getFramebuffer(0), _rendererHelper->getFramebuffer(1));
        }
        
        {
            _lightingNGShader->configZ(_engine->_lightZ);
            setFramebuffer(_fbIndex + 1, 0, 0, 0, 0);
            _spriteBatchers[0]->beginBatch();
            _spriteBatchers[0]->renderSprite(x, y, _camBounds[3]->getWidth(), _camBounds[3]->getHeight(), 0, tr);
            _spriteBatchers[0]->endBatch(_lightingNGShader, _rendererHelper->getFramebuffer(2), _rendererHelper->getFramebuffer(3));
        }
    }
    
    setFramebuffer(_fbIndex + 1);
    
    _rendererHelper->useScreenBlending();
    for (int i = fbBegin; i < _fbIndex; ++i)
    {
        static std::vector<VERTEX_2D> screenVertices;
		screenVertices.clear();
		screenVertices.reserve(4);
		screenVertices.push_back(VERTEX_2D(-1, -1));
		screenVertices.push_back(VERTEX_2D(-1, 1));
		screenVertices.push_back(VERTEX_2D(1, 1));
		screenVertices.push_back(VERTEX_2D(1, -1));

        _framebufferToScreenNGShader->bind(&screenVertices, _rendererHelper->getFramebuffer(i));
        _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
        _framebufferToScreenNGShader->unbind();
    }
}

void GameRenderer::renderLayers(World* world, bool isNormals)
{
    static const int NUM_LAYERS = 4;
    
    for (int i = 0; i < NUM_LAYERS; ++i)
    {
        _spriteBatchers[i]->beginBatch();
    }
    
    std::string textures[NUM_LAYERS];
    
    std::vector<Entity*> entities = world->getLayers();
    for (Entity* e : entities)
    {
        TextureRegion tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
        
        _spriteBatchers[e->getEntityDef().layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
        textures[e->getEntityDef().layer] = isNormals ? tr.getNormalMapName() : tr.getTextureName();
    }
    
    for (int i = 0; i < NUM_LAYERS; ++i)
    {
        if (textures[i].length() > 0)
        {
            _rendererHelper->updateMatrix(_camBounds[i]->getLeft(), _camBounds[i]->getRight(), _camBounds[i]->getBottom(), _camBounds[i]->getTop());
            _spriteBatchers[i]->endBatch(_textureNGShader, _textureManager->getTextureWithName(textures[i]));
        }
    }
}

void GameRenderer::renderEntities(World* world, bool isNormals, bool isServer)
{
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        _spriteBatchers[i]->beginBatch();
    }
    
    std::string textures[NUM_SPRITE_BATCHERS];
    
    {
        std::vector<Entity*> entities = world->getPlayers();
        for (Entity* e : entities)
        {
            TextureRegion tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
            
            _spriteBatchers[e->getEntityDef().layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
            textures[e->getEntityDef().layer] = isNormals ? tr.getNormalMapName() : tr.getTextureName();
        }
    }
    
    {
        std::vector<Entity*> entities = world->getDynamicEntities();
        for (Entity* e : entities)
        {
            TextureRegion tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
            
            _spriteBatchers[e->getEntityDef().layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
            textures[e->getEntityDef().layer] = isNormals ? tr.getNormalMapName() : tr.getTextureName();
        }
    }
    
    {
        std::vector<Entity*> entities = world->getStaticEntities();
        for (Entity* e : entities)
        {
            TextureRegion tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
            
            _spriteBatchers[e->getEntityDef().layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
            textures[e->getEntityDef().layer] = isNormals ? tr.getNormalMapName() : tr.getTextureName();
        }
    }
    
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        if (textures[i].length() > 0)
        {
            _spriteBatchers[i]->endBatch(_textureNGShader, _textureManager->getTextureWithName(textures[i]));
        }
    }
}

void GameRenderer::renderBox2D()
{
	_rendererHelper->updateMatrix(_camBounds[3]->getLeft(), _camBounds[3]->getRight(), _camBounds[3]->getBottom(), _camBounds[3]->getTop());

    _box2DDebugRenderer->render(&InstanceManager::getClientWorld()->getWorld(), _colorNGShader);
    
    if (Server::getInstance() && Server::getInstance()->isDisplaying())
    {
        _box2DDebugRenderer->render(&InstanceManager::getServerWorld()->getWorld(), _colorNGShader);
    }
}

void GameRenderer::renderUI()
{
    _rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
    
    _spriteBatchers[0]->beginBatch();
    
    if (NG_CLIENT->getState() == NCS_Welcomed)
    {
        int row = 1;
        static float padding = 1;
        
        int fps = FPSUtil::getInstance()->getFPS();
        renderText(StringUtil::format("FPS %d", fps).c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        
        float rttMS = NG_CLIENT->getAvgRoundTripTime().getValue() * 1000.f;
        renderText(StringUtil::format("RTT %d ms", static_cast<int>(rttMS)).c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        
        const WeightedTimedMovingAverage& bpsIn = NG_CLIENT->getBytesReceivedPerSecond();
        int bpsInInt = static_cast<int>(bpsIn.getValue());
        renderText(StringUtil::format(" In %d Bps", bpsInInt).c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        
        const WeightedTimedMovingAverage& bpsOut = NG_CLIENT->getBytesSentPerSecond();
        int bpsOutInt = static_cast<int>(bpsOut.getValue());
        renderText(StringUtil::format("Out %d Bps", bpsOutInt).c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        
        // Controls
        ++row;
        
        renderText(StringUtil::format("[S]         Sound %s", NG_AUDIO_ENGINE->areSoundsDisabled() ? " OFF" : "  ON").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[M]         Music %s", NG_AUDIO_ENGINE->isMusicDisabled() ? " OFF" : "  ON").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[B]   Box2D Debug %s", _engineState & GameEngineState_DisplayBox2D ? "  ON" : " OFF").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[L] Interpolation %s", _engineState & GameEngineState_Interpolation ? "  ON" : " OFF").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        std::string lightZ = StringUtil::format("%f", _engine->_lightZ);
        renderText(StringUtil::format("[Z]  Lighting %s", _engineState & GameEngineState_Lighting ? lightZ.c_str() : "     OFF").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        
        if (Server::getInstance())
        {
            renderText(StringUtil::format("[I]         Debug %s", Server::getInstance()->isDisplaying() ? "  ON" : " OFF").c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
            renderText(StringUtil::format("[T]    Toggle Map %s", InstanceManager::getServerWorld()->getMapName().c_str()).c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        }
        else
        {
            renderText(StringUtil::format("              Map %s", InstanceManager::getClientWorld()->getMapName().c_str()).c_str(), CAM_WIDTH - 0.5f, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        }
        
        if (InstanceManager::getClientWorld())
        {
            bool activePlayerIds[4] = {false};
            
            std::vector<Entity*> players = InstanceManager::getClientWorld()->getPlayers();
            for (Entity* entity : players)
            {
                PlayerController* robot = static_cast<PlayerController*>(entity->getController());
                
                int playerId = robot->getPlayerId();
                if (playerId >= 1 && playerId <= 4)
                {
                    renderText(StringUtil::format("%i|%s - %i HP", playerId, robot->getPlayerName().c_str(), robot->getHealth()).c_str(), 0.5f, CAM_HEIGHT - (playerId * 1.0f));
                    activePlayerIds[playerId - 1] = true;
                }
            }
            
            for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
            {
                if (!activePlayerIds[i])
                {
                    renderText(StringUtil::format("%i|%s", (i + 1), "Connect a controller to join...").c_str(), 0.5f, CAM_HEIGHT - ((i + 1) * 1.0f));
                }
            }
        }
    }
    else
    {
        renderText(StringUtil::format("%s, [ESC] to exit", "Joining Server...").c_str(), 0.5f, CAM_HEIGHT - 4, FONT_ALIGN_LEFT);
    }
    
    _spriteBatchers[0]->endBatch(_textureNGShader, _textureManager->getTextureWithName("texture_000.ngt"));
}

void GameRenderer::renderText(const char* inStr, float x, float y, int justification)
{
    float fgWidth = CAM_WIDTH / 60;
    float fgHeight = fgWidth * 1.171875f;
    
    std::string text(inStr);
    
    _font->renderText(*_spriteBatchers[0], text, x, y, fgWidth, fgHeight, justification);
}

void GameRenderer::endFrame()
{
    assert(_fbIndex >= 0);
    
    _rendererHelper->bindToScreenFramebuffer();
    _rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
    
    static std::vector<VERTEX_2D> screenVertices;
    screenVertices.clear();
    screenVertices.reserve(4);
    screenVertices.push_back(VERTEX_2D(-1, -1));
    screenVertices.push_back(VERTEX_2D(-1, 1));
    screenVertices.push_back(VERTEX_2D(1, 1));
    screenVertices.push_back(VERTEX_2D(1, -1));

    _framebufferToScreenNGShader->bind(&screenVertices, _rendererHelper->getFramebuffer(_fbIndex));
    _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
    _framebufferToScreenNGShader->unbind();
    
    _rendererHelper->useNoBlending();
}
