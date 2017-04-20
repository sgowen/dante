//
//  Renderer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "Renderer.h"

#include "SpriteBatcher.h"
#include "NGRectBatcher.h"
#include "LineBatcher.h"
#include "CircleBatcher.h"
#include "ITextureLoader.h"
#include "IRendererHelper.h"
#include "GpuProgramWrapper.h"
#include "PhysicalEntity.h"
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

#include <string>
#include <assert.h>

Renderer::Renderer(int maxBatchSize) :
m_spriteBatcher(SPRITE_BATCHER_FACTORY->createSpriteBatcher()),
m_fillNGRectBatcher(RECTANGLE_BATCHER_FACTORY->createNGRectBatcher(true)),
m_boundsNGRectBatcher(RECTANGLE_BATCHER_FACTORY->createNGRectBatcher(false)),
m_lineBatcher(LINE_BATCHER_FACTORY->createLineBatcher()),
m_circleBatcher(CIRCLE_BATCHER_FACTORY->createCircleBatcher()),
m_textureLoader(TEXTURE_LOADER_FACTORY->createTextureLoader()),
m_rendererHelper(RENDERER_HELPER_FACTORY->createRendererHelper()),
m_textureGpuProgramWrapper(nullptr),
m_colorGpuProgramWrapper(nullptr),
m_framebufferToScreenGpuProgramWrapper(nullptr),
m_iFramebufferIndex(0),
m_iMaxBatchSize(maxBatchSize),
m_areDeviceDependentResourcesCreated(false)
{
    // Empty
}

Renderer::~Renderer()
{
    delete m_spriteBatcher;
    delete m_fillNGRectBatcher;
    delete m_boundsNGRectBatcher;
    delete m_lineBatcher;
    delete m_circleBatcher;
    
    delete m_textureLoader;
    delete m_rendererHelper;
}

void Renderer::createDeviceDependentResources()
{
	m_rendererHelper->createDeviceDependentResources(m_iMaxBatchSize);

    m_textureGpuProgramWrapper = GPU_PROGRAM_WRAPPER_FACTORY->createTextureGpuProgramWrapper();
    m_colorGpuProgramWrapper = GPU_PROGRAM_WRAPPER_FACTORY->createColorGpuProgramWrapper();
    m_framebufferToScreenGpuProgramWrapper = GPU_PROGRAM_WRAPPER_FACTORY->createFramebufferToScreenGpuProgramWrapper();
    
    m_areDeviceDependentResourcesCreated = true;
}

void Renderer::createWindowSizeDependentResources(int renderWidth, int renderHeight, int numFramebuffers)
{
	m_rendererHelper->createWindowSizeDependentResources(renderWidth, renderHeight, numFramebuffers);
}

void Renderer::releaseDeviceDependentResources()
{
	m_rendererHelper->releaseDeviceDependentResources();

    m_areDeviceDependentResourcesCreated = false;

	m_loadingTextures.clear();

	cleanUpThreads();
    
    delete m_textureGpuProgramWrapper;
	m_textureGpuProgramWrapper = nullptr;

    delete m_colorGpuProgramWrapper;
	m_colorGpuProgramWrapper = nullptr;

    delete m_framebufferToScreenGpuProgramWrapper;
	m_framebufferToScreenGpuProgramWrapper = nullptr;
}

void Renderer::beginFrame()
{
    handleAsyncTextureLoads();
    
    m_rendererHelper->beginFrame();
    
    setFramebuffer(0);
}

void Renderer::setFramebuffer(int framebufferIndex)
{
    assert(framebufferIndex >= 0);
    
    m_iFramebufferIndex = framebufferIndex;
    
    m_rendererHelper->bindToOffscreenFramebuffer(m_iFramebufferIndex);
    m_rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
}

void Renderer::renderToScreen()
{
    assert(m_iFramebufferIndex >= 0);
    
    m_rendererHelper->bindToScreenFramebuffer();
    m_rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
    
    static TextureRegion tr = TextureRegion("framebuffer", 0, 0, 1, 1, 1, 1);
    
    m_spriteBatcher->beginBatch();
    m_spriteBatcher->drawSprite(0, 0, 2, 2, 0, tr);
    m_spriteBatcher->endBatch(*m_rendererHelper->getFramebuffer(m_iFramebufferIndex), *m_framebufferToScreenGpuProgramWrapper);
}

void Renderer::endFrame()
{
    m_rendererHelper->endFrame();
}

bool Renderer::isLoadingData()
{
    return m_loadingTextures.size() > 0;
}

#pragma mark protected

void Renderer::renderPhysicalEntity(PhysicalEntity &pe, TextureRegion& tr)
{
    m_spriteBatcher->drawSprite(pe.getPosition().getX(), pe.getPosition().getY(), pe.getWidth(), pe.getHeight(), pe.getAngle(), tr);
}

void Renderer::renderPhysicalEntityWithColor(PhysicalEntity &pe, TextureRegion& tr, Color c)
{
    m_spriteBatcher->drawSprite(pe.getPosition().getX(), pe.getPosition().getY(), pe.getWidth(), pe.getHeight(), pe.getAngle(), c, tr);
}

void Renderer::renderBoundsForPhysicalEntity(PhysicalEntity &pe)
{
    static Color red = Color(1, 0, 0, 1);
    
    renderBoundsWithColor(pe.getMainBounds(), red);
}

void Renderer::renderBoundsWithColor(NGRect &r, Color& c)
{
    m_boundsNGRectBatcher->renderNGRect(r, c);
}

void Renderer::renderHighlightForPhysicalEntity(PhysicalEntity &pe, Color &c)
{
    m_fillNGRectBatcher->renderNGRect(pe.getMainBounds(), c);
}

void Renderer::loadTextureSync(TextureWrapper* textureWrapper)
{
    assert(textureWrapper != nullptr);
    assert(textureWrapper->name.length() > 0);
    
    if (textureWrapper->gpuTextureWrapper
        || !m_areDeviceDependentResourcesCreated)
    {
        return;
    }
    
    textureWrapper->isLoadingData = true;
    textureWrapper->gpuTextureDataWrapper = m_textureLoader->loadTextureData(textureWrapper->name.c_str());
    
    textureWrapper->gpuTextureWrapper = m_textureLoader->loadTexture(textureWrapper->gpuTextureDataWrapper);
    
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
        || !m_areDeviceDependentResourcesCreated)
    {
        return;
    }
    
    m_loadingTextures.push_back(textureWrapper);
    
    textureWrapper->isLoadingData = true;
    m_textureDataLoadingThreads.push_back(new std::thread([](TextureWrapper* tw, Renderer* r)
    {
        if (r->m_areDeviceDependentResourcesCreated)
        {
            tw->gpuTextureDataWrapper = r->m_textureLoader->loadTextureData(tw->name.c_str());
        }
    }, textureWrapper, this));
}

void Renderer::unloadTexture(TextureWrapper* textureWrapper)
{
    if (textureWrapper == nullptr)
    {
        return;
    }
    
    for (std::vector<TextureWrapper *>::iterator i = m_loadingTextures.begin(); i != m_loadingTextures.end(); )
    {
        if ((*i) == textureWrapper)
        {
            i = m_loadingTextures.erase(i);
        }
        else
        {
            ++i;
        }
    }
    
    if (textureWrapper->gpuTextureWrapper)
    {
        m_rendererHelper->destroyTexture(*textureWrapper->gpuTextureWrapper);
        
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

void Renderer::handleAsyncTextureLoads()
{
    for (std::vector<TextureWrapper *>::iterator i = m_loadingTextures.begin(); i != m_loadingTextures.end(); )
    {
        if ((*i)->gpuTextureDataWrapper)
        {
            (*i)->gpuTextureWrapper = m_textureLoader->loadTexture((*i)->gpuTextureDataWrapper, (*i)->repeatS);
            
            delete (*i)->gpuTextureDataWrapper;
            (*i)->gpuTextureDataWrapper = nullptr;
            
            (*i)->isLoadingData = false;
            
            i = m_loadingTextures.erase(i);
        }
        else
        {
            ++i;
        }
    }
    
    if (m_loadingTextures.size() == 0)
    {
        cleanUpThreads();
    }
}

void Renderer::cleanUpThreads()
{
    for (std::vector<std::thread *>::iterator i = m_textureDataLoadingThreads.begin(); i != m_textureDataLoadingThreads.end(); ++i)
    {
        (*i)->join();
    }
    
    NGSTDUtil::cleanUpVectorOfPointers(m_textureDataLoadingThreads);
}
