//
//  StudioRenderer.cpp
//  dante
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/graphics/StudioRenderer.h"

#include "framework/graphics/portable/TextureManager.h"
#include "framework/graphics/portable/Font.h"
#include "game/logic/World.h"
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
#include <game/logic/StudioEngine.h>
#include <game/logic/StudioInputManager.h>

#include "framework/file/portable/Assets.h"
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
#include "game/logic/TitleInputManager.h"
#include "framework/network/server/NetworkManagerServer.h"
#include "framework/network/portable/MachineAddress.h"
#include "framework/util/MathUtil.h"
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
#include "framework/file/portable/Assets.h"
#include "framework/input/CursorInputManager.h"
#include "framework/input/CursorConverter.h"
#include <framework/entity/EntityLayoutMapper.h>
#include <framework/entity/EntityMapper.h>
#include <framework/util/Config.h>

#ifdef NG_STEAM
#include "framework/network/steam/NGSteamGameServer.h"
#include "framework/network/steam/NGSteamGameServices.h"
#endif

#include <sstream>
#include <ctime> // rand
#include <string>
#include <assert.h>
#include <cfloat>

StudioRenderer::StudioRenderer() : Renderer(),
_textureManager(new TextureManager()),
_rendererHelper(RENDERER_HELPER_FACTORY->createRendererHelper()),
_fontSpriteBatcher(new SpriteBatcher(_rendererHelper)),
_fbSpriteBatcher(new SpriteBatcher(_rendererHelper)),
_fillPolygonBatcher(new PolygonBatcher(_rendererHelper, true)),
_boundsPolygonBatcher(new PolygonBatcher(_rendererHelper, false)),
_lineBatcher(new LineBatcher(_rendererHelper)),
_circleBatcher(new CircleBatcher(_rendererHelper)),
_box2DDebugRenderer(new Box2DDebugRenderer(*_fillPolygonBatcher, *_boundsPolygonBatcher, *_lineBatcher, *_circleBatcher)),
_shaderProgramLoader(SHADER_PROGRAM_LOADER_FACTORY->createNGShaderLoader()),
_textureNGShader(new NGTextureShader(*_rendererHelper, "shader_003_vert.ngs", "shader_003_frag.ngs")),
_colorNGShader(new NGGeometryShader(*_rendererHelper, "shader_001_vert.ngs", "shader_001_frag.ngs")),
_framebufferToScreenNGShader(new NGFramebufferToScreenShader(*_rendererHelper, "shader_002_vert.ngs", "shader_002_frag.ngs")),
_font(new Font(0, 0, 16, 64, 75, 1024, 1024)),
_toastStateTime(0),
_fbIndex(0),
_scrollValue(1),
_engine(NULL),
_input(NULL),
_engineState(0),
_parallaxLayer0FactorX(0),
_parallaxLayer0FactorY(0),
_parallaxLayer1FactorX(0),
_parallaxLayer1FactorY(0),
_parallaxLayer2FactorX(0),
_parallaxLayer2FactorY(0)
{
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        _spriteBatchers[i] = new SpriteBatcher(_rendererHelper);
    }
    
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        _camBounds[i] = new NGRect(0, 0, CAM_WIDTH, CAM_HEIGHT);
    }
    
    _screenVertices.reserve(4);
    _screenVertices.push_back(VERTEX_2D(-1, -1));
    _screenVertices.push_back(VERTEX_2D(-1, 1));
    _screenVertices.push_back(VERTEX_2D(1, 1));
    _screenVertices.push_back(VERTEX_2D(1, -1));
}

StudioRenderer::~StudioRenderer()
{
    releaseDeviceDependentResources();

    delete _textureManager;
    delete _rendererHelper;
    delete _fontSpriteBatcher;
    delete _fbSpriteBatcher;
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        delete _spriteBatchers[i];
    }
    delete _fillPolygonBatcher;
    delete _boundsPolygonBatcher;
    delete _lineBatcher;
    delete _circleBatcher;
    delete _shaderProgramLoader;
    delete _font;
    delete _textureNGShader;
    delete _colorNGShader;
    delete _framebufferToScreenNGShader;
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        delete _camBounds[i];
    }
}

void StudioRenderer::createDeviceDependentResources()
{
    _rendererHelper->createDeviceDependentResources();
    _textureManager->createDeviceDependentResources();
    
    _parallaxLayer0FactorX = NG_CFG->getDouble("ParallaxLayer0FactorX");
    _parallaxLayer0FactorY = NG_CFG->getDouble("ParallaxLayer0FactorY");
    _parallaxLayer1FactorX = NG_CFG->getDouble("ParallaxLayer1FactorX");
    _parallaxLayer1FactorY = NG_CFG->getDouble("ParallaxLayer1FactorY");
    _parallaxLayer2FactorX = NG_CFG->getDouble("ParallaxLayer2FactorX");
    _parallaxLayer2FactorY = NG_CFG->getDouble("ParallaxLayer2FactorY");
    
    _fontTexture = _textureManager->getTextureWithName("texture_000.ngt");

    _textureNGShader->load(*_shaderProgramLoader);
    _colorNGShader->load(*_shaderProgramLoader);
    _framebufferToScreenNGShader->load(*_shaderProgramLoader);
}

void StudioRenderer::createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight)
{
    _rendererHelper->createWindowSizeDependentResources(screenWidth, screenHeight, renderWidth, renderHeight);
}

void StudioRenderer::releaseDeviceDependentResources()
{
    _rendererHelper->releaseDeviceDependentResources();
    _textureManager->releaseDeviceDependentResources();

    _textureNGShader->unload(*_shaderProgramLoader);
    _colorNGShader->unload(*_shaderProgramLoader);
    _framebufferToScreenNGShader->unload(*_shaderProgramLoader);
}

void StudioRenderer::render()
{
    _engineState = _engine->_state;
    
    setFramebuffer(0, 0, 0, 0, 1);
    _rendererHelper->useNormalBlending();

    if (_textureManager->ensureTextures())
    {
        renderWorld();
        
        Entity* e = _input->_activeEntity;
        if (!e)
        {
            e = _input->_lastActiveEntity;
        }
        if (e)
        {
            _rendererHelper->updateMatrix(_camBounds[3]->getLeft(), _camBounds[3]->getRight(), _camBounds[3]->getBottom(), _camBounds[3]->getTop());
            _rendererHelper->useNormalBlending();
            _spriteBatchers[0]->beginBatch();
            TextureRegion tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
            _spriteBatchers[0]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
            Color c = _input->_isDraggingActiveEntityOverDeleteZone ? Color::HALF : Color::DOUBLE;
            _spriteBatchers[0]->endBatch(_textureNGShader, _textureManager->getTextureWithName(tr.getTextureName()), NULL, c);
        }
        
        if (_engineState & StudioEngineState_DisplayBox2D)
        {
            renderBox2D();
        }
        
        if (_engineState & StudioEngineState_DisplayGrid)
        {
            renderGrid();
        }
        
        renderUI();
    }

    endFrame();
}

void StudioRenderer::setEngine(StudioEngine* inValue)
{
    _engine = inValue;
}

void StudioRenderer::setInputManager(StudioInputManager* inValue)
{
    _input = inValue;
}

void StudioRenderer::update(float x, float y, float w, float h, int scale)
{
    _scrollValue = scale;
    
    _camBounds[3]->getLowerLeft().set(x, y);
    _camBounds[2]->getLowerLeft().set(x * _parallaxLayer2FactorX, y * _parallaxLayer2FactorY);
    _camBounds[1]->getLowerLeft().set(x * _parallaxLayer1FactorX, y * _parallaxLayer1FactorY);
    _camBounds[0]->getLowerLeft().set(x * _parallaxLayer0FactorX, y * _parallaxLayer0FactorY);
    
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        _camBounds[i]->setWidth(w);
        _camBounds[i]->setHeight(h);
    }
    
    if (_toasts.size() > 0)
    {
        ++_toastStateTime;
        if (_toastStateTime >= 90)
        {
            _toasts.pop_front();
            _toastStateTime = 0;
        }
    }
}

void StudioRenderer::displayToast(std::string toast)
{
    _toasts.push_back(toast);
    _toastStateTime = 0;
}

void StudioRenderer::setFramebuffer(int framebufferIndex, float r, float g, float b, float a)
{
    assert(framebufferIndex >= 0);

    _fbIndex = framebufferIndex;

    _rendererHelper->bindToOffscreenFramebuffer(_fbIndex);
    _rendererHelper->clearFramebufferWithColor(r, g, b, a);
}

void StudioRenderer::renderWorld()
{
    World* world = _engine->_world;
    
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        _spriteBatchers[i]->beginBatch();
    }
    
    if (_engineState & StudioEngineState_DisplayTypes)
    {
        _fontSpriteBatcher->beginBatch();
    }
    
    std::string textures[NUM_SPRITE_BATCHERS];
    
    {
        std::vector<Entity*> entities = world->getLayers();
        for (Entity* e : entities)
        {
            TextureRegion tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
            
            _spriteBatchers[e->getEntityDef().layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
            textures[e->getEntityDef().layer] = tr.getTextureName();
            
            if ((_engineState & StudioEngineState_DisplayTypes) &&
                _engineState & (1 << (e->getEntityDef().layer + StudioEngineState_LayerBitBegin)))
            {
                renderText(e->getEntityDef().typeName.c_str(), e->getPosition().x, e->getPosition().y, FONT_ALIGN_CENTER);
            }
        }
    }
    
    {
        std::vector<Entity*> entities = world->getPlayers();
        for (Entity* e : entities)
        {
            TextureRegion tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
            
            _spriteBatchers[e->getEntityDef().layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
            textures[e->getEntityDef().layer] = tr.getTextureName();
            
            if ((_engineState & StudioEngineState_DisplayTypes) &&
                _engineState & (1 << (e->getEntityDef().layer + StudioEngineState_LayerBitBegin)))
            {
                renderText(e->getEntityDef().typeName.c_str(), e->getPosition().x, e->getPosition().y, FONT_ALIGN_CENTER);
            }
        }
    }
    
    {
        std::vector<Entity*> entities = world->getDynamicEntities();
        for (Entity* e : entities)
        {
            TextureRegion tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
            
            _spriteBatchers[e->getEntityDef().layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
            textures[e->getEntityDef().layer] = tr.getTextureName();
            
            if ((_engineState & StudioEngineState_DisplayTypes) &&
                _engineState & (1 << (e->getEntityDef().layer + StudioEngineState_LayerBitBegin)))
            {
                renderText(e->getEntityDef().typeName.c_str(), e->getPosition().x, e->getPosition().y, FONT_ALIGN_CENTER);
            }
        }
    }
    
    {
        std::vector<Entity*> entities = world->getStaticEntities();
        for (Entity* e : entities)
        {
            TextureRegion tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
            
            _spriteBatchers[e->getEntityDef().layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
            textures[e->getEntityDef().layer] = tr.getTextureName();
            
            if ((_engineState & StudioEngineState_DisplayTypes) &&
                _engineState & (1 << (e->getEntityDef().layer + StudioEngineState_LayerBitBegin)))
            {
                renderText(e->getEntityDef().typeName.c_str(), e->getPosition().x, e->getPosition().y, FONT_ALIGN_CENTER);
            }
        }
    }
    
    _rendererHelper->useNormalBlending();
    
    setFramebuffer(0);
    for (int i = 0; i < 5; ++i)
    {
        endBatchWithTexture(_spriteBatchers[i], _textureManager->getTextureWithName(textures[i]), i);
    }
    
    setFramebuffer(1);
    endBatchWithTexture(_spriteBatchers[5], _textureManager->getTextureWithName(textures[5]), 5);
    
    setFramebuffer(2);
    for (int i = 6; i < 9; ++i)
    {
        endBatchWithTexture(_spriteBatchers[i], _textureManager->getTextureWithName(textures[i]), i);
    }
    
    int fbBegin = 0;
    int fbEnd = 3;
    
    _rendererHelper->useScreenBlending();
    setFramebuffer(9);
    for (int i = fbBegin; i < fbEnd; ++i)
    {
        _framebufferToScreenNGShader->bind(&_screenVertices, _rendererHelper->getFramebuffer(i));
        _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
        _framebufferToScreenNGShader->unbind();
    }
    
    if (_engineState & StudioEngineState_DisplayTypes)
    {
        _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture);
    }
}

void StudioRenderer::endBatchWithTexture(SpriteBatcher* sb, NGTexture* tex, int layer)
{
    if (tex && _engineState & (1 << (layer + StudioEngineState_LayerBitBegin)))
    {
        int c = clamp(layer, 3, 0);
        if (!(_engineState & StudioEngineState_DisplayParallax))
        {
            c = 3;
        }
        
        _rendererHelper->updateMatrix(_camBounds[c]->getLeft(), _camBounds[c]->getRight(), _camBounds[c]->getBottom(), _camBounds[c]->getTop());
        
        sb->endBatch(_textureNGShader, tex);
    }
}

void StudioRenderer::renderBox2D()
{
    _rendererHelper->useScreenBlending();
    _rendererHelper->updateMatrix(_camBounds[3]->getLeft(), _camBounds[3]->getRight(), _camBounds[3]->getBottom(), _camBounds[3]->getTop());
    _box2DDebugRenderer->render(&_engine->_world->getWorld(), _colorNGShader);
}

void StudioRenderer::renderGrid()
{
    int left = clamp(_camBounds[3]->getLeft(), FLT_MAX, 0);
    int bottom = clamp(_camBounds[3]->getBottom(), FLT_MAX, 0);
    int camWidth = _camBounds[3]->getRight() + 1;
    int camHeight = _camBounds[3]->getTop() + 1;
    
    Color lineColor = Color::WHITE;
    lineColor.alpha = 0.25f;
    
    _rendererHelper->useNormalBlending();
    _rendererHelper->updateMatrix(_camBounds[3]->getLeft(), _camBounds[3]->getRight(), _camBounds[3]->getBottom(), _camBounds[3]->getTop());
    
    _lineBatcher->beginBatch();
    for (int i = left; i <= camWidth; ++i)
    {
        _lineBatcher->renderLine(i, 0, i, camHeight);
    }
    for (int i = bottom; i <= camHeight; ++i)
    {
        _lineBatcher->renderLine(0, i, camWidth, i);
    }
    _lineBatcher->endBatch(_colorNGShader, lineColor);
    
    _lineBatcher->beginBatch();
    _lineBatcher->renderLine(0, 0, 0, camHeight);
    _lineBatcher->renderLine(0, 0, camWidth, 0);
    _lineBatcher->endBatch(_colorNGShader, Color::RED);
}

void StudioRenderer::renderUI()
{
    _rendererHelper->useScreenBlending();
    _rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
    
    if (_engineState & StudioEngineState_DisplayLoadMapDialog)
    {
        /// Maps
        std::vector<MapDef>& maps = EntityLayoutMapper::getInstance()->getMaps();
        int numMaps = static_cast<int>(maps.size());
        
        _fillPolygonBatcher->beginBatch();
        int width = CAM_WIDTH / 3;
        int height = numMaps + 3;
        NGRect window = NGRect(CAM_WIDTH / 2 - width / 2, CAM_HEIGHT - 4 - height - 1, width, height);
        Color windowColor = Color::BLUE;
        windowColor.alpha = 0.5f;
        _fillPolygonBatcher->renderRect(window);
        _fillPolygonBatcher->endBatch(_colorNGShader, windowColor);
        
        int row = 2;
        static float padding = 1;
        
        _fontSpriteBatcher->beginBatch();
        renderText("Load Map", CAM_WIDTH / 2, CAM_HEIGHT - 4 - (row++ * padding), FONT_ALIGN_CENTER);
        _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture);
        
        ++row;
        
        for (int i = 0; i < numMaps; ++i)
        {
            MapDef& mp = maps[i];
            
            _fontSpriteBatcher->beginBatch();
            renderText(StringUtil::format("%s | %s", mp.name.c_str(), mp.value.c_str()).c_str(), CAM_WIDTH / 2, CAM_HEIGHT - 4 - (row++ * padding), FONT_ALIGN_CENTER);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, i == _input->_selectionIndex ? Color::WHITE : Color::BLACK);
        }
    }
    else if (_engineState & StudioEngineState_DisplayEntities)
    {
        const std::vector<EntityDef*>& entityDescriptors = EntityMapper::getInstance()->getEntityDescriptors();
        int numEntities = static_cast<int>(entityDescriptors.size());
        
        int width = CAM_WIDTH * 0.7f;
        int height = numEntities + 3;
        
        _rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
        
        _fillPolygonBatcher->beginBatch();
        
        NGRect window = NGRect(1, CAM_HEIGHT - height - 1, width, height);
        Color windowColor = Color::BLUE;
        windowColor.alpha = 0.5f;
        _fillPolygonBatcher->renderRect(window);
        _fillPolygonBatcher->endBatch(_colorNGShader, windowColor);
        
        int selectionIndex = _input->_selectionIndex;
        int row = 1;
        static float padding = 5;
        
        _fontSpriteBatcher->beginBatch();
        renderText("Entities", width / 2, CAM_HEIGHT - 1 - (row * padding), FONT_ALIGN_CENTER);
        _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture);
        
        ++row;
        
        for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
        {
            _spriteBatchers[i]->beginBatch();
        }
        
        std::string textures[NUM_SPRITE_BATCHERS];
        
        for (int i = clamp(selectionIndex - 2, numEntities - 1, 0); i < numEntities; ++i)
        {
            EntityDef* ed = entityDescriptors[i];
            
            _fontSpriteBatcher->beginBatch();
            renderText(StringUtil::format("%s | %s", ed->typeName.c_str(), ed->name.c_str()).c_str(), 7, CAM_HEIGHT - 1 - (row * padding), FONT_ALIGN_LEFT);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, i == selectionIndex ? Color::WHITE : Color::BLACK);
            
            TextureRegion tr = ASSETS->findTextureRegion(ed->textureMappings[0], 0);
            _spriteBatchers[ed->layer]->renderSprite(4, CAM_HEIGHT - 1 - (row * padding), 4, 4, 0, tr);
            textures[ed->layer] = tr.getTextureName();
            
            ++row;
        }
        
        for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
        {
            if (textures[i].length() > 0)
            {
                _spriteBatchers[i]->endBatch(_textureNGShader, _textureManager->getTextureWithName(textures[i]));
            }
        }
    }
    else if (_engineState & StudioEngineState_DisplayControls)
    {
        /// Controls
        _fillPolygonBatcher->beginBatch();
        int width = 22;
        int height = 18;
        NGRect window = NGRect(CAM_WIDTH - width - 1, CAM_HEIGHT - height - 1, width, height);
        Color windowColor = Color::BLUE;
        windowColor.alpha = 0.5f;
        _fillPolygonBatcher->renderRect(window);
        _fillPolygonBatcher->endBatch(_colorNGShader, windowColor);
        
        _fontSpriteBatcher->beginBatch();
        
        int row = 2;
        static float padding = 1;
        
        renderText(StringUtil::format("[B]   Box2D Debug %s", _engineState & StudioEngineState_DisplayBox2D ? " ON" : "OFF").c_str(), CAM_WIDTH - 2, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[G]          Grid %s", _engineState & StudioEngineState_DisplayGrid ? " ON" : "OFF").c_str(), CAM_WIDTH - 2, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[D]   Debug Types %s", _engineState & StudioEngineState_DisplayTypes ? " ON" : "OFF").c_str(), CAM_WIDTH - 2, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[P]      Parallax %s", _engineState & StudioEngineState_DisplayParallax ? " ON" : "OFF").c_str(), CAM_WIDTH - 2, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        ++row;
        renderText(StringUtil::format("[R]      Reset Camera").c_str(), CAM_WIDTH - 2, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[X]       Reset World").c_str(), CAM_WIDTH - 2, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[T]         Test Zone").c_str(), CAM_WIDTH - 2, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        ++row;
        renderText(StringUtil::format("[N]               New").c_str(), CAM_WIDTH - 2, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[L]              Load").c_str(), CAM_WIDTH - 2, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[S]              Save").c_str(), CAM_WIDTH - 2, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[CTRL+S]      Save As").c_str(), CAM_WIDTH - 2, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        ++row;
        renderText(StringUtil::format("[C]  %s Controls", _engineState & StudioEngineState_DisplayControls ? "Hide   " : "Display").c_str(), CAM_WIDTH - 2, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[A]  %s   Assets", _engineState & StudioEngineState_DisplayAssets ? "Hide   " : "Display").c_str(), CAM_WIDTH - 2, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[E]  %s Entities", _engineState & StudioEngineState_DisplayEntities ? "Hide   " : "Display").c_str(), CAM_WIDTH - 2, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_RIGHT);
        
        _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture);
    }
    
    {
        /// Bottom Bar
        _fillPolygonBatcher->beginBatch();
        NGRect bar = NGRect(0, 0, CAM_WIDTH, 2);
        Color barColor = Color(0.33f, 0.33f, 0.33f, 0.85f);
        _fillPolygonBatcher->renderRect(bar);
        _fillPolygonBatcher->endBatch(_colorNGShader, barColor);
        
        int column = 1;
        static float padding = 1;
        
        for (int i = 0; i < StudioEngineState_NumLayers; ++i)
        {
            _fontSpriteBatcher->beginBatch();
            renderText(StringUtil::format("%d", i).c_str(), 1 + (column++ * padding), 1.5f, FONT_ALIGN_RIGHT);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, _engineState & (1 << (i + StudioEngineState_LayerBitBegin)) ? Color::WHITE : Color::BLACK);
        }
        
        _fontSpriteBatcher->beginBatch();
        renderText("Layers", 1 + column / 2.0f, 0.5f, FONT_ALIGN_CENTER);
        _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, Color::WHITE);
        
        {
            /// Render Map Info in the center of the bar
            _fontSpriteBatcher->beginBatch();
            renderText(StringUtil::format("%s | %s", _engine->_world->getMapName().c_str(), _engine->_world->getMapFileName().c_str()).c_str(), CAM_WIDTH / 2, 1.5f, FONT_ALIGN_CENTER);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, Color::WHITE);
            
            _fontSpriteBatcher->beginBatch();
            renderText("Map", CAM_WIDTH / 2, 0.5f, FONT_ALIGN_CENTER);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, Color::WHITE);
        }
        
        column = 56;
        {
            _fontSpriteBatcher->beginBatch();
            renderText("C", 1 + (column++ * padding), 1.5f, FONT_ALIGN_LEFT);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, _engineState & StudioEngineState_DisplayControls ? Color::WHITE : Color::BLACK);
            
            _fontSpriteBatcher->beginBatch();
            renderText("A", 1 + (column++ * padding), 1.5f, FONT_ALIGN_LEFT);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, _engineState & StudioEngineState_DisplayAssets ? Color::WHITE : Color::BLACK);
            
            _fontSpriteBatcher->beginBatch();
            renderText("E", 1 + (column++ * padding), 1.5f, FONT_ALIGN_LEFT);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, _engineState & StudioEngineState_DisplayEntities ? Color::WHITE : Color::BLACK);
            
            _fontSpriteBatcher->beginBatch();
            renderText("Windows", 58, 0.5f, FONT_ALIGN_CENTER);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, Color::WHITE);
        }
    }
    
    Entity* e = _input->_activeEntity;
    if (e)
    {
        /// Render Delete Zone
        _fillPolygonBatcher->beginBatch();
        int width = CAM_WIDTH / 3;
        int height = 3;
        static NGRect deleteWindow = NGRect(CAM_WIDTH / 2 - width / 2, CAM_HEIGHT - height - 1, width, height);
        Color windowColor = Color::RED;
        windowColor.alpha = 0.5f;
        _fillPolygonBatcher->renderRect(deleteWindow);
        _fillPolygonBatcher->endBatch(_colorNGShader, windowColor);
        
        int row = 3;
        static float padding = 1;
        
        _fontSpriteBatcher->beginBatch();
        renderText("DELETE", CAM_WIDTH / 2, CAM_HEIGHT - (row++ * padding), FONT_ALIGN_CENTER);
        _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture);
    }
    
    {
        /// Toasts
        int y = CAM_HEIGHT - 2;
        _fillPolygonBatcher->beginBatch();
        _fontSpriteBatcher->beginBatch();
        for (std::string t : _toasts)
        {
            NGRect window = NGRect(CAM_WIDTH / 2 - t.length() / 2.0f - 1, y - 1, t.length() + 1, 2);
            _fillPolygonBatcher->renderRect(window);
            renderText(t.c_str(), CAM_WIDTH / 2, y, FONT_ALIGN_CENTER);
            y -= 2;
        }
        Color windowColor = Color::BLUE;
        windowColor.alpha = 0.5f;
        _fillPolygonBatcher->endBatch(_colorNGShader, windowColor);
        _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture);
    }
}

void StudioRenderer::renderText(const char* inStr, float x, float y, int justification)
{
    float fgWidth = CAM_WIDTH / 64;
    float fgHeight = fgWidth * (75.0f / 64.0f);

    std::string text(inStr);

    _font->renderText(*_fontSpriteBatcher, text, x, y, fgWidth, fgHeight, justification);
}

void StudioRenderer::endFrame()
{
    assert(_fbIndex >= 0);

    _rendererHelper->bindToScreenFramebuffer();
    _rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
    _rendererHelper->useScreenBlending();

    static std::vector<VERTEX_2D> screenVertices;
    screenVertices.clear();
    screenVertices.push_back(VERTEX_2D(-1, -1));
    screenVertices.push_back(VERTEX_2D(-1, 1));
    screenVertices.push_back(VERTEX_2D(1, 1));
    screenVertices.push_back(VERTEX_2D(1, -1));

    _framebufferToScreenNGShader->bind(&screenVertices, _rendererHelper->getFramebuffer(_fbIndex));
    _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
    _framebufferToScreenNGShader->unbind();

    _rendererHelper->useNoBlending();
}
