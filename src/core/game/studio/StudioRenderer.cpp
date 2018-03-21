//
//  StudioRenderer.cpp
//  dante
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/studio/StudioRenderer.h>

#include <framework/graphics/portable/TextureManager.h>
#include <framework/graphics/portable/Font.h>
#include <game/logic/World.h>
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
#include <game/studio/StudioEngine.h>
#include <game/studio/StudioInputManager.h>
#include <framework/graphics/portable/FramebufferWrapper.h>

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
#include <game/title/TitleInputManager.h>
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
#include <framework/input/CursorInputManager.h>
#include <framework/input/CursorConverter.h>
#include <framework/entity/EntityLayoutMapper.h>
#include <framework/entity/EntityMapper.h>
#include <game/logic/GameConfig.h>
#include <framework/util/Config.h>
#include <framework/util/StringUtil.h>

#ifdef NG_STEAM
#include <framework/network/steam/NGSteamGameServer.h>
#include <framework/network/steam/NGSteamGameServices.h>
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
_activeEntitySpriteBatcher(new SpriteBatcher(_rendererHelper)),
_fillPolygonBatcher(new PolygonBatcher(_rendererHelper, true)),
_boundsPolygonBatcher(new PolygonBatcher(_rendererHelper, false)),
_lineBatcher(new LineBatcher(_rendererHelper)),
_circleBatcher(new CircleBatcher(_rendererHelper)),
_box2DDebugRenderer(new Box2DDebugRenderer(*_fillPolygonBatcher, *_boundsPolygonBatcher, *_lineBatcher, *_circleBatcher)),
_shaderProgramLoader(SHADER_PROGRAM_LOADER_FACTORY->createShaderLoader()),
_textureNGShader(new NGTextureShader(*_rendererHelper)),
_colorNGShader(new NGGeometryShader(*_rendererHelper)),
_framebufferToScreenNGShader(new NGFramebufferToScreenShader(*_rendererHelper)),
_font(new Font(0, 0, 16, 64, 75, 1024, 1024)),
_toastStateTime(0),
_fbIndex(0),
_scrollValue(1),
_engine(NULL),
_input(NULL),
_engineState(0)
{
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        _spriteBatchers[i] = new SpriteBatcher(_rendererHelper);
    }
    
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        _camBounds[i] = new NGRect(0, 0, GM_CFG->_camWidth, GM_CFG->_camHeight);
    }
}

StudioRenderer::~StudioRenderer()
{
	delete _textureManager;
    delete _rendererHelper;
    delete _fontSpriteBatcher;
    delete _fbSpriteBatcher;
    delete _activeEntitySpriteBatcher;
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
    
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        _camBounds[i]->setWidth(GM_CFG->_camWidth);
        _camBounds[i]->setHeight(GM_CFG->_camHeight);
    }
    
    _fontTexture = _textureManager->getTextureWithName("texture_000.ngt");

    _textureNGShader->load(*_shaderProgramLoader);
    _colorNGShader->load(*_shaderProgramLoader);
    _framebufferToScreenNGShader->load(*_shaderProgramLoader);
}

void StudioRenderer::createWindowSizeDependentResources(int screenWidth, int screenHeight)
{
    _rendererHelper->createWindowSizeDependentResources(screenWidth, screenHeight);
    
    createFramebuffers();
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
    
    bindOffscreenFramebuffer(0, 0, 0, 0, 1);
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
            _activeEntitySpriteBatcher->beginBatch(10);
            TextureRegion& tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
            _activeEntitySpriteBatcher->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
            _activeEntitySpriteBatcher->endBatch(_textureNGShader, _textureManager->getTextureWithName(tr.getTextureName()), NULL, Color::DOUBLE);
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

void StudioRenderer::onMapLoaded()
{
    _rendererHelper->releaseFramebuffers();
    
    createFramebuffers();
}

void StudioRenderer::onWaterAdded(Entity* e)
{
    _rendererHelper->useScreenBlending();
    
    int right = e->getWidth();
    int top = e->getHeight();
    _rendererHelper->updateMatrix(0, right, 0, top);
    
    TextureRegion& trWaterSurface = ASSETS->findTextureRegion(e->getTextureMapping(1));
    TextureRegion& trWaterBody = ASSETS->findTextureRegion(e->getTextureMapping(2));
    int tileHeight = trWaterSurface._regionHeight / GRID_TO_PIXEL;
    int tileWidth = trWaterSurface._regionWidth / GRID_TO_PIXEL;
    
    _fbSpriteBatcher->beginBatch(0);
    for (int j = 0; j < top; j += tileHeight)
    {
        for (int i = 0; i < right; i += tileWidth)
        {
            float x = i + tileWidth / 2.0f;
            float y = j + tileHeight / 2.0f;
            
            if (j == (top - tileHeight))
            {
                _fbSpriteBatcher->renderSprite(x, y, tileWidth, tileHeight, 0, trWaterSurface);
            }
            else
            {
                _fbSpriteBatcher->renderSprite(x, y, tileWidth, tileHeight, 0, trWaterBody);
            }
        }
    }
    
    int renderWidth = e->getWidth() * GRID_TO_PIXEL;
    int renderHeight = e->getHeight() * GRID_TO_PIXEL;
    
    std::string textureName = StringUtil::toString(e->getID());
    std::string normalMapName = std::string("n_") + textureName;
    
    std::string textureNameTemp = std::string("temp_") + textureName;
    std::string normalMapNameTemp = std::string("temp_") + normalMapName;
    
    e->getEntityDef().textureMappings.find(0)->second = textureName;
    TextureRegion* tr = new TextureRegion(textureName, 0, 0, renderWidth, renderHeight, renderWidth, renderHeight, trWaterSurface._layer);
    ASSETS->registerTextureRegion(textureName, tr);
    
    {
        FramebufferWrapper* fbTemp = _rendererHelper->addFramebuffer(renderWidth, renderHeight, textureNameTemp);
        bindFramebuffer(fbTemp);
        _fbSpriteBatcher->endBatch(_textureNGShader, _textureManager->getTextureWithName(trWaterSurface.getTextureName()));
        
        FramebufferWrapper* fb = _rendererHelper->addFramebuffer(renderWidth, renderHeight, textureName);
        bindFramebuffer(fb);
        
        _fbSpriteBatcher->beginBatch(0);
        _fbSpriteBatcher->renderSprite(e->getWidth() / 2, e->getHeight() / 2, e->getWidth(), e->getHeight(), 0, *tr);
        _fbSpriteBatcher->endBatch(_textureNGShader, fbTemp->texture);
        
        _rendererHelper->removeFramebuffer(textureNameTemp);
        _textureManager->registerFramebuffer(textureName, fb);
    }
    
    {
        FramebufferWrapper* fbTemp = _rendererHelper->addFramebuffer(renderWidth, renderHeight, normalMapNameTemp);
        bindFramebuffer(fbTemp);
        _fbSpriteBatcher->endBatch(_textureNGShader, _textureManager->getTextureWithName(trWaterSurface.getNormalMapName()));
        
        FramebufferWrapper* fb = _rendererHelper->addFramebuffer(renderWidth, renderHeight, normalMapName);
        bindFramebuffer(fb);
        
        _fbSpriteBatcher->beginBatch(0);
        _fbSpriteBatcher->renderSprite(e->getWidth() / 2, e->getHeight() / 2, e->getWidth(), e->getHeight(), 0, *tr);
        _fbSpriteBatcher->endBatch(_textureNGShader, fbTemp->texture);
        
        _rendererHelper->removeFramebuffer(normalMapNameTemp);
        _textureManager->registerFramebuffer(normalMapName, fb);
    }
}

void StudioRenderer::onWaterRemoved(Entity* e)
{
    std::string textureName = StringUtil::toString(e->getID());
    std::string normalMapName = textureName;
    normalMapName.insert(0, "n_");
    
    _rendererHelper->removeFramebuffer(textureName);
    _rendererHelper->removeFramebuffer(normalMapName);
}

void StudioRenderer::update(float x, float y, float w, float h, int scale)
{
    _scrollValue = scale;
    
    _camBounds[3]->getLowerLeft().set(x, y);
    _camBounds[2]->getLowerLeft().set(x * GM_CFG->_parallaxLayer2FactorX, y * GM_CFG->_parallaxLayer2FactorY);
    _camBounds[1]->getLowerLeft().set(x * GM_CFG->_parallaxLayer1FactorX, y * GM_CFG->_parallaxLayer1FactorY);
    _camBounds[0]->getLowerLeft().set(x * GM_CFG->_parallaxLayer0FactorX, y * GM_CFG->_parallaxLayer0FactorY);
    
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

void StudioRenderer::createFramebuffers()
{
    World* world = _engine->_world;
    for (Entity* e : world->getWaterBodies())
    {
        onWaterAdded(e);
    }
}

void StudioRenderer::bindOffscreenFramebuffer(int fbIndex, float r, float g, float b, float a)
{
    assert(fbIndex >= 0);
    
    _fbIndex = fbIndex;
    
    FramebufferWrapper* fb = _rendererHelper->getOffscreenFramebuffer(_fbIndex);
    _rendererHelper->bindFramebuffer(fb);
    _rendererHelper->clearFramebufferWithColor(r, g, b, a);
}

void StudioRenderer::bindFramebuffer(FramebufferWrapper* fb, float r, float g, float b, float a)
{
    _rendererHelper->bindFramebuffer(fb);
    _rendererHelper->clearFramebufferWithColor(r, g, b, a);
}

void StudioRenderer::renderWorld()
{
    World* world = _engine->_world;
    
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        _spriteBatchers[i]->beginBatch(i);
    }
    
    if (_engineState & StudioEngineState_DisplayTypes)
    {
        _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
    }
    
    std::fill(_textures, _textures + NUM_SPRITE_BATCHERS, "");
    
    renderEntities(world->getLayers());
    renderEntities(world->getStaticEntities());
    renderEntities(world->getPlayers());
    renderEntities(world->getDynamicEntities());
    
    _rendererHelper->useNormalBlending();
    bindOffscreenFramebuffer(0);
    for (int i = 0; i < 5; ++i)
    {
        endBatchWithTexture(_spriteBatchers[i], _textureManager->getTextureWithName(_textures[i]), i);
    }
    
    _rendererHelper->useScreenBlending();
    bindOffscreenFramebuffer(1);
    endBatchWithTexture(_spriteBatchers[5], _textureManager->getTextureWithName(_textures[5]), 5);
    
    _rendererHelper->useNormalBlending();
    bindOffscreenFramebuffer(2);
    for (int i = 6; i < 8; ++i)
    {
        endBatchWithTexture(_spriteBatchers[i], _textureManager->getTextureWithName(_textures[i]), i);
    }
    
    // Should be Layer 7, but doesn't have to be I suppose...
    renderWater(world->getWaterBodies());
    
    // Static Layer
    endBatchWithTexture(_spriteBatchers[8], _textureManager->getTextureWithName(_textures[8]), 8);
    
    int fbBegin = 0;
    int fbEnd = 3;
    
    _rendererHelper->useScreenBlending();
    bindOffscreenFramebuffer(9);
    for (int i = fbBegin; i < fbEnd; ++i)
    {
        _framebufferToScreenNGShader->bind(_rendererHelper->getOffscreenFramebuffer(i)->texture);
        _rendererHelper->bindScreenVertexBuffer();
        _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
        _framebufferToScreenNGShader->unbind();
    }
    
    if (_engineState & StudioEngineState_DisplayTypes)
    {
        _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture);
    }
}

void StudioRenderer::renderEntities(std::vector<Entity*>& entities)
{
    for (Entity* e : entities)
    {
        TextureRegion& tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
        
        _spriteBatchers[tr._layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
        _textures[tr._layer] = tr.getTextureName();
        
        if ((_engineState & StudioEngineState_DisplayTypes) &&
            _engineState & (1 << (tr._layer + StudioEngineState_LayerBitBegin)))
        {
            renderText(e->getEntityDef().typeName.c_str(), e->getPosition().x, e->getPosition().y, FONT_ALIGN_CENTER);
        }
    }
}

void StudioRenderer::renderWater(std::vector<Entity*>& entities)
{
    for (Entity* e : entities)
    {
        TextureRegion& tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
        
        int layer = tr._layer;
        _spriteBatchers[layer]->beginBatch(layer);
        _spriteBatchers[layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
        endBatchWithTexture(_spriteBatchers[layer], _textureManager->getTextureWithName(tr.getTextureName()), layer);
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
    float x = clamp(_camBounds[3]->getLeft(), FLT_MAX, 0);
    float y = clamp(_camBounds[3]->getBottom(), FLT_MAX, 0);
    float px = fmodf(x, GM_CFG->_camWidth);
    float py = fmodf(y, GM_CFG->_camHeight);
    float bx = x - px;
    float by = y - py;
    
    int left = x;
    int bottom = y;
    int leftAligned = bx;
    int bottomAligned = by;
    int camWidth = _camBounds[3]->getRight() + 1;
    int camHeight = _camBounds[3]->getTop() + 1;
    
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
    static Color lineColor = Color::WHITE;
    lineColor.alpha = 0.25f;
    _lineBatcher->endBatch(_colorNGShader, lineColor);
    
    _lineBatcher->beginBatch();
    for (int i = leftAligned; i <= camWidth; i += GM_CFG->_camWidth)
    {
        _lineBatcher->renderLine(i, 0, i, camHeight);
    }
    for (int i = bottomAligned; i <= camHeight; i += GM_CFG->_camHeight)
    {
        _lineBatcher->renderLine(0, i, camWidth, i);
    }
    _lineBatcher->endBatch(_colorNGShader, Color::GREEN);
    
    _lineBatcher->beginBatch();
    _lineBatcher->renderLine(0, 0, 0, camHeight);
    _lineBatcher->renderLine(0, 0, camWidth, 0);
    _lineBatcher->endBatch(_colorNGShader, Color::RED);
}

void StudioRenderer::renderUI()
{
    _rendererHelper->useScreenBlending();
    _rendererHelper->updateMatrix(0, GM_CFG->_camWidth, 0, GM_CFG->_camHeight);
    
    if (_engineState & StudioEngineState_DisplayLoadMapDialog)
    {
        /// Maps
        std::vector<MapDef>& maps = EntityLayoutMapper::getInstance()->getMaps();
        int numMaps = static_cast<int>(maps.size());
        
        _fillPolygonBatcher->beginBatch();
        int width = GM_CFG->_camWidth / 3;
        int height = numMaps + 3;
        NGRect window = NGRect(GM_CFG->_camWidth / 2 - width / 2, GM_CFG->_camHeight - 4 - height - 1, width, height);
        Color windowColor = Color::BLUE;
        windowColor.alpha = 0.5f;
        _fillPolygonBatcher->renderRect(window);
        _fillPolygonBatcher->endBatch(_colorNGShader, windowColor);
        
        int row = 2;
        static float padding = 1;
        
        _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
        renderText("Load Map", GM_CFG->_camWidth / 2, GM_CFG->_camHeight - 4 - (row++ * padding), FONT_ALIGN_CENTER);
        _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture);
        
        ++row;
        
        for (int i = 0; i < numMaps; ++i)
        {
            MapDef& mp = maps[i];
            
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText(StringUtil::format("%s | %s", mp.name.c_str(), mp.value.c_str()).c_str(), GM_CFG->_camWidth / 2, GM_CFG->_camHeight - 4 - (row++ * padding), FONT_ALIGN_CENTER);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, i == _input->_selectionIndex ? Color::WHITE : Color::BLACK);
        }
    }
    else if (_engineState & StudioEngineState_DisplayEntities)
    {
        const std::vector<EntityDef*>& entityDescriptors = EntityMapper::getInstance()->getEntityDescriptors();
        int numEntities = static_cast<int>(entityDescriptors.size());
        
        int width = GM_CFG->_camWidth * 0.7f;
        int height = numEntities + 3;
        
        _rendererHelper->updateMatrix(0, GM_CFG->_camWidth, 0, GM_CFG->_camHeight);
        
        _fillPolygonBatcher->beginBatch();
        
        NGRect window = NGRect(1, GM_CFG->_camHeight - height - 1, width, height);
        Color windowColor = Color::BLUE;
        windowColor.alpha = 0.5f;
        _fillPolygonBatcher->renderRect(window);
        _fillPolygonBatcher->endBatch(_colorNGShader, windowColor);
        
        int selectionIndex = _input->_selectionIndex;
        int row = 1;
        static float padding = 5;
        
        _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
        renderText("Entities", width / 2, GM_CFG->_camHeight - 1 - (row * padding), FONT_ALIGN_CENTER);
        _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture);
        
        ++row;
        
        for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
        {
            _spriteBatchers[i]->beginBatch(i);
        }
        
        std::fill(_textures, _textures + NUM_SPRITE_BATCHERS, "");
        
        for (int i = clamp(selectionIndex - 2, numEntities - 1, 0); i < numEntities; ++i)
        {
            EntityDef* ed = entityDescriptors[i];
            
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText(StringUtil::format("%s | %s", ed->typeName.c_str(), ed->name.c_str()).c_str(), 7, GM_CFG->_camHeight - 1 - (row * padding), FONT_ALIGN_LEFT);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, i == selectionIndex ? Color::WHITE : Color::BLACK);
            
            TextureRegion& tr = ASSETS->findTextureRegion(ed->textureMappings[0], 0);
            _spriteBatchers[tr._layer]->renderSprite(4, GM_CFG->_camHeight - 1 - (row * padding), 4, 4, 0, tr);
            _textures[tr._layer] = tr.getTextureName();
            
            ++row;
        }
        
        for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
        {
            if (_textures[i].length() > 0)
            {
                _spriteBatchers[i]->endBatch(_textureNGShader, _textureManager->getTextureWithName(_textures[i]));
            }
        }
    }
    else if (_engineState & StudioEngineState_DisplayControls)
    {
        /// Controls
        _fillPolygonBatcher->beginBatch();
        int width = 22;
        int height = 18;
        NGRect window = NGRect(GM_CFG->_camWidth - width - 1, GM_CFG->_camHeight - height - 3, width, height);
        Color windowColor = Color::BLUE;
        windowColor.alpha = 0.5f;
        _fillPolygonBatcher->renderRect(window);
        _fillPolygonBatcher->endBatch(_colorNGShader, windowColor);
        
        _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
        
        int row = 4;
        static float padding = 1;
        
        renderText(StringUtil::format("[B]   Box2D Debug %s", _engineState & StudioEngineState_DisplayBox2D ? " ON" : "OFF").c_str(), GM_CFG->_camWidth - 2, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[G]          Grid %s", _engineState & StudioEngineState_DisplayGrid ? " ON" : "OFF").c_str(), GM_CFG->_camWidth - 2, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[D]   Debug Types %s", _engineState & StudioEngineState_DisplayTypes ? " ON" : "OFF").c_str(), GM_CFG->_camWidth - 2, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[P]      Parallax %s", _engineState & StudioEngineState_DisplayParallax ? " ON" : "OFF").c_str(), GM_CFG->_camWidth - 2, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        ++row;
        renderText(StringUtil::format("[R]      Reset Camera").c_str(), GM_CFG->_camWidth - 2, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[X]       Reset World").c_str(), GM_CFG->_camWidth - 2, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[T]         Test Zone").c_str(), GM_CFG->_camWidth - 2, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        ++row;
        renderText(StringUtil::format("[N]               New").c_str(), GM_CFG->_camWidth - 2, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[L]              Load").c_str(), GM_CFG->_camWidth - 2, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[S]              Save").c_str(), GM_CFG->_camWidth - 2, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[CTRL+S]      Save As").c_str(), GM_CFG->_camWidth - 2, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        ++row;
        renderText(StringUtil::format("[C]  %s Controls", _engineState & StudioEngineState_DisplayControls ? "Hide   " : "Display").c_str(), GM_CFG->_camWidth - 2, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[A]  %s   Assets", _engineState & StudioEngineState_DisplayAssets ? "Hide   " : "Display").c_str(), GM_CFG->_camWidth - 2, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[E]  %s Entities", _engineState & StudioEngineState_DisplayEntities ? "Hide   " : "Display").c_str(), GM_CFG->_camWidth - 2, GM_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        
        _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture);
    }
    
    {
        /// Top Bar
        _fillPolygonBatcher->beginBatch();
        NGRect bar = NGRect(0, GM_CFG->_camHeight - 2, GM_CFG->_camWidth, 2);
        Color barColor = Color(0.33f, 0.33f, 0.33f, 0.85f);
        _fillPolygonBatcher->renderRect(bar);
        _fillPolygonBatcher->endBatch(_colorNGShader, barColor);
        
        int column = 1;
        static float padding = 1;
        static float textY = GM_CFG->_camHeight - 2 + 1.5f;
        static float textY2 = GM_CFG->_camHeight - 2 + 0.5f;
        
        for (int i = 0; i < StudioEngineState_NumLayers; ++i)
        {
            _fontSpriteBatcher->beginBatch(10);
            renderText(StringUtil::format("%d", i).c_str(), 1 + (column++ * padding), textY, FONT_ALIGN_RIGHT);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, _engineState & (1 << (i + StudioEngineState_LayerBitBegin)) ? Color::WHITE : Color::BLACK);
        }
        
        _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
        renderText("Layers", 1 + column / 2.0f, textY2, FONT_ALIGN_CENTER);
        _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, Color::WHITE);
        
        {
            /// Render Map Info in the center of the bar
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText(StringUtil::format("%s | %s", _engine->_world->getMapName().c_str(), _engine->_world->getMapFileName().c_str()).c_str(), GM_CFG->_camWidth / 2, textY, FONT_ALIGN_CENTER);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, Color::WHITE);
            
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText("Map", GM_CFG->_camWidth / 2, textY2, FONT_ALIGN_CENTER);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, Color::WHITE);
        }
        
        column = 56;
        {
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText("C", 1 + (column++ * padding), textY, FONT_ALIGN_LEFT);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, _engineState & StudioEngineState_DisplayControls ? Color::WHITE : Color::BLACK);
            
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText("A", 1 + (column++ * padding), textY, FONT_ALIGN_LEFT);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, _engineState & StudioEngineState_DisplayAssets ? Color::WHITE : Color::BLACK);
            
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText("E", 1 + (column++ * padding), textY, FONT_ALIGN_LEFT);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, _engineState & StudioEngineState_DisplayEntities ? Color::WHITE : Color::BLACK);
            
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText("Windows", 58, textY2, FONT_ALIGN_CENTER);
            _fontSpriteBatcher->endBatch(_textureNGShader, _fontTexture, NULL, Color::WHITE);
        }
    }
    
    {
        /// Toasts
        int y = GM_CFG->_camHeight - 4;
        _fillPolygonBatcher->beginBatch();
        _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
        for (std::string t : _toasts)
        {
            NGRect window = NGRect(GM_CFG->_camWidth / 2 - t.length() / 2.0f - 1, y - 1, t.length() + 1, 2);
            _fillPolygonBatcher->renderRect(window);
            renderText(t.c_str(), GM_CFG->_camWidth / 2, y, FONT_ALIGN_CENTER);
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
    float fgWidth = GM_CFG->_camWidth / 64;
    float fgHeight = fgWidth * (75.0f / 64.0f);

    std::string text(inStr);

    _font->renderText(*_fontSpriteBatcher, text, x, y, fgWidth, fgHeight, justification);
}

void StudioRenderer::endFrame()
{
    assert(_fbIndex >= 0);

    _rendererHelper->bindScreenFramebuffer();
    _rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
    _rendererHelper->useScreenBlending();

    _framebufferToScreenNGShader->bind(_rendererHelper->getOffscreenFramebuffer(_fbIndex)->texture);
    _rendererHelper->bindScreenVertexBuffer();
    _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
    _framebufferToScreenNGShader->unbind();

    _rendererHelper->disableBlending();
}
