//
//  RendererHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/RendererHelper.h"

#include "framework/graphics/portable/TextureWrapper.h"
#include "framework/graphics/portable/GpuTextureWrapper.h"

#include <framework/util/FrameworkConstants.h>

RendererHelper::RendererHelper() : _framebuffer(new TextureWrapper("framebuffer", NULL, false)), _screenWidth(1), _screenHeight(1), _renderWidth(1), _renderHeight(1), _numFramebuffers(1)
{
    generateIndices(MAX_BATCH_SIZE);
}

RendererHelper::~RendererHelper()
{
    _indices.clear();
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
