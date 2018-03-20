//
//  RendererHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/RendererHelper.h>

#include <framework/graphics/portable/NGTexture.h>
#include <framework/graphics/portable/TextureWrapper.h>
#include <framework/graphics/portable/FramebufferWrapper.h>

#include <framework/util/Constants.h>
#include <framework/util/NGSTDUtil.h>
#include <framework/util/MathUtil.h>
#include <framework/util/Config.h>

#include <algorithm>

RendererHelper::RendererHelper() :
_indexBuffer(NULL),
_staticScreenVertexBuffer(NULL),
_screenWidth(1),
_screenHeight(1)
{
    // Empty
}

RendererHelper::~RendererHelper()
{
    NGSTDUtil::cleanUpVectorOfPointers(_offscreenFramebufferWrappers);
    NGSTDUtil::cleanUpMapOfPointers(_framebufferWrappers);
}

void RendererHelper::createDeviceDependentResources()
{
    assert(_indexBuffer == NULL);
    assert(_staticScreenVertexBuffer == NULL);
    
    {
        std::vector<VERTEX_2D_TEXTURE> vertices(MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
        std::fill(vertices.begin(), vertices.end(), VERTEX_2D_TEXTURE());
        for (int i = 0; i < NUM_TEXTURE_VERTEX_BUFFERS; ++i)
        {
            size_t size = sizeof(VERTEX_2D_TEXTURE) * vertices.size();
            _dynamicTextureVertexBuffers.push_back(createGPUBuffer(size, &vertices[0], true, true));
            _staticTextureVertexBuffers.push_back(createGPUBuffer(size, &vertices[0], false, true));
        }
    }
    
    {
        std::vector<VERTEX_2D> vertices(MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
        std::fill(vertices.begin(), vertices.end(), VERTEX_2D());
        for (int i = 0; i < NUM_VERTEX_BUFFERS; ++i)
        {
            size_t size = sizeof(VERTEX_2D) * vertices.size();
            _dynamicVertexBuffers.push_back(createGPUBuffer(size, &vertices[0], true, true));
            _staticVertexBuffers.push_back(createGPUBuffer(size, &vertices[0], false, true));
        }
    }
    
    createIndexBuffer();
    createStaticScreenVertexBuffer();
}

void RendererHelper::releaseDeviceDependentResources()
{
    releaseOffscreenFramebuffers();
    releaseFramebuffers();
    
    destroyAllGPUBuffers(_dynamicTextureVertexBuffers);
    destroyAllGPUBuffers(_staticTextureVertexBuffers);
    destroyAllGPUBuffers(_dynamicVertexBuffers);
    destroyAllGPUBuffers(_staticVertexBuffers);
    
    destroyGPUBuffer(_indexBuffer);
    delete _indexBuffer;
    _indexBuffer = NULL;
    
    destroyGPUBuffer(_staticScreenVertexBuffer);
    delete _staticScreenVertexBuffer;
    _staticScreenVertexBuffer = NULL;
}

void RendererHelper::createWindowSizeDependentResources(int screenWidth, int screenHeight)
{
    _screenWidth = screenWidth;
    _screenHeight = screenHeight;
    
    releaseOffscreenFramebuffers();
    releaseFramebuffers();
    
    for (int i = 0; i < NUM_OFFSCREEN_FRAMEBUFFERS; ++i)
    {
        FramebufferWrapper* framebufferWrapper = new FramebufferWrapper(FW_CFG->getInt("FramebufferWidth"), FW_CFG->getInt("FramebufferHeight"));
        createFramebuffer(framebufferWrapper);
        _offscreenFramebufferWrappers.push_back(framebufferWrapper);
    }
    
    for (std::map<std::string, FramebufferWrapper* >::iterator i = _framebufferWrappers.begin(); i != _framebufferWrappers.end(); ++i)
    {
        FramebufferWrapper* framebufferWrapper = i->second;
        createFramebuffer(framebufferWrapper);
    }
}

void RendererHelper::updateMatrix(float left, float right, float bottom, float top)
{
    mat4x4_identity(_matrix);
    mat4x4_ortho(_matrix, left, right, bottom, top, -1, 1);
}

FramebufferWrapper* RendererHelper::getOffscreenFramebuffer(int fbIndex)
{
    return _offscreenFramebufferWrappers[fbIndex];
}

FramebufferWrapper* RendererHelper::getFramebuffer(std::string name)
{
    auto q = _framebufferWrappers.find(name);
    assert(q != _framebufferWrappers.end());
    
    FramebufferWrapper* ret = q->second;
    assert(ret);
    
    return ret;
}

FramebufferWrapper* RendererHelper::addFramebuffer(int renderWidth, int renderHeight, std::string name)
{
    auto q = _framebufferWrappers.find(name);
    assert(q == _framebufferWrappers.end());
    
    FramebufferWrapper* framebufferWrapper = new FramebufferWrapper(renderWidth, renderHeight);
    createFramebuffer(framebufferWrapper);
    
    _framebufferWrappers.insert(std::make_pair(name, framebufferWrapper));
    
    return framebufferWrapper;
}

void RendererHelper::removeFramebuffer(std::string name)
{
    auto q = _framebufferWrappers.find(name);
    assert(q != _framebufferWrappers.end());
    
    FramebufferWrapper* framebufferWrapper = q->second;
    assert(framebufferWrapper);
    
    destroyFramebuffer(framebufferWrapper);
    
    delete framebufferWrapper;
    
    _framebufferWrappers.erase(q);
}

void RendererHelper::clearFramebuffers()
{
    releaseFramebuffers();
    
    NGSTDUtil::cleanUpMapOfPointers(_framebufferWrappers);
}

void RendererHelper::createFramebuffer(FramebufferWrapper* framebufferWrapper)
{
    TextureWrapper* framebuffer = platformCreateFramebuffer(framebufferWrapper);
    framebufferWrapper->texture->textureWrapper = framebuffer;
}

void RendererHelper::createIndexBuffer()
{
    size_t size = MAX_BATCH_SIZE * INDICES_PER_RECTANGLE;
    std::vector<uint16_t> indices;
    indices.reserve(size);
    
    uint16_t j = 0;
    for (int i = 0; i < size; i += INDICES_PER_RECTANGLE, j += VERTICES_PER_RECTANGLE)
    {
        indices.push_back(j);
        indices.push_back(j + 1);
        indices.push_back(j + 2);
        indices.push_back(j + 2);
        indices.push_back(j + 3);
        indices.push_back(j + 0);
    }
    
    _indexBuffer = createGPUBuffer(sizeof(uint16_t) * indices.size(), &indices[0], false, false);
}

void RendererHelper::createStaticScreenVertexBuffer()
{
    std::vector<VERTEX_2D> vertices;
    vertices.reserve(4);
    
    vertices.push_back(VERTEX_2D(-1, -1));
    vertices.push_back(VERTEX_2D(-1, 1));
    vertices.push_back(VERTEX_2D(1, 1));
    vertices.push_back(VERTEX_2D(1, -1));
    
    _staticScreenVertexBuffer = createGPUBuffer(sizeof(VERTEX_2D) * vertices.size(), &vertices[0], false, true);
}

void RendererHelper::destroyAllGPUBuffers(std::vector<GPUBufferWrapper* >& buffers)
{
    for (std::vector<GPUBufferWrapper*>::iterator i = buffers.begin(); i != buffers.end(); ++i)
    {
        destroyGPUBuffer((*i));
    }
    
    NGSTDUtil::cleanUpVectorOfPointers(buffers);
}

void RendererHelper::releaseOffscreenFramebuffers()
{
    for (std::vector<FramebufferWrapper*>::iterator i = _offscreenFramebufferWrappers.begin(); i != _offscreenFramebufferWrappers.end(); ++i)
    {
        FramebufferWrapper* fb = (*i);
        destroyFramebuffer(fb);
        NGTexture* texture = fb->texture;
        delete texture->textureWrapper;
        delete texture;
    }
    NGSTDUtil::cleanUpVectorOfPointers(_offscreenFramebufferWrappers);
}

void RendererHelper::releaseFramebuffers()
{
    for (std::map<std::string, FramebufferWrapper*>::iterator i = _framebufferWrappers.begin(); i != _framebufferWrappers.end(); ++i)
    {
        FramebufferWrapper* fb = i->second;
        destroyFramebuffer(fb);
        NGTexture* texture = fb->texture;
        delete texture->textureWrapper;
    }
}
