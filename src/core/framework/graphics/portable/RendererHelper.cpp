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

#include <framework/util/Constants.h>
#include <framework/util/NGSTDUtil.h>
#include <framework/util/MathUtil.h>

#include <algorithm>

RendererHelper::RendererHelper() : _screenWidth(1), _screenHeight(1)
{
    // Empty
}

RendererHelper::~RendererHelper()
{
    // Empty
}

void RendererHelper::createDeviceDependentResources()
{
    assert(_offscreenFramebufferWrappers.size() == 0);
    assert(_framebufferWrappers.size() == 0);
    
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
    disposeAllGPUBuffers(_dynamicTextureVertexBuffers);
    disposeAllGPUBuffers(_staticTextureVertexBuffers);
    disposeAllGPUBuffers(_dynamicVertexBuffers);
    disposeAllGPUBuffers(_staticVertexBuffers);
    
    disposeGPUBuffer(_indexBuffer);
    disposeGPUBuffer(_staticScreenVertexBuffer);
    
    releaseOffscreenFramebuffers();
    releaseFramebuffers();
}

void RendererHelper::createWindowSizeDependentResources(int screenWidth, int screenHeight)
{
    _screenWidth = screenWidth;
    _screenHeight = screenHeight;
    
    releaseOffscreenFramebuffers();
    releaseFramebuffers();
    
    for (int i = 0; i < static_cast<int>(_offscreenFramebufferDefs.size()); ++i)
    {
        _offscreenFramebufferWrappers.push_back(createOffscreenFramebuffer(_offscreenFramebufferDefs[i]));
    }
    
    for (int i = 0; i < static_cast<int>(_framebufferDefs.size()); ++i)
    {
        _framebufferWrappers.push_back(createFramebuffer(_framebufferDefs[i]));
    }
}

void RendererHelper::updateMatrix(float left, float right, float bottom, float top)
{
    mat4x4_identity(_matrix);
    mat4x4_ortho(_matrix, left, right, bottom, top, -1, 1);
}

NGTexture* RendererHelper::getOffscreenFramebuffer(int index)
{
    return _offscreenFramebufferWrappers[index];
}

NGTexture* RendererHelper::getFramebuffer(int index)
{
    return _framebufferWrappers[index];
}

void RendererHelper::addOffscreenFramebuffers(int renderWidth, int renderHeight, int numFramebuffers)
{
    _offscreenFramebufferDefs.clear();
    
    for (int i = 0; i < numFramebuffers; ++i)
    {
        _offscreenFramebufferDefs.push_back(FramebufferDef(renderWidth, renderHeight));
    }
}

int RendererHelper::addFramebuffer(int renderWidth, int renderHeight)
{
    int ret = static_cast<int>(_framebufferDefs.size());
    
    FramebufferDef framebufferDef = FramebufferDef(renderWidth, renderHeight);
    _framebufferDefs.push_back(framebufferDef);
    
    _framebufferWrappers.push_back(createFramebuffer(framebufferDef));
    
    return ret;
}

void RendererHelper::clearFramebuffers()
{
    _framebufferDefs.clear();
    
    releaseFramebuffers();
}

NGTexture* RendererHelper::createOffscreenFramebuffer(FramebufferDef framebufferDef)
{
    TextureWrapper* framebuffer = createOffscreenFramebuffer(framebufferDef.renderWidth, framebufferDef.renderHeight);
    NGTexture* texture = new NGTexture("framebuffer", NULL, NULL);
    texture->textureWrapper = framebuffer;
    
    return texture;
}

NGTexture* RendererHelper::createFramebuffer(FramebufferDef framebufferDef)
{
    TextureWrapper* framebuffer = createFramebuffer(framebufferDef.renderWidth, framebufferDef.renderHeight);
    NGTexture* texture = new NGTexture("framebuffer", NULL, NULL);
    texture->textureWrapper = framebuffer;
    
    return texture;
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

void RendererHelper::disposeAllGPUBuffers(std::vector<GPUBufferWrapper* >& buffers)
{
    for (std::vector<GPUBufferWrapper*>::iterator i = buffers.begin(); i != buffers.end(); ++i)
    {
        disposeGPUBuffer((*i));
    }
    
    NGSTDUtil::cleanUpVectorOfPointers(buffers);
}

void RendererHelper::releaseOffscreenFramebuffers()
{
    for (std::vector<NGTexture*>::iterator i = _offscreenFramebufferWrappers.begin(); i != _offscreenFramebufferWrappers.end(); ++i)
    {
        NGTexture* texture = (*i);
        delete texture->textureWrapper;
    }
    NGSTDUtil::cleanUpVectorOfPointers(_offscreenFramebufferWrappers);
    platformReleaseOffscreenFramebuffers();
}

void RendererHelper::releaseFramebuffers()
{
    for (std::vector<NGTexture*>::iterator i = _framebufferWrappers.begin(); i != _framebufferWrappers.end(); ++i)
    {
        NGTexture* texture = (*i);
        delete texture->textureWrapper;
    }
    NGSTDUtil::cleanUpVectorOfPointers(_framebufferWrappers);
    platformReleaseFramebuffers();
}

