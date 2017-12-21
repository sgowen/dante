//
//  Renderer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/Renderer.h"

#include "framework/graphics/portable/SpriteBatcher.h"
#include "framework/graphics/portable/NGRectBatcher.h"
#include "framework/graphics/portable/LineBatcher.h"
#include "framework/graphics/portable/CircleBatcher.h"
#include "framework/graphics/portable/TextureLoader.h"
#include "framework/graphics/portable/ShaderProgramLoader.h"
#include "framework/graphics/portable/RendererHelper.h"
#include "framework/graphics/portable/ShaderProgram.h"
#include "framework/entity/Entity.h"
#include "framework/graphics/portable/TextureRegion.h"
#include "framework/graphics/portable/Color.h"
#include "framework/math/NGRect.h"
#include "framework/graphics/portable/NGTexture.h"

#include "framework/graphics/portable/TextureWrapper.h"
#include "framework/graphics/portable/TextureDataWrapper.h"
#include "framework/graphics/portable/TextureLoaderFactory.h"
#include "framework/graphics/portable/ShaderProgramLoaderFactory.h"
#include "framework/graphics/portable/RendererHelperFactory.h"
#include "framework/util/NGSTDUtil.h"
#include "framework/math/Circle.h"
#include "framework/math/Line.h"
#include <framework/graphics/portable/NGTextureProgram.h>
#include <framework/graphics/portable/NGGeometryProgram.h>
#include <framework/graphics/portable/NGFramebufferToScreenProgram.h>
#include "framework/graphics/portable/NGTextureDesc.h"
#include "framework/graphics/portable/Assets.h"

#include <Box2D/Box2D.h>

#include <string>
#include <assert.h>

Renderer::Renderer() :
_rendererHelper(RENDERER_HELPER_FACTORY->createRendererHelper()),
_spriteBatcher(new SpriteBatcher(_rendererHelper)),
_fillNGRectBatcher(new NGRectBatcher(_rendererHelper, true)),
_boundsNGRectBatcher(new NGRectBatcher(_rendererHelper, false)),
_lineBatcher(new LineBatcher(_rendererHelper)),
_circleBatcher(new CircleBatcher(_rendererHelper)),
_textureLoader(TEXTURE_LOADER_FACTORY->createTextureLoader()),
_shaderProgramLoader(SHADER_PROGRAM_LOADER_FACTORY->createShaderProgramLoader()),
_textureShaderProgram(NULL),
_colorShaderProgram(NULL),
_framebufferToScreenShaderProgram(NULL),
_framebufferIndex(0),
_areDeviceDependentResourcesCreated(false),
_areWindowSizeDependentResourcesCreated(false)
{
    // Empty
}

Renderer::~Renderer()
{
    releaseDeviceDependentResources();
    
    delete _rendererHelper;
    
    delete _spriteBatcher;
    delete _fillNGRectBatcher;
    delete _boundsNGRectBatcher;
    delete _lineBatcher;
    delete _circleBatcher;
    
    delete _textureLoader;
    delete _shaderProgramLoader;
    
    NGSTDUtil::cleanUpVectorOfPointers(_textures);
}

void Renderer::createDeviceDependentResources()
{
	_rendererHelper->createDeviceDependentResources();

    _textureShaderProgram = new NGTextureProgram(*_rendererHelper, *_shaderProgramLoader, "shader_003_vert.ngs", "shader_003_frag.ngs");
    _colorShaderProgram = new NGGeometryProgram(*_rendererHelper, *_shaderProgramLoader, "shader_001_vert.ngs", "shader_001_frag.ngs");
    _framebufferToScreenShaderProgram = new NGFramebufferToScreenProgram(*_rendererHelper, *_shaderProgramLoader, "shader_002_vert.ngs", "shader_002_frag.ngs");
    
    ASSETS->initWithJsonFile("assets.cfg", true);
    
    if (_textures.size() == 0)
    {
        std::vector<NGTextureDesc*>& textureDescs = ASSETS->getTextures();
        for (NGTextureDesc* td : textureDescs)
        {
            _textures.push_back(new NGTexture(td->_textureName, this, td->_repeatS, td->_isEncrypted));
        }
    }
    
    for (NGTexture* t : _textures)
    {
        loadTextureSync(t);
    }
    
    _areDeviceDependentResourcesCreated = true;
}

void Renderer::createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int numFramebuffers)
{
	_rendererHelper->createWindowSizeDependentResources(screenWidth, screenHeight, renderWidth, renderHeight, numFramebuffers);

	_areWindowSizeDependentResourcesCreated = true;
}

void Renderer::releaseDeviceDependentResources()
{
	_rendererHelper->releaseDeviceDependentResources();

    _areDeviceDependentResourcesCreated = false;
	_areWindowSizeDependentResourcesCreated = false;

	_loadingTextures.clear();

	cleanUpThreads();
    
    delete _textureShaderProgram;
	_textureShaderProgram = NULL;

    delete _colorShaderProgram;
	_colorShaderProgram = NULL;

    delete _framebufferToScreenShaderProgram;
	_framebufferToScreenShaderProgram = NULL;
    
    for (NGTexture* t : _textures)
    {
        unloadTexture(t);
    }
}

void Renderer::loadTextureDataSync(NGTexture* arg)
{
    assert(arg != NULL);
    
    NGTexture* texture = static_cast<NGTexture*>(arg);
    
    assert(texture->_renderer != NULL);
    assert(texture->name.length() > 0);
    
    if (texture->textureWrapper
        || texture->_isLoadingData)
    {
        return;
    }
    
    texture->_isLoadingData = true;
    texture->textureDataWrapper = texture->_renderer->_textureLoader->loadTextureData(texture);
}

void Renderer::loadTextureSync(NGTexture* texture)
{
    loadTextureDataSync(texture);
    
    texture->textureWrapper = _textureLoader->loadTexture(texture->textureDataWrapper, texture->_repeatS);
    
    delete texture->textureDataWrapper;
    texture->textureDataWrapper = NULL;
    
    texture->_isLoadingData = false;
}

void tthreadLoadTextureDataSync(void* arg)
{
    assert(arg != NULL);
    
    NGTexture* texture = static_cast<NGTexture*>(arg);
    
    texture->_renderer->loadTextureDataSync(texture);
}

void Renderer::loadTextureAsync(NGTexture* texture)
{
    assert(texture != NULL);
    assert(texture->name.length() > 0);
    
    if (texture->textureWrapper
        || texture->_isLoadingData)
    {
        return;
    }
    
    _loadingTextures.push_back(texture);
    
    texture->_isLoadingData = true;
    _textureDataLoadingThreads.push_back(new tthread::thread(tthreadLoadTextureDataSync, texture));
}

void Renderer::unloadTexture(NGTexture* texture)
{
    if (texture == NULL)
    {
        return;
    }
    
    for (std::vector<NGTexture *>::iterator i = _loadingTextures.begin(); i != _loadingTextures.end(); )
    {
        if ((*i) == texture)
        {
            i = _loadingTextures.erase(i);
        }
        else
        {
            ++i;
        }
    }
    
    if (texture->textureWrapper)
    {
        _rendererHelper->destroyTexture(*texture->textureWrapper);
        
        delete texture->textureWrapper;
        texture->textureWrapper = NULL;
    }
    
    if (texture->textureDataWrapper)
    {
        delete texture->textureDataWrapper;
        texture->textureDataWrapper = NULL;
    }
    
    texture->_isLoadingData = false;
}

bool Renderer::ensureTextures()
{
    for (NGTexture* t : _textures)
    {
        if (!ensureTexture(t))
        {
            return false;
        }
    }
    
    return true;
}

bool Renderer::ensureTexture(NGTexture* texture)
{
    if (texture->textureWrapper == NULL)
    {
        if (!texture->_isLoadingData)
        {
            loadTextureAsync(texture);
        }
        
        return false;
    }
    
    return true;
}

#pragma mark protected

void Renderer::beginFrame()
{
    handleAsyncTextureLoads();
    
    _rendererHelper->beginFrame();
    
    setFramebuffer(0);
}

void Renderer::setFramebuffer(int framebufferIndex)
{
    assert(framebufferIndex >= 0);
    
    _framebufferIndex = framebufferIndex;
    
    _rendererHelper->bindToOffscreenFramebuffer(_framebufferIndex);
    _rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
}

void Renderer::renderToScreen()
{
    assert(_framebufferIndex >= 0);
    
    _rendererHelper->bindToScreenFramebuffer();
    _rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
    
    _rendererHelper->clearScreenVertices();
    _rendererHelper->addVertexCoordinate(-1, -1);
    _rendererHelper->addVertexCoordinate(-1, 1);
    _rendererHelper->addVertexCoordinate(1, 1);
    _rendererHelper->addVertexCoordinate(1, -1);
    
    _framebufferToScreenShaderProgram->bind(_rendererHelper->getFramebuffer(_framebufferIndex));
    _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, INDICES_PER_RECTANGLE);
    _framebufferToScreenShaderProgram->unbind();
}

void Renderer::endFrame()
{
    _rendererHelper->endFrame();
}

bool Renderer::isLoadingData()
{
    return _loadingTextures.size() > 0;
}

bool Renderer::isReadyForRendering()
{
    return _areDeviceDependentResourcesCreated && _areWindowSizeDependentResourcesCreated;
}

void Renderer::renderEntity(Entity &pe, TextureRegion& tr, bool flipX)
{
    _spriteBatcher->renderSprite(pe.getPosition().x, pe.getPosition().y, pe.getWidth(), pe.getHeight(), pe.getAngle(), tr, flipX);
}

void Renderer::renderEntityWithColor(Entity &pe, TextureRegion& tr, Color c, bool flipX)
{
    _spriteBatcher->renderSprite(pe.getPosition().x, pe.getPosition().y, pe.getWidth(), pe.getHeight(), pe.getAngle(), c, tr, flipX);
}

void Renderer::testRenderingSuite()
{
    _rendererHelper->updateMatrix(0, 16, 0, 9);
    
    static Circle c1(10, 4, 2);
    _circleBatcher->renderCircle(c1, Color::RED, *_colorShaderProgram);
    
    static Circle c2(7, 7, 2);
    _circleBatcher->renderPartialCircle(c2, 135, Color::RED, *_colorShaderProgram);
    
    static NGRect r1(1, 1, 2, 1);
    _boundsNGRectBatcher->beginBatch();
    _boundsNGRectBatcher->renderNGRect(r1, Color::RED);
    _boundsNGRectBatcher->endBatch(*_colorShaderProgram);
    
    static NGRect r2(4, 1, 2, 1);
    _fillNGRectBatcher->beginBatch();
    _fillNGRectBatcher->renderNGRect(r2, Color::RED);
    _fillNGRectBatcher->endBatch(*_colorShaderProgram);
    
    static Line line(3, 3, 5, 5);
    _lineBatcher->beginBatch();
    _lineBatcher->renderLine(line, Color::RED);
    _lineBatcher->endBatch(*_colorShaderProgram);
}

#pragma mark private

void Renderer::handleAsyncTextureLoads()
{
    for (std::vector<NGTexture *>::iterator i = _loadingTextures.begin(); i != _loadingTextures.end(); )
    {
        if ((*i)->textureDataWrapper)
        {
            (*i)->textureWrapper = _textureLoader->loadTexture((*i)->textureDataWrapper, (*i)->_repeatS);
            
            delete (*i)->textureDataWrapper;
            (*i)->textureDataWrapper = NULL;
            
            (*i)->_isLoadingData = false;
            
            i = _loadingTextures.erase(i);
        }
        else
        {
            ++i;
        }
    }
    
    if (_loadingTextures.size() == 0)
    {
        cleanUpThreads();
    }
}

void Renderer::cleanUpThreads()
{
    for (std::vector<tthread::thread *>::iterator i = _textureDataLoadingThreads.begin(); i != _textureDataLoadingThreads.end(); ++i)
    {
        (*i)->join();
    }
    
    NGSTDUtil::cleanUpVectorOfPointers(_textureDataLoadingThreads);
}
