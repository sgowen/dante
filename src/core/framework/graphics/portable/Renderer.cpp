//
//  Renderer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Renderer.h"

#include "SpriteBatcher.h"
#include "NGRectBatcher.h"
#include "LineBatcher.h"
#include "CircleBatcher.h"
#include "ITextureLoader.h"
#include "IRendererHelper.h"
#include "GpuProgramWrapper.h"
#include "Entity.h"
#include "TextureRegion.h"
#include "Color.h"
#include "NGRect.h"
#include "TextureWrapper.h"

#include "GpuTextureWrapper.h"
#include "GpuTextureDataWrapper.h"
#include "SpriteBatcherFactory.h"
#include "NGRectBatcherFactory.h"
#include "LineBatcherFactory.h"
#include "CircleBatcherFactory.h"
#include "TextureLoaderFactory.h"
#include "RendererHelperFactory.h"
#include "GpuProgramWrapperFactory.h"
#include "NGSTDUtil.h"

#include <Box2D/Box2D.h>

#include <string>
#include <assert.h>

Renderer::Renderer(int maxBatchSize) :
_spriteBatcher(SPRITE_BATCHER_FACTORY->createSpriteBatcher()),
_fillNGRectBatcher(RECTANGLE_BATCHER_FACTORY->createNGRectBatcher(true)),
_boundsNGRectBatcher(RECTANGLE_BATCHER_FACTORY->createNGRectBatcher(false)),
_lineBatcher(LINE_BATCHER_FACTORY->createLineBatcher()),
_circleBatcher(CIRCLE_BATCHER_FACTORY->createCircleBatcher()),
_textureLoader(TEXTURE_LOADER_FACTORY->createTextureLoader()),
_rendererHelper(RENDERER_HELPER_FACTORY->createRendererHelper()),
_textureGpuProgramWrapper(nullptr),
_colorGpuProgramWrapper(nullptr),
_framebufferToScreenGpuProgramWrapper(nullptr),
_framebufferIndex(0),
_maxBatchSize(maxBatchSize),
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
	_rendererHelper->createDeviceDependentResources(_maxBatchSize);

    _textureGpuProgramWrapper = GPU_PROGRAM_WRAPPER_FACTORY->createTextureGpuProgramWrapper();
    _colorGpuProgramWrapper = GPU_PROGRAM_WRAPPER_FACTORY->createColorGpuProgramWrapper();
    _framebufferToScreenGpuProgramWrapper = GPU_PROGRAM_WRAPPER_FACTORY->createFramebufferToScreenGpuProgramWrapper();
    
    _areDeviceDependentResourcesCreated = true;
}

void Renderer::createWindowSizeDependentResources(int renderWidth, int renderHeight, int numFramebuffers)
{
	_rendererHelper->createWindowSizeDependentResources(renderWidth, renderHeight, numFramebuffers);

	_areWindowSizeDependentResourcesCreated = true;
}

void Renderer::releaseDeviceDependentResources()
{
	_rendererHelper->releaseDeviceDependentResources();

    _areDeviceDependentResourcesCreated = false;
	_areWindowSizeDependentResourcesCreated = false;

	_loadingTextures.clear();

	cleanUpThreads();
    
    delete _textureGpuProgramWrapper;
	_textureGpuProgramWrapper = nullptr;

    delete _colorGpuProgramWrapper;
	_colorGpuProgramWrapper = nullptr;

    delete _framebufferToScreenGpuProgramWrapper;
	_framebufferToScreenGpuProgramWrapper = nullptr;
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
    _spriteBatcher->drawSprite(0, 0, 2, 2, 0, tr);
    _spriteBatcher->endBatch(*_rendererHelper->getFramebuffer(_framebufferIndex), *_framebufferToScreenGpuProgramWrapper);
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
    _spriteBatcher->renderSprite(pe.getPosition().x, pe.getPosition().y, pe.getWidth(), pe.getHeight(), pe.getAngle(), flipX, tr);
}

void Renderer::renderEntityWithColor(Entity &pe, TextureRegion& tr, Color c, bool flipX)
{
    _spriteBatcher->renderSprite(pe.getPosition().x, pe.getPosition().y, pe.getWidth(), pe.getHeight(), pe.getAngle(), flipX, c, tr);
}

void loadTextureDataSync(void* arg)
{
    assert(arg != nullptr);
    
    TextureWrapper* textureWrapper = reinterpret_cast<TextureWrapper*>(arg);
    
    assert(textureWrapper->_renderer != nullptr);
    assert(textureWrapper->name.length() > 0);
    
    if (textureWrapper->gpuTextureWrapper
        || !textureWrapper->_renderer->_areDeviceDependentResourcesCreated)
    {
        return;
    }
    
    textureWrapper->isLoadingData = true;
    textureWrapper->gpuTextureDataWrapper = textureWrapper->_renderer->_textureLoader->loadTextureData(textureWrapper->name.c_str());
}

void Renderer::loadTextureSync(TextureWrapper* textureWrapper)
{
    loadTextureDataSync(textureWrapper);
    
    textureWrapper->gpuTextureWrapper = _textureLoader->loadTexture(textureWrapper->gpuTextureDataWrapper, textureWrapper->repeatS);
    
    delete textureWrapper->gpuTextureDataWrapper;
    textureWrapper->gpuTextureDataWrapper = nullptr;
    
    textureWrapper->isLoadingData = false;
}

void Renderer::loadTextureAsync(TextureWrapper* textureWrapper)
{
    assert(textureWrapper != nullptr);
    assert(textureWrapper->name.length() > 0);
    
    if (textureWrapper->gpuTextureWrapper
        || textureWrapper->isLoadingData
        || !_areDeviceDependentResourcesCreated)
    {
        return;
    }
    
    _loadingTextures.push_back(textureWrapper);
    
    textureWrapper->isLoadingData = true;
    _textureDataLoadingThreads.push_back(new tthread::thread(loadTextureDataSync, textureWrapper));
}

void Renderer::unloadTexture(TextureWrapper* textureWrapper)
{
    if (textureWrapper == nullptr)
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
        textureWrapper->gpuTextureWrapper = nullptr;
    }
    
    if (textureWrapper->gpuTextureDataWrapper)
    {
        delete textureWrapper->gpuTextureDataWrapper;
        textureWrapper->gpuTextureDataWrapper = nullptr;
    }
    
    textureWrapper->isLoadingData = false;
}

bool Renderer::ensureTexture(TextureWrapper* textureWrapper)
{
    if (textureWrapper->gpuTextureWrapper == nullptr)
    {
        if (!textureWrapper->isLoadingData)
        {
            loadTextureAsync(textureWrapper);
        }
        
        return false;
    }
    
    return true;
}

#pragma mark private

void Renderer::handleAsyncTextureLoads()
{
    for (std::vector<TextureWrapper *>::iterator i = _loadingTextures.begin(); i != _loadingTextures.end(); )
    {
        if ((*i)->gpuTextureDataWrapper)
        {
            (*i)->gpuTextureWrapper = _textureLoader->loadTexture((*i)->gpuTextureDataWrapper, (*i)->repeatS);
            
            delete (*i)->gpuTextureDataWrapper;
            (*i)->gpuTextureDataWrapper = nullptr;
            
            (*i)->isLoadingData = false;
            
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
