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
#include <framework/studio/World.h>
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
#include <game/game/Server.h>
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
#include <game/config/GameConfig.h>
#include <framework/graphics/portable/VertexProgramInput.h>
#include <framework/util/Config.h>
#include <framework/graphics/portable/FramebufferWrapper.h>

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
_textureShader(new NGTextureShader(*_rendererHelper)),
_colorShader(new NGGeometryShader(*_rendererHelper)),
_lightingNGShader(new NGLightingShader(*_rendererHelper)),
_framebufferToScreenShader(new NGFramebufferToScreenShader(*_rendererHelper)),
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
    }
    
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        _camBounds[i] = new NGRect(0, 0, FW_CFG->_camWidth, FW_CFG->_camHeight);
    }
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
    delete _textureShader;
    delete _colorShader;
    delete _lightingNGShader;
    delete _framebufferToScreenShader;
}

void GameRenderer::createDeviceDependentResources()
{
    _engine = GameEngine::getInstance();
    
    _rendererHelper->createDeviceDependentResources();
    _textureManager->createDeviceDependentResources();
    
    _fontTexture = _textureManager->getTextureWithName("texture_000.ngt");
    
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        _camBounds[i]->setWidth(FW_CFG->_camWidth);
        _camBounds[i]->setHeight(FW_CFG->_camHeight);
    }
    
    _textureShader->load(*_shaderProgramLoader);
    _colorShader->load(*_shaderProgramLoader);
    _lightingNGShader->load(*_shaderProgramLoader);
    _framebufferToScreenShader->load(*_shaderProgramLoader);
}

void GameRenderer::createWindowSizeDependentResources(int screenWidth, int screenHeight)
{
    _rendererHelper->createWindowSizeDependentResources(screenWidth, screenHeight);
}

void GameRenderer::releaseDeviceDependentResources()
{
    _rendererHelper->releaseDeviceDependentResources();
    _textureManager->releaseDeviceDependentResources();
    
    _textureShader->unload(*_shaderProgramLoader);
    _colorShader->unload(*_shaderProgramLoader);
    _lightingNGShader->unload(*_shaderProgramLoader);
    _framebufferToScreenShader->unload(*_shaderProgramLoader);
}

void GameRenderer::render()
{
    uint32_t textDisplayState = calcTextDisplayState();
    if (_engineState != _engine->_state ||
        _textDisplayState != textDisplayState)
    {
        _staticFontSpriteBatcher->useDynamicConfig();
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
        _spriteBatchers[i]->useDynamicConfig();
    }
}

void GameRenderer::updateCamera()
{
    // Adjust camera based on the player position
    float x = 0;
    float y = 0;
    
    _playerLights.clear();
    _lights.clear();
    
    bool isCamInitialized = false;
    
    for (Entity* e : _engine->_world->getPlayers())
    {
        float pX = e->getPosition().x;
        float pY = e->getPosition().y;
        float lightPosY = pY - e->getHeight() / 2 + e->getHeight() * GM_CFG->_robotLightPositionFactorY;
        
        _playerLights.push_back(LightDef(pX, lightPosY, GM_CFG->_playerLightColor[0], GM_CFG->_playerLightColor[1], GM_CFG->_playerLightColor[2], GM_CFG->_playerLightColor[3]));
        
        PlayerNetworkController* p = static_cast<PlayerNetworkController*>(e->getNetworkController());
        if (!isCamInitialized && p->isLocalPlayer())
        {
            x = pX;
            y = pY;
            
            x -= FW_CFG->_camWidth * 0.5f;
            y -= FW_CFG->_camHeight * 0.5f;
            
            x = clamp(x, 0, FLT_MAX);
            y = clamp(y, 0, FLT_MAX);
            
            isCamInitialized = true;
        }
    }
    
    /// Temporary
    _lights.push_back(LightDef(GM_CFG->_tempStaticLight1[0], GM_CFG->_tempStaticLight1[1], GM_CFG->_tempStaticLight1[2], GM_CFG->_tempStaticLight1[3], GM_CFG->_tempStaticLight1[4], GM_CFG->_tempStaticLight1[5]));
    _lights.push_back(LightDef(GM_CFG->_tempStaticLight2[0], GM_CFG->_tempStaticLight2[1], GM_CFG->_tempStaticLight2[2], GM_CFG->_tempStaticLight2[3], GM_CFG->_tempStaticLight2[4], GM_CFG->_tempStaticLight2[5]));
    
    _camBounds[3]->getLowerLeft().set(x, y);
    _camBounds[2]->getLowerLeft().set(x * FW_CFG->_parallaxLayer2FactorX, y * FW_CFG->_parallaxLayer2FactorY);
    _camBounds[1]->getLowerLeft().set(x * FW_CFG->_parallaxLayer1FactorX, y * FW_CFG->_parallaxLayer1FactorY);
    _camBounds[0]->getLowerLeft().set(x * FW_CFG->_parallaxLayer0FactorX, y * FW_CFG->_parallaxLayer0FactorY);
}

void GameRenderer::bindOffscreenFramebuffer(int fbIndex, float r, float g, float b, float a)
{
    assert(fbIndex >= 0);
    
    _fbIndex = fbIndex;
    
    FramebufferWrapper* fb = _rendererHelper->getOffscreenFramebuffer(_fbIndex);
    _rendererHelper->bindFramebuffer(fb);
    _rendererHelper->clearFramebufferWithColor(r, g, b, a);
}

void GameRenderer::renderWorld()
{
    World* world = _engine->_world;
    
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        if (_spriteBatchers[i]->isDynamic())
        {
            _spriteBatchers[i]->beginBatch(i);
        }
    }
    
    std::fill(_textures, _textures + NUM_SPRITE_BATCHERS, "");
    
    renderEntities(world->getLayers());
    renderEntities(world->getStaticEntities());
    renderEntities(world->getDynamicEntities());
    
    _rendererHelper->useNormalBlending();
    bindOffscreenFramebuffer(0);
    for (int i = 0; i < 5; ++i)
    {
        endBatchWithTexture(_spriteBatchers[i], _textureManager->getTextureWithName(_textures[i]), i);
    }
    
    _rendererHelper->useNormalBlending();
    bindOffscreenFramebuffer(1);
    endBatchWithTexture(_spriteBatchers[5], _textureManager->getTextureWithName(_textures[5]), 5);
    
    _rendererHelper->useNormalBlending();
    bindOffscreenFramebuffer(2);
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
    bindOffscreenFramebuffer(9);
    for (int i = fbBegin; i < fbEnd; ++i)
    {
        _framebufferToScreenShader->bind(_rendererHelper->getOffscreenFramebuffer(i)->texture);
        _rendererHelper->bindScreenVertexBuffer();
        _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
        _framebufferToScreenShader->unbind();
    }
}

void GameRenderer::renderEntities(std::vector<Entity*>& entities)
{
    for (Entity* e : entities)
    {
        TextureRegion tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
        
        int layer = tr._layer;
        _textures[layer] = tr.getTextureName();
        _normals[layer] = tr.getNormalMapName();
        
        if (_spriteBatchers[layer]->isStaticBatchRendered())
        {
            continue;
        }
        
        _spriteBatchers[layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
    }
}

void GameRenderer::renderLighting()
{
    _rendererHelper->useNormalBlending();
    bindOffscreenFramebuffer(3);
    for (int i = 0; i < 5; ++i)
    {
        endBatchWithTexture(_spriteBatchers[i], _textureManager->getTextureWithName(_normals[i]), i);
    }
    
    _rendererHelper->useNormalBlending();
    bindOffscreenFramebuffer(4);
    endBatchWithTexture(_spriteBatchers[5], _textureManager->getTextureWithName(_normals[5]), 5);
    
    _rendererHelper->useNormalBlending();
    bindOffscreenFramebuffer(5);
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
        
        bindOffscreenFramebuffer(6);
        _lightingNGShader->bind(_rendererHelper->getOffscreenFramebuffer(0)->texture, _rendererHelper->getOffscreenFramebuffer(3)->texture);
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
        
        bindOffscreenFramebuffer(7);
        _lightingNGShader->bind(_rendererHelper->getOffscreenFramebuffer(1)->texture, _rendererHelper->getOffscreenFramebuffer(4)->texture);
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
        
        bindOffscreenFramebuffer(8);
        _lightingNGShader->bind(_rendererHelper->getOffscreenFramebuffer(2)->texture, _rendererHelper->getOffscreenFramebuffer(5)->texture);
        _rendererHelper->bindScreenVertexBuffer();
        _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
        _lightingNGShader->unbind();
    }
}

void GameRenderer::endBatchWithTexture(SpriteBatcher* sb, NGTexture* tex, int layer)
{
    if (tex)
    {
        int c = clamp(layer, 0, 3);
        _rendererHelper->updateMatrix(_camBounds[c]->getLeft(), _camBounds[c]->getRight(), _camBounds[c]->getBottom(), _camBounds[c]->getTop());
        
        if (layer == 0 ||
            layer == 1 ||
            layer == 2 ||
            layer == 3 ||
            layer == 8)
        {
            _spriteBatchers[layer]->useStaticConfig();
        }
        
        sb->endBatch(_textureShader, tex);
    }
}

void GameRenderer::renderBox2D()
{
	_rendererHelper->updateMatrix(_camBounds[3]->getLeft(), _camBounds[3]->getRight(), _camBounds[3]->getBottom(), _camBounds[3]->getTop());

    _box2DDebugRenderer->render(&_engine->_world->getWorld(), _colorShader);
}

void GameRenderer::renderUI()
{
    _rendererHelper->updateMatrix(0, FW_CFG->_camWidth, 0, FW_CFG->_camHeight);
    
    int jl = FONT_ALIGN_LEFT;
    int jr = FONT_ALIGN_RIGHT;
    float fontX = FW_CFG->_camWidth - 0.5f;
    float camHeight = FW_CFG->_camHeight;
    SpriteBatcher* dSb = _dynamicFontSpriteBatcher;
    SpriteBatcher* sSb = _staticFontSpriteBatcher;
    
    if (sSb->isDynamic())
    {
        sSb->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
    }
    
    if (NG_CLIENT && NG_CLIENT->getState() == NCS_Welcomed)
    {
        dSb->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
        
        int row = 1;
        static float padding = 1;
        
        int fps = FPSUtil::getInstance()->getFPS();
        renderText(dSb, StringUtil::format("FPS %d", fps).c_str(), fontX, camHeight - (row++ * padding), jr);
        
        float rttMS = NG_CLIENT->getAvgRoundTripTime().getValue() * 1000.f;
        renderText(dSb, StringUtil::format("RTT %d ms", static_cast<int>(rttMS)).c_str(), fontX, camHeight - (row++ * padding), jr);
        
        const WeightedTimedMovingAverage& bpsIn = NG_CLIENT->getBytesReceivedPerSecond();
        int bpsInInt = static_cast<int>(bpsIn.getValue());
        renderText(dSb, StringUtil::format(" In %d Bps", bpsInInt).c_str(), fontX, camHeight - (row++ * padding), jr);
        
        const WeightedTimedMovingAverage& bpsOut = NG_CLIENT->getBytesSentPerSecond();
        int bpsOutInt = static_cast<int>(bpsOut.getValue());
        renderText(dSb, StringUtil::format("Out %d Bps", bpsOutInt).c_str(), fontX, camHeight - (row++ * padding), jr);
        
        dSb->endBatch(_textureShader, _fontTexture);
        
        if (!sSb->isStaticBatchRendered())
        {
            // Controls
            ++row;
            
            renderText(sSb, StringUtil::format("[N]         Sound %s", NG_AUDIO_ENGINE->areSoundsDisabled() ? " OFF" : "  ON").c_str(), fontX, camHeight - (row++ * padding), jr);
            renderText(sSb, StringUtil::format("[M]         Music %s", NG_AUDIO_ENGINE->isMusicDisabled() ? " OFF" : "  ON").c_str(), fontX, camHeight - (row++ * padding), jr);
            renderText(sSb, StringUtil::format("[B]   Box2D Debug %s", _engineState & GameEngineState_DisplayBox2D ? "  ON" : " OFF").c_str(), fontX, camHeight - (row++ * padding), jr);
            renderText(sSb, StringUtil::format("[I] Interpolation %s", _engineState & GameEngineState_Interpolation ? "  ON" : " OFF").c_str(), fontX, camHeight - (row++ * padding), jr);
            std::string lightZ = StringUtil::format("%f", GM_CFG->_playerLightZ);
            renderText(sSb, StringUtil::format("[L]  Lighting %s", _engineState & GameEngineState_Lighting ? lightZ.c_str() : "     OFF").c_str(), fontX, camHeight - (row++ * padding), jr);
            renderText(sSb, StringUtil::format("[U]    Display UI %s", _engineState & GameEngineState_DisplayUI ? "  ON" : " OFF").c_str(), fontX, camHeight - (row++ * padding), jr);
            
            Server* server = Server::getInstance();
            if (server && !(server->getFlags() & ServerFlag_TestSession))
            {
                renderText(sSb, StringUtil::format("[T]    Toggle Map %s", _engine->_world->getMapName().c_str()).c_str(), fontX, camHeight - (row++ * padding), jr);
            }
            else
            {
                renderText(sSb, StringUtil::format("              Map %s", _engine->_world->getMapName().c_str()).c_str(), fontX, camHeight - (row++ * padding), jr);
            }
            
            bool activePlayerIds[4] = {false};
            
            std::vector<Entity*> players = _engine->_world->getPlayers();
            for (Entity* entity : players)
            {
                PlayerController* robot = static_cast<PlayerController*>(entity->getController());
                
                int playerId = robot->getPlayerId();
                if (playerId >= 1 && playerId <= 4)
                {
                    renderText(sSb, StringUtil::format("%i|%s", playerId, robot->getPlayerName().c_str()).c_str(), 0.5f, camHeight - (playerId * 1.0f), jl);
                    activePlayerIds[playerId - 1] = true;
                }
            }
            
            for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
            {
                if (!activePlayerIds[i])
                {
                    renderText(sSb, StringUtil::format("%i|%s", (i + 1), "Connect a controller to join...").c_str(), 0.5f, camHeight - ((i + 1) * 1.0f), jl);
                }
            }
        }
    }
    else
    {
        renderText(sSb, StringUtil::format("%s, [ESC] to exit", "Joining Server...").c_str(), 0.5f, camHeight - 4, jl);
    }
    
    sSb->useStaticConfig();
    sSb->endBatch(_textureShader, _fontTexture);
}

void GameRenderer::renderText(SpriteBatcher* sb, const char* inStr, float x, float y, int justification)
{
    float fgWidth = FW_CFG->_camWidth / 64;
    float fgHeight = fgWidth * 1.171875f;
    
    std::string text(inStr);
    
    _font->renderText(*sb, text, x, y, fgWidth, fgHeight, justification);
}

void GameRenderer::endFrame()
{
    assert(_fbIndex >= 0);
    
    _rendererHelper->bindScreenFramebuffer();
    _rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
    _rendererHelper->useScreenBlending();

    _framebufferToScreenShader->bind(_rendererHelper->getOffscreenFramebuffer(_fbIndex)->texture);
    _rendererHelper->bindScreenVertexBuffer();
    _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
    _framebufferToScreenShader->unbind();
    
    _rendererHelper->disableBlending();
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
