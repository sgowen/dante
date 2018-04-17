//
//  RendererHelper.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/RendererHelper.h>

#include <framework/graphics/portable/NGTexture.h>
#include <framework/graphics/portable/TextureWrapper.h>
#include <framework/graphics/portable/FramebufferWrapper.h>

#include <framework/graphics/portable/NGTextureDesc.h>
#include <framework/util/Constants.h>
#include <framework/util/NGSTDUtil.h>
#include <framework/util/MathUtil.h>
#include <framework/util/Config.h>

#include <algorithm>

RendererHelper::RendererHelper() :
_screenFramebufferWrapper(new FramebufferWrapper(1, 1)),
_indexBuffer(NULL),
_staticScreenVertexBuffer(NULL)
{
    // Empty
}

RendererHelper::~RendererHelper()
{
    delete _screenFramebufferWrapper;
}

void RendererHelper::createDeviceDependentResources()
{
    assert(_offscreenFramebufferWrappers.size() == 0);
    assert(_framebufferWrappers.size() == 0);
    
    assert(_dynamicTextureVertexBuffers.size() == 0);
    assert(_staticTextureVertexBuffers.size() == 0);
    assert(_dynamicVertexBuffers.size() == 0);
    assert(_staticVertexBuffers.size() == 0);
    
    createScreenFramebufferWrapper(_screenFramebufferWrapper);
    
    int fbWidth = FW_CFG->getInt("FramebufferWidth");
    int fbHeight = FW_CFG->getInt("FramebufferHeight");
    std::string cfgFilterMin = FW_CFG->getString("FramebufferFilterMin");
    std::string cfgFilterMag = FW_CFG->getString("FramebufferFilterMag");
    for (int i = 0; i < NUM_OFFSCREEN_FRAMEBUFFERS; ++i)
    {
        FramebufferWrapper* fbw = new FramebufferWrapper(fbWidth, fbHeight);
        createFramebuffer(fbw, cfgFilterMin, cfgFilterMag);
        _offscreenFramebufferWrappers.push_back(fbw);
    }
    
    {
        std::vector<VERTEX_2D_TEXTURE> vertices(MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
        std::fill(vertices.begin(), vertices.end(), VERTEX_2D_TEXTURE());
        for (int i = 0; i < NUM_TEXTURE_VERTEX_BUFFERS; ++i)
        {
            size_t size = sizeof(VERTEX_2D_TEXTURE) * vertices.size();
            _dynamicTextureVertexBuffers.push_back(createGPUBufferImpl(size, &vertices[0], true, true));
            _staticTextureVertexBuffers.push_back(createGPUBufferImpl(size, &vertices[0], false, true));
        }
    }
    
    {
        std::vector<VERTEX_2D> vertices(MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
        std::fill(vertices.begin(), vertices.end(), VERTEX_2D());
        for (int i = 0; i < NUM_VERTEX_BUFFERS; ++i)
        {
            size_t size = sizeof(VERTEX_2D) * vertices.size();
            _dynamicVertexBuffers.push_back(createGPUBufferImpl(size, &vertices[0], true, true));
            _staticVertexBuffers.push_back(createGPUBufferImpl(size, &vertices[0], false, true));
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
    
    destroyGPUBufferImpl(_indexBuffer);
    delete _indexBuffer;
    
    destroyGPUBufferImpl(_staticScreenVertexBuffer);
    delete _staticScreenVertexBuffer;
}

void RendererHelper::createWindowSizeDependentResources(int screenWidth, int screenHeight)
{
    _screenFramebufferWrapper->renderWidth = screenWidth;
    _screenFramebufferWrapper->renderHeight = screenHeight;
}

void RendererHelper::bindMatrix(NGShaderUniformInput* uniform)
{
    bindMatrix(uniform, _matrix);
}

void RendererHelper::updateMatrix(float left, float right, float bottom, float top)
{
    mat4x4_identity(_matrix);
    mat4x4_ortho(_matrix, left, right, bottom, top, -1, 1);
}

void RendererHelper::bindFramebuffer(FramebufferWrapper* fb)
{
    bindFramebufferImpl(fb);
    
    bindViewport(fb->renderWidth, fb->renderHeight);
}

void RendererHelper::bindScreenFramebuffer()
{
    bindFramebuffer(_screenFramebufferWrapper);
}

FramebufferWrapper* RendererHelper::addFramebuffer(std::string name, int renderWidth, int renderHeight, std::string textureFilterMin, std::string textureFilterMag)
{
    auto q = _framebufferWrappers.find(name);
    assert(q == _framebufferWrappers.end());
    
    FramebufferWrapper* fbw = new FramebufferWrapper(renderWidth, renderHeight);
    createFramebuffer(fbw, textureFilterMin, textureFilterMag);
    
    _framebufferWrappers.insert(std::make_pair(name, fbw));
    
    return fbw;
}

void RendererHelper::removeFramebuffer(std::string name)
{
    auto q = _framebufferWrappers.find(name);
    assert(q != _framebufferWrappers.end());
    
    FramebufferWrapper* fbw = q->second;
    assert(fbw);
    
    destroyFramebuffer(fbw);
    
    delete fbw;
    
    _framebufferWrappers.erase(q);
}

void RendererHelper::releaseFramebuffers()
{
    for (std::map<std::string, FramebufferWrapper*>::iterator i = _framebufferWrappers.begin(); i != _framebufferWrappers.end(); ++i)
    {
        destroyFramebuffer(i->second);
    }
    NGSTDUtil::cleanUpMapOfPointers(_framebufferWrappers);
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

void RendererHelper::createFramebuffer(FramebufferWrapper* fbw, std::string textureFilterMin, std::string textureFilterMag)
{
    NGTextureDesc* td = new NGTextureDesc("framebuffer", 0, false, false, false, textureFilterMin, textureFilterMag, false);
    fbw->texture = new NGTexture("framebuffer", td);
    fbw->texture->_textureWrapper = createFramebufferImpl(fbw);
}

void RendererHelper::destroyFramebuffer(FramebufferWrapper* fbw)
{
    destroyFramebufferImpl(fbw);
    
    NGTexture* texture = fbw->texture;
    delete texture->_desc;
    delete texture->_textureWrapper;
    delete texture;
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
    
    _indexBuffer = createGPUBufferImpl(sizeof(uint16_t) * indices.size(), &indices[0], false, false);
}

void RendererHelper::createStaticScreenVertexBuffer()
{
    std::vector<VERTEX_2D> vertices;
    vertices.reserve(4);
    
    vertices.push_back(VERTEX_2D(-1, -1));
    vertices.push_back(VERTEX_2D(-1, 1));
    vertices.push_back(VERTEX_2D(1, 1));
    vertices.push_back(VERTEX_2D(1, -1));
    
    _staticScreenVertexBuffer = createGPUBufferImpl(sizeof(VERTEX_2D) * vertices.size(), &vertices[0], false, true);
}

void RendererHelper::destroyAllGPUBuffers(std::vector<GPUBufferWrapper* >& buffers)
{
    for (std::vector<GPUBufferWrapper*>::iterator i = buffers.begin(); i != buffers.end(); ++i)
    {
        destroyGPUBufferImpl((*i));
    }
    NGSTDUtil::cleanUpVectorOfPointers(buffers);
}

void RendererHelper::releaseOffscreenFramebuffers()
{
    for (std::vector<FramebufferWrapper*>::iterator i = _offscreenFramebufferWrappers.begin(); i != _offscreenFramebufferWrappers.end(); ++i)
    {
        destroyFramebuffer(*i);
    }
    NGSTDUtil::cleanUpVectorOfPointers(_offscreenFramebufferWrappers);
}
