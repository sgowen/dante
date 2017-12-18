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
#include "framework/graphics/portable/RendererHelper.h"
#include "framework/graphics/portable/GpuProgram.h"
#include "framework/entity/Entity.h"
#include "framework/graphics/portable/TextureRegion.h"
#include "framework/graphics/portable/Color.h"
#include "framework/math/NGRect.h"
#include "framework/graphics/portable/TextureWrapper.h"

#include "framework/graphics/portable/GpuTextureWrapper.h"
#include "framework/graphics/portable/GpuTextureDataWrapper.h"
#include "framework/graphics/portable/TextureLoaderFactory.h"
#include "framework/graphics/portable/RendererHelperFactory.h"
#include "framework/graphics/portable/GpuProgramFactory.h"
#include "framework/util/NGSTDUtil.h"
#include "framework/math/Circle.h"
#include "framework/math/Line.h"

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
_textureGpuProgram(NULL),
_colorGpuProgram(NULL),
_framebufferToScreenGpuProgram(NULL),
_framebufferIndex(0),
_areDeviceDependentResourcesCreated(false),
_areWindowSizeDependentResourcesCreated(false)
{
    // Empty
}

Renderer::~Renderer()
{
    delete _spriteBatcher;
    delete _fillNGRectBatcher;
    delete _boundsNGRectBatcher;
    delete _lineBatcher;
    delete _circleBatcher;
    
    delete _textureLoader;
    delete _rendererHelper;
}

void Renderer::createDeviceDependentResources()
{
	_rendererHelper->createDeviceDependentResources();

    _textureGpuProgram = GPU_PROGRAM_WRAPPER_FACTORY->createTextureGpuProgram(_rendererHelper);
    _colorGpuProgram = GPU_PROGRAM_WRAPPER_FACTORY->createColorGpuProgram(_rendererHelper);
    _framebufferToScreenGpuProgram = GPU_PROGRAM_WRAPPER_FACTORY->createFramebufferToScreenGpuProgram(_rendererHelper);
    
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
    
    delete _textureGpuProgram;
	_textureGpuProgram = NULL;

    delete _colorGpuProgram;
	_colorGpuProgram = NULL;

    delete _framebufferToScreenGpuProgram;
	_framebufferToScreenGpuProgram = NULL;
}

void Renderer::loadTextureDataSync(TextureWrapper* arg)
{
    assert(arg != NULL);
    
    TextureWrapper* textureWrapper = static_cast<TextureWrapper*>(arg);
    
    assert(textureWrapper->_renderer != NULL);
    assert(textureWrapper->name.length() > 0);
    
    if (textureWrapper->gpuTextureWrapper
        || !textureWrapper->_renderer->_areDeviceDependentResourcesCreated)
    {
        return;
    }
    
    textureWrapper->_isLoadingData = true;
    textureWrapper->gpuTextureDataWrapper = textureWrapper->_renderer->_textureLoader->loadTextureData(textureWrapper);
}

void Renderer::loadTextureSync(TextureWrapper* textureWrapper)
{
    loadTextureDataSync(textureWrapper);
    
    textureWrapper->gpuTextureWrapper = _textureLoader->loadTexture(textureWrapper->gpuTextureDataWrapper, textureWrapper->_repeatS);
    
    delete textureWrapper->gpuTextureDataWrapper;
    textureWrapper->gpuTextureDataWrapper = NULL;
    
    textureWrapper->_isLoadingData = false;
}

void tthreadLoadTextureDataSync(void* arg)
{
    assert(arg != NULL);
    
    TextureWrapper* textureWrapper = static_cast<TextureWrapper*>(arg);
    
    textureWrapper->_renderer->loadTextureDataSync(textureWrapper);
}

void Renderer::loadTextureAsync(TextureWrapper* textureWrapper)
{
    assert(textureWrapper != NULL);
    assert(textureWrapper->name.length() > 0);
    
    if (textureWrapper->gpuTextureWrapper
        || textureWrapper->_isLoadingData
        || !_areDeviceDependentResourcesCreated)
    {
        return;
    }
    
    _loadingTextures.push_back(textureWrapper);
    
    textureWrapper->_isLoadingData = true;
    _textureDataLoadingThreads.push_back(new tthread::thread(tthreadLoadTextureDataSync, textureWrapper));
}

void Renderer::unloadTexture(TextureWrapper* textureWrapper)
{
    if (textureWrapper == NULL)
    {
        return;
    }
    
    for (std::vector<TextureWrapper *>::iterator i = _loadingTextures.begin(); i != _loadingTextures.end(); )
    {
        if ((*i) == textureWrapper)
        {
            i = _loadingTextures.erase(i);
        }
        else
        {
            ++i;
        }
    }
    
    if (textureWrapper->gpuTextureWrapper)
    {
        _rendererHelper->destroyTexture(*textureWrapper->gpuTextureWrapper);
        
        delete textureWrapper->gpuTextureWrapper;
        textureWrapper->gpuTextureWrapper = NULL;
    }
    
    if (textureWrapper->gpuTextureDataWrapper)
    {
        delete textureWrapper->gpuTextureDataWrapper;
        textureWrapper->gpuTextureDataWrapper = NULL;
    }
    
    textureWrapper->_isLoadingData = false;
}

bool Renderer::ensureTexture(TextureWrapper* textureWrapper)
{
    if (textureWrapper->gpuTextureWrapper == NULL)
    {
        if (!textureWrapper->_isLoadingData)
        {
            loadTextureAsync(textureWrapper);
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
    
    static TextureRegion tr = TextureRegion("framebuffer", 0, 0, 1, 1, 1, 1);
    
    _spriteBatcher->beginBatch();
    _spriteBatcher->renderSprite(0, 0, 2, 2, 0, tr);
    _spriteBatcher->endBatch(_rendererHelper->getFramebuffer(_framebufferIndex), *_framebufferToScreenGpuProgram);
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
    _circleBatcher->renderCircle(c1, Color::RED, *_colorGpuProgram);
    
    static Circle c2(7, 7, 2);
    _circleBatcher->renderPartialCircle(c2, 135, Color::RED, *_colorGpuProgram);
    
    static NGRect r1(1, 1, 2, 1);
    _boundsNGRectBatcher->beginBatch();
    _boundsNGRectBatcher->renderNGRect(r1, Color::RED);
    _boundsNGRectBatcher->endBatch(*_colorGpuProgram);
    
    static NGRect r2(4, 1, 2, 1);
    _fillNGRectBatcher->beginBatch();
    _fillNGRectBatcher->renderNGRect(r2, Color::RED);
    _fillNGRectBatcher->endBatch(*_colorGpuProgram);
    
    static Line line(3, 3, 5, 5);
    _lineBatcher->beginBatch();
    _lineBatcher->renderLine(line, Color::RED);
    _lineBatcher->endBatch(*_colorGpuProgram);
}

#pragma mark private

void Renderer::handleAsyncTextureLoads()
{
    for (std::vector<TextureWrapper *>::iterator i = _loadingTextures.begin(); i != _loadingTextures.end(); )
    {
        if ((*i)->gpuTextureDataWrapper)
        {
            (*i)->gpuTextureWrapper = _textureLoader->loadTexture((*i)->gpuTextureDataWrapper, (*i)->_repeatS);
            
            delete (*i)->gpuTextureDataWrapper;
            (*i)->gpuTextureDataWrapper = NULL;
            
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
