//
//  GameRenderer.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/game/GameRenderer.h>

#include <framework/graphics/portable/TextureManager.h>
#include <framework/graphics/portable/Font.h>
#include <game/logic/World.h>
#include <framework/graphics/portable/Box2DDebugRenderer.h>
#include <framework/graphics/portable/SpriteBatcher.h>
#include <framework/graphics/portable/PolygonBatcher.h>
#include <framework/graphics/portable/LineBatcher.h>
#include <framework/graphics/portable/CircleBatcher.h>
#include <framework/graphics/portable/ShaderProgramLoader.h>
#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/NGShader.h>
#include <framework/entity/Entity.h>
#include <framework/graphics/portable/TextureRegion.h>
#include <framework/math/NGRect.h>
#include <framework/graphics/portable/NGTexture.h>
#include <framework/graphics/portable/Box2DDebugRenderer.h>
#include <framework/graphics/portable/NGLightingShader.h>

#include <framework/graphics/portable/Color.h>
#include <framework/file/portable/Assets.h>
#include <framework/graphics/portable/RendererHelper.h>
#include <framework/util/Constants.h>
#include <framework/math/NGRect.h>
#include <framework/math/Line.h>
#include <framework/graphics/portable/SpriteBatcher.h>
#include <framework/graphics/portable/TextureRegion.h>
#include <framework/util/macros.h>
#include <framework/network/client/NetworkManagerClient.h>
#include <framework/util/StringUtil.h>
#include <framework/util/WeightedTimedMovingAverage.h>
#include <framework/util/NGSTDUtil.h>
#include <game/game/GameInputManager.h>
#include <framework/network/server/NetworkManagerServer.h>
#include <framework/network/portable/MachineAddress.h>
#include <framework/util/MathUtil.h>
#include <framework/audio/portable/NGAudioEngine.h>
#include <game/logic/Server.h>
#include <game/title/TitleEngine.h>
#include <framework/util/FPSUtil.h>
#include <framework/math/Circle.h>
#include <framework/graphics/portable/CircleBatcher.h>
#include <framework/graphics/portable/PolygonBatcher.h>
#include <framework/graphics/portable/LineBatcher.h>
#include <framework/util/Constants.h>
#include <framework/graphics/portable/TextureWrapper.h>
#include <framework/graphics/portable/TextureDataWrapper.h>
#include <framework/graphics/portable/TextureLoaderFactory.h>
#include <framework/graphics/portable/ShaderProgramLoaderFactory.h>
#include <framework/graphics/portable/RendererHelperFactory.h>
#include <framework/util/NGSTDUtil.h>
#include <framework/math/Circle.h>
#include <framework/math/Line.h>
#include <framework/graphics/portable/NGTextureShader.h>
#include <framework/graphics/portable/NGGeometryShader.h>
#include <framework/graphics/portable/NGFramebufferToScreenShader.h>
#include <framework/graphics/portable/NGTextureDesc.h>
#include <framework/file/portable/Assets.h>
#include <game/game/GameEngine.h>
#include <game/entity/PlayerController.h>
#include <game/logic/GameConfig.h>
#include <framework/graphics/portable/VertexProgramInput.h>

#ifdef NG_STEAM
#include <framework/network/steam/NGSteamGameServer.h>
#include <framework/network/steam/NGSteamGameServices.h>
#endif

#include <sstream>
#include <string>
#include <assert.h>
#include <vector>
#include <cfloat>

GameRenderer::GameRenderer() : Renderer(),
_textureManager(new TextureManager()),
_rendererHelper(RENDERER_HELPER_FACTORY->createRendererHelper()),
_staticFontSpriteBatcher(new SpriteBatcher(_rendererHelper)),
_dynamicFontSpriteBatcher(new SpriteBatcher(_rendererHelper)),
_fillPolygonBatcher(new PolygonBatcher(_rendererHelper, true)),
_boundsPolygonBatcher(new PolygonBatcher(_rendererHelper, false)),
_lineBatcher(new LineBatcher(_rendererHelper)),
_circleBatcher(new CircleBatcher(_rendererHelper)),
_box2DDebugRenderer(new Box2DDebugRenderer(*_fillPolygonBatcher, *_boundsPolygonBatcher, *_lineBatcher, *_circleBatcher)),
_shaderProgramLoader(SHADER_PROGRAM_LOADER_FACTORY->createShaderLoader()),
_textureNGShader(new NGTextureShader(*_rendererHelper)),
_colorNGShader(new NGGeometryShader(*_rendererHelper)),
_lightingNGShader(new NGLightingShader(*_rendererHelper)),
_framebufferToScreenNGShader(new NGFramebufferToScreenShader(*_rendererHelper)),
_font(new Font(0, 0, 16, 64, 75, 1024, 1024)),
_fbIndex(0),
_map(0),
_engine(NULL),
_engineState(0),
_fontTexture(NULL),
_textDisplayState(0)
{
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        _spriteBatchers[i] = new SpriteBatcher(_rendererHelper);
        _spriteBatchers[i]->_index = i;
    }
    
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        _camBounds[i] = new NGRect(0, 0, GM_CFG->_camWidth, GM_CFG->_camHeight);
    }
    
    _staticFontSpriteBatcher->_index = 10;
    _dynamicFontSpriteBatcher->_index = 10;
}

GameRenderer::~GameRenderer()
{
    delete _textureManager;
    delete _rendererHelper;
    delete _staticFontSpriteBatcher;
    delete _dynamicFontSpriteBatcher;
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
    
    _fontTexture = _textureManager->getTextureWithName("texture_000.ngt");
    
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        _camBounds[i]->setWidth(GM_CFG->_camWidth);
        _camBounds[i]->setHeight(GM_CFG->_camHeight);
    }
    
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
    uint32_t textDisplayState = calcTextDisplayState();
    if (_engineState != _engine->_state ||
        _textDisplayState != textDisplayState)
    {
        _staticFontSpriteBatcher->_isDynamic = true;
        _staticFontSpriteBatcher->_isStaticBatchRendered = false;
    }
    
    _engineState = _engine->_state;
    _textDisplayState = textDisplayState;
    
    if (_textureManager->ensureTextures())
    {
        updateCamera();
        
        renderWorld();
        
        if (_engineState & GameEngineState_DisplayBox2D)
        {
            renderBox2D();
        }
        
        if (_engineState & GameEngineState_DisplayUI)
        {
            renderUI();
        }
        
        endFrame();
    }
}

void GameRenderer::onNewMapLoaded()
{
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        _spriteBatchers[i]->_isDynamic = true;
        _spriteBatchers[i]->_isStaticBatchRendered = false;
        _spriteBatchers[i]->_index = i;
    }
}

void GameRenderer::setEngine(GameEngine* inValue)
{
    _engine = inValue;
}

void GameRenderer::updateCamera()
{
    // Adjust camera based on the player position
    float x = 0;
    float y = 0;
    
    _playerLights.clear();
    _lights.clear();
    
    bool isCamInitialized = false;
    
    for (Entity* entity : _engine->_world->getPlayers())
    {
        float pX = entity->getPosition().x;
        float pY = entity->getPosition().y;
        float lightPosY = pY - entity->getHeight() / 2 + entity->getHeight() * GM_CFG->_robotLightPositionFactorY;
        
        _playerLights.push_back(LightDef(pX, lightPosY, GM_CFG->_playerLightColor[0], GM_CFG->_playerLightColor[1], GM_CFG->_playerLightColor[2], GM_CFG->_playerLightColor[3]));
        
        PlayerController* robot = static_cast<PlayerController*>(entity->getController());
        if (!isCamInitialized && robot->isLocalPlayer())
        {
            x = pX;
            y = pY;
            
            x -= GM_CFG->_camWidth * 0.5f;
            y -= GM_CFG->_camHeight * 0.5f;
            
            x = clamp(x, FLT_MAX, 0);
            y = clamp(y, FLT_MAX, 0);
            
            isCamInitialized = true;
        }
    }
    
    /// Temporary
    _lights.push_back(LightDef(GM_CFG->_tempStaticLight1[0], GM_CFG->_tempStaticLight1[1], GM_CFG->_tempStaticLight1[2], GM_CFG->_tempStaticLight1[3], GM_CFG->_tempStaticLight1[4], GM_CFG->_tempStaticLight1[5]));
    _lights.push_back(LightDef(GM_CFG->_tempStaticLight2[0], GM_CFG->_tempStaticLight2[1], GM_CFG->_tempStaticLight2[2], GM_CFG->_tempStaticLight2[3], GM_CFG->_tempStaticLight2[4], GM_CFG->_tempStaticLight2[5]));
    
    _camBounds[3]->getLowerLeft().set(x, y);
    
    _camBounds[2]->getLowerLeft().set(x * GM_CFG->_parallaxLayer2FactorX, y * GM_CFG->_parallaxLayer2FactorY);
    _camBounds[1]->getLowerLeft().set(x * GM_CFG->_parallaxLayer1FactorX, y * GM_CFG->_parallaxLayer1FactorY);
    _camBounds[0]->getLowerLeft().set(x * GM_CFG->_parallaxLayer0FactorX, y * GM_CFG->_parallaxLayer0FactorY);
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
    World* world = _engine->_world;
    
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        if (_spriteBatchers[i]->_isDynamic)
        {
            _spriteBatchers[i]->beginBatch();
        }
    }
    
    std::fill(_textures, _textures + NUM_SPRITE_BATCHERS, "");
    
    renderEntities(world->getLayers());
    renderEntities(world->getStaticEntities());
    renderEntities(world->getPlayers());
    renderEntities(world->getDynamicEntities());
    
    _rendererHelper->useNormalBlending();
    setFramebuffer(0, 0, 0, 0, 0);
    for (int i = 0; i < 5; ++i)
    {
        endBatchWithTexture(_spriteBatchers[i], _textureManager->getTextureWithName(_textures[i]), i);
    }
    
    _rendererHelper->useScreenBlending();
    setFramebuffer(1, 0, 0, 0, 0);
    endBatchWithTexture(_spriteBatchers[5], _textureManager->getTextureWithName(_textures[5]), 5);
    
    _rendererHelper->useNormalBlending();
    setFramebuffer(2, 0, 0, 0, 0);
    for (int i = 6; i < 9; ++i)
    {
        endBatchWithTexture(_spriteBatchers[i], _textureManager->getTextureWithName(_textures[i]), i);
    }
    
    int fbBegin = 0;
    int fbEnd = 3;
    
    if (_engineState & GameEngineState_Lighting)
    {
        fbBegin = 6;
        fbEnd = 9;
        
        renderLighting();
    }
    
    _rendererHelper->useScreenBlending();
    setFramebuffer(9, 0, 0, 0, 0);
    for (int i = fbBegin; i < fbEnd; ++i)
    {
        _framebufferToScreenNGShader->bind(_rendererHelper->getFramebuffer(i));
        _rendererHelper->bindScreenVertexBuffer();
        _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
        _framebufferToScreenNGShader->unbind();
    }
}

void GameRenderer::renderEntities(std::vector<Entity*>& entities)
{
    for (Entity* e : entities)
    {
        TextureRegion tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
        
        int layer = e->getEntityDef().layer;
        _textures[e->getEntityDef().layer] = tr.getTextureName();
        _normals[e->getEntityDef().layer] = tr.getNormalMapName();
        
        if (_spriteBatchers[layer]->_isStaticBatchRendered)
        {
            continue;
        }
        
        _spriteBatchers[layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
    }
}

void GameRenderer::renderLighting()
{
    _rendererHelper->useNormalBlending();
    setFramebuffer(3, 0, 0, 0, 0);
    for (int i = 0; i < 5; ++i)
    {
        endBatchWithTexture(_spriteBatchers[i], _textureManager->getTextureWithName(_normals[i]), i);
    }
    
    _rendererHelper->useScreenBlending();
    setFramebuffer(4, 0, 0, 0, 0);
    endBatchWithTexture(_spriteBatchers[5], _textureManager->getTextureWithName(_normals[5]), 5);
    
    _rendererHelper->useNormalBlending();
    setFramebuffer(5, 0, 0, 0, 0);
    for (int i = 6; i < 9; ++i)
    {
        endBatchWithTexture(_spriteBatchers[i], _textureManager->getTextureWithName(_normals[i]), i);
    }
    
    _rendererHelper->useScreenBlending();
    
    {
        _lightingNGShader->resetLights();
        _lightingNGShader->configAmbientLight(GM_CFG->_ambientColor[0], GM_CFG->_ambientColor[1], GM_CFG->_ambientColor[2], GM_CFG->_ambientColor[3]);
        _lightingNGShader->configureFallOff(GM_CFG->_fallOff[0], GM_CFG->_fallOff[1], GM_CFG->_fallOff[2]);
        for (int j = 0 ; j < _playerLights.size(); ++j)
        {
            LightDef& ld = _playerLights[j];
            _lightingNGShader->addLight(ld._lightPosX, ld._lightPosY, GM_CFG->_behindPlayerLightZFactor * GM_CFG->_playerLightZ, ld._lightColorR, ld._lightColorG, ld._lightColorB, ld._lightColorA);
        }
        for (int j = 0 ; j < _lights.size(); ++j)
        {
            LightDef& ld = _lights[j];
            _lightingNGShader->addLight(ld._lightPosX, ld._lightPosY, GM_CFG->_behindPlayerLightZFactor * GM_CFG->_playerLightZ, ld._lightColorR, ld._lightColorG, ld._lightColorB, ld._lightColorA);
        }
        
        setFramebuffer(6, 0, 0, 0, 0);
        _lightingNGShader->bind(_rendererHelper->getFramebuffer(0), _rendererHelper->getFramebuffer(3));
        _rendererHelper->bindScreenVertexBuffer();
        _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
        _lightingNGShader->unbind();
    }
    
    {
        _lightingNGShader->resetLights();
        _lightingNGShader->configAmbientLight(1, 1, 1, 1);
        _lightingNGShader->configureFallOff(GM_CFG->_fallOff[0], GM_CFG->_fallOff[1], GM_CFG->_fallOff[2]);
        for (int j = 0 ; j < _lights.size(); ++j)
        {
            LightDef& ld = _lights[j];
            _lightingNGShader->addLight(ld._lightPosX, ld._lightPosY, GM_CFG->_playerLightZ, ld._lightColorR, ld._lightColorG, ld._lightColorB, ld._lightColorA);
        }
        
        setFramebuffer(7, 0, 0, 0, 0);
        _lightingNGShader->bind(_rendererHelper->getFramebuffer(1), _rendererHelper->getFramebuffer(4));
        _rendererHelper->bindScreenVertexBuffer();
        _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
        _lightingNGShader->unbind();
    }
    
    {
        _lightingNGShader->resetLights();
        _lightingNGShader->configAmbientLight(GM_CFG->_ambientColor[0], GM_CFG->_ambientColor[1], GM_CFG->_ambientColor[2], GM_CFG->_ambientColor[3]);
        _lightingNGShader->configureFallOff(GM_CFG->_fallOff[0], GM_CFG->_fallOff[1], GM_CFG->_fallOff[2]);
        for (int j = 0 ; j < _playerLights.size(); ++j)
        {
            LightDef& ld = _playerLights[j];
            _lightingNGShader->addLight(ld._lightPosX, ld._lightPosY, GM_CFG->_frontPlayerLightZFactor * GM_CFG->_playerLightZ, ld._lightColorR, ld._lightColorG, ld._lightColorB, ld._lightColorA);
        }
        for (int j = 0 ; j < _lights.size(); ++j)
        {
            LightDef& ld = _lights[j];
            _lightingNGShader->addLight(ld._lightPosX, ld._lightPosY, GM_CFG->_frontPlayerLightZFactor * GM_CFG->_playerLightZ, ld._lightColorR, ld._lightColorG, ld._lightColorB, ld._lightColorA);
        }
        
        setFramebuffer(8, 0, 0, 0, 0);
        _lightingNGShader->bind(_rendererHelper->getFramebuffer(2), _rendererHelper->getFramebuffer(5));
        _rendererHelper->bindScreenVertexBuffer();
        _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
        _lightingNGShader->unbind();
    }
}

void GameRenderer::endBatchWithTexture(SpriteBatcher* sb, NGTexture* tex, int layer)
{
    if (tex)
    {
        int c = clamp(layer, 3, 0);
        _rendererHelper->updateMatrix(_camBounds[c]->getLeft(), _camBounds[c]->getRight(), _camBounds[c]->getBottom(), _camBounds[c]->getTop());
        
        _spriteBatchers[layer]->_index = layer;
        _spriteBatchers[layer]->_isDynamic = layer == 4 || layer == 5 || layer == 6 || layer == 7;
        sb->endBatch(_textureNGShader, tex);
    }
}

void GameRenderer::renderBox2D()
{
	_rendererHelper->updateMatrix(_camBounds[3]->getLeft(), _camBounds[3]->getRight(), _camBounds[3]->getBottom(), _camBounds[3]->getTop());

    _box2DDebugRenderer->render(&_engine->_world->getWorld(), _colorNGShader);
}

void GameRenderer::renderUI()
{
    _rendererHelper->updateMatrix(0, GM_CFG->_camWidth, 0, GM_CFG->_camHeight);
    
    if (_staticFontSpriteBatcher->_isDynamic)
    {
        _staticFontSpriteBatcher->beginBatch();
    }
    
    if (NG_CLIENT->getState() == NCS_Welcomed)
    {
        _dynamicFontSpriteBatcher->beginBatch();
        
        int row = 1;
        static float padding = 1;
        
        int fps = FPSUtil::getInstance()->getFPS();
        renderText(_dynamicFontSpriteBatcher, StringUtil::format("FPS %d", fps).c_str(), GM_CFG->_camWidth - 0.5f, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        
        float rttMS = NG_CLIENT->getAvgRoundTripTime().getValue() * 1000.f;
        renderText(_dynamicFontSpriteBatcher, StringUtil::format("RTT %d ms", static_cast<int>(rttMS)).c_str(), GM_CFG->_camWidth - 0.5f, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        
        const WeightedTimedMovingAverage& bpsIn = NG_CLIENT->getBytesReceivedPerSecond();
        int bpsInInt = static_cast<int>(bpsIn.getValue());
        renderText(_dynamicFontSpriteBatcher, StringUtil::format(" In %d Bps", bpsInInt).c_str(), GM_CFG->_camWidth - 0.5f, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        
        const WeightedTimedMovingAverage& bpsOut = NG_CLIENT->getBytesSentPerSecond();
        int bpsOutInt = static_cast<int>(bpsOut.getValue());
        renderText(_dynamicFontSpriteBatcher, StringUtil::format("Out %d Bps", bpsOutInt).c_str(), GM_CFG->_camWidth - 0.5f, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        
        _dynamicFontSpriteBatcher->endBatch(_textureNGShader, _fontTexture);
        
        if (!_staticFontSpriteBatcher->_isStaticBatchRendered)
        {
            // Controls
            ++row;
            
            renderText(_staticFontSpriteBatcher, StringUtil::format("[S]         Sound %s", NG_AUDIO_ENGINE->areSoundsDisabled() ? " OFF" : "  ON").c_str(), GM_CFG->_camWidth - 0.5f, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
            renderText(_staticFontSpriteBatcher, StringUtil::format("[M]         Music %s", NG_AUDIO_ENGINE->isMusicDisabled() ? " OFF" : "  ON").c_str(), GM_CFG->_camWidth - 0.5f, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
            renderText(_staticFontSpriteBatcher, StringUtil::format("[B]   Box2D Debug %s", _engineState & GameEngineState_DisplayBox2D ? "  ON" : " OFF").c_str(), GM_CFG->_camWidth - 0.5f, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
            renderText(_staticFontSpriteBatcher, StringUtil::format("[I] Interpolation %s", _engineState & GameEngineState_Interpolation ? "  ON" : " OFF").c_str(), GM_CFG->_camWidth - 0.5f, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
            std::string lightZ = StringUtil::format("%f", GM_CFG->_playerLightZ);
            renderText(_staticFontSpriteBatcher, StringUtil::format("[L]  Lighting %s", _engineState & GameEngineState_Lighting ? lightZ.c_str() : "     OFF").c_str(), GM_CFG->_camWidth - 0.5f, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
            renderText(_staticFontSpriteBatcher, StringUtil::format("[U]    Display UI %s", _engineState & GameEngineState_DisplayUI ? "  ON" : " OFF").c_str(), GM_CFG->_camWidth - 0.5f, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
            
            if (Server::getInstance())
            {
                renderText(_staticFontSpriteBatcher, StringUtil::format("[T]    Toggle Map %s", _engine->_world->getMapName().c_str()).c_str(), GM_CFG->_camWidth - 0.5f, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
            }
            else
            {
                renderText(_staticFontSpriteBatcher, StringUtil::format("              Map %s", _engine->_world->getMapName().c_str()).c_str(), GM_CFG->_camWidth - 0.5f, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
            }
            
            bool activePlayerIds[4] = {false};
            
            std::vector<Entity*> players = _engine->_world->getPlayers();
            for (Entity* entity : players)
            {
                PlayerController* robot = static_cast<PlayerController*>(entity->getController());
                
                int playerId = robot->getPlayerId();
                if (playerId >= 1 && playerId <= 4)
                {
                    renderText(_staticFontSpriteBatcher, StringUtil::format("%i|%s", playerId, robot->getPlayerName().c_str()).c_str(), 0.5f, GM_CFG->_camHeight - (playerId * 1.0f), FONT_ALIGN_LEFT);
                    activePlayerIds[playerId - 1] = true;
                }
            }
            
            for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
            {
                if (!activePlayerIds[i])
                {
                    renderText(_staticFontSpriteBatcher, StringUtil::format("%i|%s", (i + 1), "Connect a controller to join...").c_str(), 0.5f, GM_CFG->_camHeight - ((i + 1) * 1.0f), FONT_ALIGN_LEFT);
                }
            }
        }
    }
    else
    {
        renderText(_staticFontSpriteBatcher, StringUtil::format("%s, [ESC] to exit", "Joining Server...").c_str(), 0.5f, GM_CFG->_camHeight - 4, FONT_ALIGN_LEFT);
    }
    
    _staticFontSpriteBatcher->_isDynamic = false;
    _staticFontSpriteBatcher->endBatch(_textureNGShader, _fontTexture);
}

void GameRenderer::renderText(SpriteBatcher* sb, const char* inStr, float x, float y, int justification)
{
    float fgWidth = GM_CFG->_camWidth / 64;
    float fgHeight = fgWidth * 1.171875f;
    
    std::string text(inStr);
    
    _font->renderText(*sb, text, x, y, fgWidth, fgHeight, justification);
}

void GameRenderer::endFrame()
{
    assert(_fbIndex >= 0);
    
    _rendererHelper->bindToScreenFramebuffer();
    _rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
    _rendererHelper->useScreenBlending();

    _framebufferToScreenNGShader->bind(_rendererHelper->getFramebuffer(_fbIndex));
    _rendererHelper->bindScreenVertexBuffer();
    _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
    _framebufferToScreenNGShader->unbind();
    
    _rendererHelper->useNoBlending();
}

uint32_t GameRenderer::calcTextDisplayState()
{
    uint32_t ret = 0;
    
    std::vector<Entity*> players = _engine->_world->getPlayers();
    for (Entity* entity : players)
    {
        PlayerController* robot = static_cast<PlayerController*>(entity->getController());
        
        int playerId = robot->getPlayerId();
        if (playerId >= 1 && playerId <= 4)
        {
            int bitShift = playerId - 1;
            ret |= 1 << bitShift;
        }
    }
    
    ret |= NG_AUDIO_ENGINE->areSoundsDisabled() ? TextDisplayState_SoundDisabled : 0;
    ret |= NG_AUDIO_ENGINE->isMusicDisabled() ? TextDisplayState_MusicDisabled : 0;
    
    std::string& mapName = _engine->_world->getMapName();
    ret |= mapName == "Z001" ? TextDisplayState_MapZone1 : mapName == "Z002" ? TextDisplayState_MapZone2 : 0;
    
    return ret;
}
