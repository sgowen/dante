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

RendererHelper::RendererHelper() : _framebuffer(new NGTexture("framebuffer", NULL, false)), _screenWidth(1), _screenHeight(1), _renderWidth(1), _renderHeight(1)
{
    _textureVertices.reserve(MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
    _colorVertices.reserve(MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
    _screenVertices.reserve(4);
    
    generateIndices(MAX_BATCH_SIZE);
}

RendererHelper::~RendererHelper()
{
    _indices.clear();
}

void RendererHelper::createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight)
{
    _screenWidth = screenWidth;
    _screenHeight = screenHeight;
    _renderWidth = renderWidth;
    _renderHeight = renderHeight;
    
    releaseFramebuffers();
    createFramebufferObjects();
}

inline void mat4x4_identity(mat4x4 M)
{
    int i, j;
    for (i=0; i<4; ++i)
    {
        for (j=0; j<4; ++j)
        {
            M[i][j] = i==j ? 1.f : 0.f;
        }
    }
}

inline void mat4x4_ortho(mat4x4 M, float l, float r, float b, float t, float n, float f)
{
    M[0][0] = 2.f/(r-l);
    M[0][1] = M[0][2] = M[0][3] = 0.f;
    
    M[1][1] = 2.f/(t-b);
    M[1][0] = M[1][2] = M[1][3] = 0.f;
    
    M[2][2] = -2.f/(f-n);
    M[2][0] = M[2][1] = M[2][3] = 0.f;
    
    M[3][0] = -(r+l)/(r-l);
    M[3][1] = -(t+b)/(t-b);
    M[3][2] = -(f+n)/(f-n);
    M[3][3] = 1.f;
}

void RendererHelper::updateMatrix(float left, float right, float bottom, float top)
{
    mat4x4_identity(_matrix);
    
    mat4x4_ortho(_matrix, left, right, bottom, top, -1, 1);
}

void RendererHelper::unmapTextureVertices()
{
    // Unused
}

void RendererHelper::unmapColorVertices()
{
    // Unused
}

void RendererHelper::unmapScreenVertices()
{
    // Unused
}

void RendererHelper::clearTextureVertices()
{
    _textureVertices.clear();
}

void RendererHelper::clearColorVertices()
{
    _colorVertices.clear();
}

void RendererHelper::clearScreenVertices()
{
    _screenVertices.clear();
}

void RendererHelper::addVertexCoordinate(float x, float y, float r, float g, float b, float a, float u, float v)
{
    TEXTURE_VERTEX vertex(x, y, r, g, b, a, u, v);
    _textureVertices.push_back(vertex);
}

void RendererHelper::addVertexCoordinate(float x, float y, float r, float g, float b, float a)
{
    COLOR_VERTEX vertex(x, y, r, g, b, a);
    _colorVertices.push_back(vertex);
}

void RendererHelper::addVertexCoordinate(float x, float y)
{
    SCREEN_VERTEX vertex(x, y);
    _screenVertices.push_back(vertex);
}

std::vector<TEXTURE_VERTEX>& RendererHelper::getTextureVertices()
{
    return _textureVertices;
}

std::vector<COLOR_VERTEX>& RendererHelper::getColorVertices()
{
    return _colorVertices;
}

std::vector<SCREEN_VERTEX>& RendererHelper::getScreenVertices()
{
    return _screenVertices;
}

void RendererHelper::createFramebufferObjects()
{
    for (int i = 0; i < NUM_FRAMEBUFFERS; ++i)
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
