//
//  StudioRenderer.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/studio/StudioRenderer.h>

#include <framework/graphics/portable/TextureManager.h>
#include <framework/graphics/portable/Font.h>
#include <framework/studio/World.h>
#include <framework/graphics/portable/SpriteBatcher.h>
#include <framework/graphics/portable/PolygonBatcher.h>
#include <framework/graphics/portable/LineBatcher.h>
#include <framework/graphics/portable/CircleBatcher.h>
#include <framework/graphics/portable/ShaderProgramLoader.h>
#include <framework/graphics/portable/RendererHelper.h>
#include <framework/graphics/portable/NGShader.h>
#include <framework/graphics/portable/NGWaterShader.h>
#include <framework/entity/Entity.h>
#include <framework/graphics/portable/TextureRegion.h>
#include <framework/math/NGRect.h>
#include <framework/graphics/portable/NGTexture.h>
#include <framework/graphics/portable/Box2DDebugRenderer.h>
#include <framework/studio/StudioEngine.h>
#include <framework/studio/StudioInputManager.h>
#include <framework/graphics/portable/FramebufferWrapper.h>
#include <framework/studio/WorldRenderer.h>

#include <framework/graphics/portable/Color.h>
#include <framework/file/portable/Assets.h>
#include <framework/graphics/portable/RendererHelper.h>
#include <framework/util/Constants.h>
#include <framework/math/NGRect.h>
#include <framework/math/Line.h>
#include <framework/graphics/portable/SpriteBatcher.h>
#include <framework/graphics/portable/TextureRegion.h>
#include <framework/util/macros.h>
#include <framework/util/StringUtil.h>
#include <framework/util/NGSTDUtil.h>
#include <framework/util/MathUtil.h>
#include <framework/audio/portable/NGAudioEngine.h>
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
#include <framework/util/Config.h>
#include <framework/util/StringUtil.h>

#include <sstream>
#include <ctime> // rand
#include <string>
#include <assert.h>
#include <cfloat>

StudioRenderer::StudioRenderer() : Renderer(),
_textureManager(new TextureManager()),
_rendererHelper(RENDERER_HELPER_FACTORY->createRendererHelper()),
_shaderProgramLoader(SHADER_PROGRAM_LOADER_FACTORY->createShaderLoader()),
_worldRenderer(new WorldRenderer(_rendererHelper)),
_fontSpriteBatcher(new SpriteBatcher(_rendererHelper)),
_fbSpriteBatcher(new SpriteBatcher(_rendererHelper)),
_activeEntitySpriteBatcher(new SpriteBatcher(_rendererHelper)),
_fillPolygonBatcher(new PolygonBatcher(_rendererHelper, true)),
_boundsPolygonBatcher(new PolygonBatcher(_rendererHelper, false)),
_lineBatcher(new LineBatcher(_rendererHelper)),
_circleBatcher(new CircleBatcher(_rendererHelper)),
_box2DDebugRenderer(new Box2DDebugRenderer(*_fillPolygonBatcher, *_boundsPolygonBatcher, *_lineBatcher, *_circleBatcher)),
_font(new Font(0, 0, 16, 64, 75, 1024, 1024)),
_textureShader(new NGTextureShader(*_rendererHelper)),
_waterShader(new NGWaterShader(*_rendererHelper)),
_colorShader(new NGGeometryShader(*_rendererHelper)),
_framebufferToScreenShader(new NGFramebufferToScreenShader(*_rendererHelper)),
_toastStateTime(0),
_fbIndex(0),
_scrollValue(1),
_engine(NULL),
_input(NULL),
_engineState(0),
_textInputField(0)
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

StudioRenderer::~StudioRenderer()
{
	delete _textureManager;
    delete _rendererHelper;
    delete _shaderProgramLoader;
    delete _worldRenderer;
    delete _fontSpriteBatcher;
    delete _fbSpriteBatcher;
    delete _activeEntitySpriteBatcher;
    delete _fillPolygonBatcher;
    delete _boundsPolygonBatcher;
    delete _lineBatcher;
    delete _circleBatcher;
    delete _box2DDebugRenderer;
    delete _font;
    
    delete _textureShader;
    delete _waterShader;
    delete _colorShader;
    delete _framebufferToScreenShader;
    
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        delete _spriteBatchers[i];
    }
    
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        delete _camBounds[i];
    }
}

void StudioRenderer::createDeviceDependentResources()
{
    _engine = StudioEngine::getInstance();
    _input = StudioInputManager::getInstance();
    
    _rendererHelper->createDeviceDependentResources();
    _textureManager->createDeviceDependentResources();
    
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        _camBounds[i]->setWidth(FW_CFG->_camWidth);
        _camBounds[i]->setHeight(FW_CFG->_camHeight);
    }
    
    _fontTexture = _textureManager->getTextureWithName("texture_000.ngt");

    _textureShader->load(*_shaderProgramLoader);
    _waterShader->load(*_shaderProgramLoader);
    _colorShader->load(*_shaderProgramLoader);
    _framebufferToScreenShader->load(*_shaderProgramLoader);
    
    createFramebuffers();
}

void StudioRenderer::createWindowSizeDependentResources(int screenWidth, int screenHeight)
{
    _rendererHelper->createWindowSizeDependentResources(screenWidth, screenHeight);
}

void StudioRenderer::releaseDeviceDependentResources()
{
    _rendererHelper->releaseDeviceDependentResources();
    _textureManager->releaseDeviceDependentResources();

    _textureShader->unload(*_shaderProgramLoader);
    _colorShader->unload(*_shaderProgramLoader);
    _framebufferToScreenShader->unload(*_shaderProgramLoader);
}

void StudioRenderer::render()
{
    _engineState = _engine->_state;
    _textInputField = _engine->_textInputField;
    
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
            _activeEntitySpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            TextureRegion& tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
            _activeEntitySpriteBatcher->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
            _activeEntitySpriteBatcher->endBatch(_textureShader, _textureManager->getTextureWithName(tr.getTextureName()), NULL, Color::DOUBLE);
        }
        
        if (_engineState & SES_DisplayBox2D)
        {
            renderBox2D();
        }
        
        if (_engineState & SES_DisplayGrid)
        {
            renderGrid();
        }
        
        renderUI();
    }

    endFrame();
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
    
    NGTexture* texture = _textureManager->getTextureWithName(trWaterSurface.getTextureName());
    std::string tfMin = texture->_desc->_textureFilterMin;
    std::string tfMag = texture->_desc->_textureFilterMag;
    
    {
        FramebufferWrapper* fbTemp = _rendererHelper->addFramebuffer(textureNameTemp, renderWidth, renderHeight, tfMin, tfMag);
        bindFramebuffer(fbTemp);
        _fbSpriteBatcher->endBatch(_textureShader, _textureManager->getTextureWithName(trWaterSurface.getTextureName()));
        
        FramebufferWrapper* fb = _rendererHelper->addFramebuffer(textureName, renderWidth, renderHeight, tfMin, tfMag);
        bindFramebuffer(fb);
        
        _fbSpriteBatcher->beginBatch(0);
        _fbSpriteBatcher->renderSprite(e->getWidth() / 2, e->getHeight() / 2, e->getWidth(), e->getHeight(), 0, *tr);
        _fbSpriteBatcher->endBatch(_textureShader, fbTemp->texture);
        
        _rendererHelper->removeFramebuffer(textureNameTemp);
        _textureManager->registerFramebuffer(textureName, fb);
    }
    
    {
        FramebufferWrapper* fbTemp = _rendererHelper->addFramebuffer(normalMapNameTemp, renderWidth, renderHeight, tfMin, tfMag);
        bindFramebuffer(fbTemp);
        _fbSpriteBatcher->endBatch(_textureShader, _textureManager->getTextureWithName(trWaterSurface.getNormalMapName()));
        
        FramebufferWrapper* fb = _rendererHelper->addFramebuffer(normalMapName, renderWidth, renderHeight, tfMin, tfMag);
        bindFramebuffer(fb);
        
        _fbSpriteBatcher->beginBatch(0);
        _fbSpriteBatcher->renderSprite(e->getWidth() / 2, e->getHeight() / 2, e->getWidth(), e->getHeight(), 0, *tr);
        _fbSpriteBatcher->endBatch(_textureShader, fbTemp->texture);
        
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
    _camBounds[2]->getLowerLeft().set(x * FW_CFG->_parallaxLayer2FactorX, y * FW_CFG->_parallaxLayer2FactorY);
    _camBounds[1]->getLowerLeft().set(x * FW_CFG->_parallaxLayer1FactorX, y * FW_CFG->_parallaxLayer1FactorY);
    _camBounds[0]->getLowerLeft().set(x * FW_CFG->_parallaxLayer0FactorX, y * FW_CFG->_parallaxLayer0FactorY);
    
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
    bindFramebuffer(fb, r, g, b, a);
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
    
    if (_engineState & SES_DisplayTypes)
    {
        _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
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
        _framebufferToScreenShader->bind(_rendererHelper->getOffscreenFramebuffer(i)->texture);
        _rendererHelper->bindScreenVertexBuffer();
        _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
        _framebufferToScreenShader->unbind();
    }
    
    if (_engineState & SES_DisplayTypes)
    {
        _fontSpriteBatcher->endBatch(_textureShader, _fontTexture);
    }
}

void StudioRenderer::renderEntities(std::vector<Entity*>& entities)
{
    for (Entity* e : entities)
    {
        TextureRegion& tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
        
        _spriteBatchers[tr._layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
        _textures[tr._layer] = tr.getTextureName();
        
        if ((_engineState & SES_DisplayTypes) &&
            _engineState & (1 << (tr._layer + SES_LayerBitBegin)))
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
        
        SpriteBatcher* sb = _spriteBatchers[layer];
        NGTexture* tex = _textureManager->getTextureWithName(tr.getTextureName());
        NGTexture* tex2 = _textureManager->getTextureWithName("texture_010.ngt");
        if (tex && _engineState & (1 << (layer + SES_LayerBitBegin)))
        {
            int c = clamp(layer, 0, 3);
            if (!(_engineState & SES_DisplayParallax))
            {
                c = 3;
            }
            
            _rendererHelper->updateMatrix(_camBounds[c]->getLeft(), _camBounds[c]->getRight(), _camBounds[c]->getBottom(), _camBounds[c]->getTop());
            
            _waterShader->update(FRAME_RATE); // e->getStateTime()
            sb->endBatch(_waterShader, tex, tex2);
        }
    }
}

void StudioRenderer::endBatchWithTexture(SpriteBatcher* sb, NGTexture* tex, int layer)
{
    if (tex && _engineState & (1 << (layer + SES_LayerBitBegin)))
    {
        int c = clamp(layer, 0, 3);
        if (!(_engineState & SES_DisplayParallax))
        {
            c = 3;
        }
        
        _rendererHelper->updateMatrix(_camBounds[c]->getLeft(), _camBounds[c]->getRight(), _camBounds[c]->getBottom(), _camBounds[c]->getTop());
        
        sb->endBatch(_textureShader, tex);
    }
}

void StudioRenderer::renderBox2D()
{
    _rendererHelper->useScreenBlending();
    _rendererHelper->updateMatrix(_camBounds[3]->getLeft(), _camBounds[3]->getRight(), _camBounds[3]->getBottom(), _camBounds[3]->getTop());
    _box2DDebugRenderer->render(&_engine->_world->getWorld(), _colorShader);
}

void StudioRenderer::renderGrid()
{
    float x = clamp(_camBounds[3]->getLeft(), 0, FLT_MAX);
    float y = clamp(_camBounds[3]->getBottom(), 0, FLT_MAX);
    float px = fmodf(x, FW_CFG->_camWidth);
    float py = fmodf(y, FW_CFG->_camHeight);
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
    _lineBatcher->endBatch(_colorShader, lineColor);
    
    _lineBatcher->beginBatch();
    for (int i = leftAligned; i <= camWidth; i += FW_CFG->_camWidth)
    {
        _lineBatcher->renderLine(i, 0, i, camHeight);
    }
    for (int i = bottomAligned; i <= camHeight; i += FW_CFG->_camHeight)
    {
        _lineBatcher->renderLine(0, i, camWidth, i);
    }
    _lineBatcher->endBatch(_colorShader, Color::GREEN);
    
    _lineBatcher->beginBatch();
    _lineBatcher->renderLine(0, 0, 0, camHeight);
    _lineBatcher->renderLine(0, 0, camWidth, 0);
    _lineBatcher->endBatch(_colorShader, Color::RED);
}

void StudioRenderer::renderUI()
{
    _rendererHelper->useScreenBlending();
    _rendererHelper->updateMatrix(0, FW_CFG->_camWidth, 0, FW_CFG->_camHeight);
    
    if (_engineState & SES_TextInput)
    {
        _fillPolygonBatcher->beginBatch();
        int width = FW_CFG->_camWidth / 3;
        int height = 5;
        NGRect window = NGRect(FW_CFG->_camWidth / 2 - width / 2, FW_CFG->_camHeight - 4 - height - 1, width, height);
        Color windowColor = Color::BLUE;
        windowColor.alpha = 0.5f;
        _fillPolygonBatcher->renderRect(window);
        _fillPolygonBatcher->endBatch(_colorShader, windowColor);
        
        int row = 2;
        static float padding = 1;
        
        _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
        
        if (_textInputField == SETIF_WaterDepth)
        {
            renderText("Enter Water Depth", FW_CFG->_camWidth / 2, FW_CFG->_camHeight - 4 - (row++ * padding), FONT_ALIGN_CENTER);
        }
        else if (_textInputField == SETIF_WaterWidth)
        {
            renderText("Enter Water Width", FW_CFG->_camWidth / 2, FW_CFG->_camHeight - 4 - (row++ * padding), FONT_ALIGN_CENTER);
        }
        renderText(_input->getLiveInput().c_str(), FW_CFG->_camWidth / 2, FW_CFG->_camHeight - 4 - (row++ * padding), FONT_ALIGN_CENTER);
        _fontSpriteBatcher->endBatch(_textureShader, _fontTexture);
    }
    else if (_engineState & SES_DisplayLoadMapDialog)
    {
        /// Maps
        std::vector<MapDef>& maps = EntityLayoutMapper::getInstance()->getMaps();
        int numMaps = static_cast<int>(maps.size());
        
        _fillPolygonBatcher->beginBatch();
        int width = FW_CFG->_camWidth / 3;
        int height = numMaps + 3;
        NGRect window = NGRect(FW_CFG->_camWidth / 2 - width / 2, FW_CFG->_camHeight - 4 - height - 1, width, height);
        Color windowColor = Color::BLUE;
        windowColor.alpha = 0.5f;
        _fillPolygonBatcher->renderRect(window);
        _fillPolygonBatcher->endBatch(_colorShader, windowColor);
        
        int row = 2;
        static float padding = 1;
        
        _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
        renderText("Load Map", FW_CFG->_camWidth / 2, FW_CFG->_camHeight - 4 - (row++ * padding), FONT_ALIGN_CENTER);
        _fontSpriteBatcher->endBatch(_textureShader, _fontTexture);
        
        ++row;
        
        for (int i = 0; i < numMaps; ++i)
        {
            MapDef& mp = maps[i];
            
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText(StringUtil::format("%s | %s", mp.name.c_str(), mp.value.c_str()).c_str(), FW_CFG->_camWidth / 2, FW_CFG->_camHeight - 4 - (row++ * padding), FONT_ALIGN_CENTER);
            _fontSpriteBatcher->endBatch(_textureShader, _fontTexture, NULL, i == _input->_selectionIndex ? Color::WHITE : Color::BLACK);
        }
    }
    else if (_engineState & SES_DisplayEntities)
    {
        const std::vector<EntityDef*>& entityDescriptors = EntityMapper::getInstance()->getEntityDescriptors();
        int numEntities = static_cast<int>(entityDescriptors.size());
        
        int width = FW_CFG->_camWidth * 0.7f;
        int height = numEntities + 3;
        
        _rendererHelper->updateMatrix(0, FW_CFG->_camWidth, 0, FW_CFG->_camHeight);
        
        _fillPolygonBatcher->beginBatch();
        
        NGRect window = NGRect(1, FW_CFG->_camHeight - height - 1, width, height);
        Color windowColor = Color::BLUE;
        windowColor.alpha = 0.5f;
        _fillPolygonBatcher->renderRect(window);
        _fillPolygonBatcher->endBatch(_colorShader, windowColor);
        
        int selectionIndex = _input->_selectionIndex;
        int row = 1;
        static float padding = 5;
        
        _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
        renderText("Entities", width / 2, FW_CFG->_camHeight - 1 - (row * padding), FONT_ALIGN_CENTER);
        _fontSpriteBatcher->endBatch(_textureShader, _fontTexture);
        
        ++row;
        
        for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
        {
            _spriteBatchers[i]->beginBatch(i);
        }
        
        std::fill(_textures, _textures + NUM_SPRITE_BATCHERS, "");
        
        for (int i = clamp(selectionIndex - 2, 0, numEntities - 1); i < numEntities; ++i)
        {
            EntityDef* ed = entityDescriptors[i];
            
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText(StringUtil::format("%s | %s", ed->typeName.c_str(), ed->name.c_str()).c_str(), 7, FW_CFG->_camHeight - 1 - (row * padding), FONT_ALIGN_LEFT);
            _fontSpriteBatcher->endBatch(_textureShader, _fontTexture, NULL, i == selectionIndex ? Color::WHITE : Color::BLACK);
            
            TextureRegion& tr = ASSETS->findTextureRegion(ed->textureMappings[0], 0);
            _spriteBatchers[tr._layer]->renderSprite(4, FW_CFG->_camHeight - 1 - (row * padding), 4, 4, 0, tr);
            _textures[tr._layer] = tr.getTextureName();
            
            ++row;
        }
        
        for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
        {
            if (_textures[i].length() > 0)
            {
                _spriteBatchers[i]->endBatch(_textureShader, _textureManager->getTextureWithName(_textures[i]));
            }
        }
    }
    else if (_engineState & SES_DisplayControls)
    {
        /// Controls
        _fillPolygonBatcher->beginBatch();
        int width = 22;
        int height = 18;
        NGRect window = NGRect(FW_CFG->_camWidth - width - 1, FW_CFG->_camHeight - height - 3, width, height);
        Color windowColor = Color::BLUE;
        windowColor.alpha = 0.5f;
        _fillPolygonBatcher->renderRect(window);
        _fillPolygonBatcher->endBatch(_colorShader, windowColor);
        
        _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
        
        int row = 4;
        static float padding = 1;
        
        renderText(StringUtil::format("[B]   Box2D Debug %s", _engineState & SES_DisplayBox2D ? " ON" : "OFF").c_str(), FW_CFG->_camWidth - 2, FW_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[G]          Grid %s", _engineState & SES_DisplayGrid ? " ON" : "OFF").c_str(), FW_CFG->_camWidth - 2, FW_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[D]   Debug Types %s", _engineState & SES_DisplayTypes ? " ON" : "OFF").c_str(), FW_CFG->_camWidth - 2, FW_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[P]      Parallax %s", _engineState & SES_DisplayParallax ? " ON" : "OFF").c_str(), FW_CFG->_camWidth - 2, FW_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        ++row;
        renderText(StringUtil::format("[R]      Reset Camera").c_str(), FW_CFG->_camWidth - 2, FW_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[X]       Reset World").c_str(), FW_CFG->_camWidth - 2, FW_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[T]         Test Zone").c_str(), FW_CFG->_camWidth - 2, FW_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        ++row;
        renderText(StringUtil::format("[N]               New").c_str(), FW_CFG->_camWidth - 2, FW_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[L]              Load").c_str(), FW_CFG->_camWidth - 2, FW_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[S]              Save").c_str(), FW_CFG->_camWidth - 2, FW_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[CTRL+S]      Save As").c_str(), FW_CFG->_camWidth - 2, FW_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        ++row;
        renderText(StringUtil::format("[C]  %s Controls", _engineState & SES_DisplayControls ? "Hide   " : "Display").c_str(), FW_CFG->_camWidth - 2, FW_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[A]  %s   Assets", _engineState & SES_DisplayAssets ? "Hide   " : "Display").c_str(), FW_CFG->_camWidth - 2, FW_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        renderText(StringUtil::format("[E]  %s Entities", _engineState & SES_DisplayEntities ? "Hide   " : "Display").c_str(), FW_CFG->_camWidth - 2, FW_CFG->_camHeight - (row++ * padding), FONT_ALIGN_RIGHT);
        
        _fontSpriteBatcher->endBatch(_textureShader, _fontTexture);
    }
    
    {
        /// Top Bar
        _fillPolygonBatcher->beginBatch();
        NGRect bar = NGRect(0, FW_CFG->_camHeight - 2, FW_CFG->_camWidth, 2);
        Color barColor = Color(0.33f, 0.33f, 0.33f, 0.85f);
        _fillPolygonBatcher->renderRect(bar);
        _fillPolygonBatcher->endBatch(_colorShader, barColor);
        
        int column = 1;
        static float padding = 1;
        static float textY = FW_CFG->_camHeight - 2 + 1.5f;
        static float textY2 = FW_CFG->_camHeight - 2 + 0.5f;
        
        for (int i = 0; i < SES_NumLayers; ++i)
        {
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText(StringUtil::format("%d", i).c_str(), 1 + (column++ * padding), textY, FONT_ALIGN_RIGHT);
            _fontSpriteBatcher->endBatch(_textureShader, _fontTexture, NULL, _engineState & (1 << (i + SES_LayerBitBegin)) ? Color::WHITE : Color::BLACK);
        }
        
        _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
        renderText("Layers", 1 + column / 2.0f, textY2, FONT_ALIGN_CENTER);
        _fontSpriteBatcher->endBatch(_textureShader, _fontTexture, NULL, Color::WHITE);
        
        {
            /// Render Map Info in the center of the bar
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText(StringUtil::format("%s | %s", _engine->_world->getMapName().c_str(), _engine->_world->getMapFileName().c_str()).c_str(), FW_CFG->_camWidth / 2, textY, FONT_ALIGN_CENTER);
            _fontSpriteBatcher->endBatch(_textureShader, _fontTexture, NULL, Color::WHITE);
            
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText("Map", FW_CFG->_camWidth / 2, textY2, FONT_ALIGN_CENTER);
            _fontSpriteBatcher->endBatch(_textureShader, _fontTexture, NULL, Color::WHITE);
        }
        
        column = 56;
        {
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText("C", 1 + (column++ * padding), textY, FONT_ALIGN_LEFT);
            _fontSpriteBatcher->endBatch(_textureShader, _fontTexture, NULL, _engineState & SES_DisplayControls ? Color::WHITE : Color::BLACK);
            
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText("A", 1 + (column++ * padding), textY, FONT_ALIGN_LEFT);
            _fontSpriteBatcher->endBatch(_textureShader, _fontTexture, NULL, _engineState & SES_DisplayAssets ? Color::WHITE : Color::BLACK);
            
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText("E", 1 + (column++ * padding), textY, FONT_ALIGN_LEFT);
            _fontSpriteBatcher->endBatch(_textureShader, _fontTexture, NULL, _engineState & SES_DisplayEntities ? Color::WHITE : Color::BLACK);
            
            _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
            renderText("Windows", 58, textY2, FONT_ALIGN_CENTER);
            _fontSpriteBatcher->endBatch(_textureShader, _fontTexture, NULL, Color::WHITE);
        }
    }
    
    {
        /// Toasts
        int y = FW_CFG->_camHeight - 4;
        _fillPolygonBatcher->beginBatch();
        _fontSpriteBatcher->beginBatch(INDEX_LAST_TEXTURE_VERTEX_BUFFER);
        for (std::string t : _toasts)
        {
            NGRect window = NGRect(FW_CFG->_camWidth / 2 - t.length() / 2.0f - 1, y - 1, t.length() + 1, 2);
            _fillPolygonBatcher->renderRect(window);
            renderText(t.c_str(), FW_CFG->_camWidth / 2, y, FONT_ALIGN_CENTER);
            y -= 2;
        }
        Color windowColor = Color::BLUE;
        windowColor.alpha = 0.5f;
        _fillPolygonBatcher->endBatch(_colorShader, windowColor);
        _fontSpriteBatcher->endBatch(_textureShader, _fontTexture);
    }
}

void StudioRenderer::renderText(const char* inStr, float x, float y, int justification)
{
    float fgWidth = FW_CFG->_camWidth / 64.0f;
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

    _framebufferToScreenShader->bind(_rendererHelper->getOffscreenFramebuffer(_fbIndex)->texture);
    _rendererHelper->bindScreenVertexBuffer();
    _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
    _framebufferToScreenShader->unbind();

    _rendererHelper->disableBlending();
}
