//
//  RendererHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/RendererHelper.h"

#include "framework/graphics/portable/NGTexture.h"
#include "framework/graphics/portable/TextureWrapper.h"

#include <framework/util/FrameworkConstants.h>

RendererHelper::RendererHelper() : _framebuffer(new NGTexture("framebuffer", NULL, false)), _screenWidth(1), _screenHeight(1), _renderWidth(1), _renderHeight(1), _numFramebuffers(1)
{
    _screenVertices.reserve(4);
    _textureVertices.reserve(MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
    _colorVertices.reserve(MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
    generateIndices(MAX_BATCH_SIZE);
}

RendererHelper::~RendererHelper()
{
    _indices.clear();
}

void RendererHelper::createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int numFramebuffers)
{
    _screenWidth = screenWidth;
    _screenHeight = screenHeight;
    _renderWidth = renderWidth;
    _renderHeight = renderHeight;
    _numFramebuffers = numFramebuffers;
    
    releaseFramebuffers();
    createFramebufferObjects();
}

void RendererHelper::releaseDeviceDependentResources()
{
    releaseFramebuffers();
    
    _textureVertices.clear();
    _colorVertices.clear();
}

void RendererHelper::clearScreenVertices()
{
    _screenVertices.clear();
}

void RendererHelper::clearTextureVertices()
{
    _textureVertices.clear();
}

void RendererHelper::clearColorVertices()
{
    _colorVertices.clear();
}

void RendererHelper::addVertexCoordinate(float x, float y)
{
    SCREEN_VERTEX vertex = { x, y };
    _screenVertices.push_back(vertex);
}

void RendererHelper::addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a, float u, float v)
{
    TEXTURE_VERTEX vertex = { x, y, z, r, g, b, a, u, v };
    _textureVertices.push_back(vertex);
}

void RendererHelper::addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a)
{
    COLOR_VERTEX vertex = { x, y, z, r, g, b, a };
    _colorVertices.push_back(vertex);
}

std::vector<SCREEN_VERTEX>& RendererHelper::getScreenVertices()
{
    return _screenVertices;
}

std::vector<TEXTURE_VERTEX>& RendererHelper::getTextureVertices()
{
    return _textureVertices;
}

std::vector<COLOR_VERTEX>& RendererHelper::getColorVertices()
{
    return _colorVertices;
}

void RendererHelper::createFramebufferObjects()
{
    for (int i = 0; i < _numFramebuffers; ++i)
    {
        createFramebufferObject();
    }
}

void RendererHelper::generateIndices(int maxBatchSize)
{
    _indices.reserve(maxBatchSize * INDICES_PER_RECTANGLE);
    
    uint16_t j = 0;
    for (int i = 0; i < maxBatchSize * INDICES_PER_RECTANGLE; i += INDICES_PER_RECTANGLE, j += VERTICES_PER_RECTANGLE)
    {
        _indices.push_back(j);
        _indices.push_back(j + 1);
        _indices.push_back(j + 2);
        _indices.push_back(j + 2);
        _indices.push_back(j + 3);
        _indices.push_back(j + 0);
    }
}
