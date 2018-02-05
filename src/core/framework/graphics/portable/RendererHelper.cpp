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
#include <framework/util/NGSTDUtil.h>
#include <framework/util/MathUtil.h>

RendererHelper::RendererHelper() : _screenWidth(1), _screenHeight(1), _renderWidth(1), _renderHeight(1)
{
    // Empty
}

RendererHelper::~RendererHelper()
{
    releaseFramebuffers();
}

void RendererHelper::createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight)
{
    _screenWidth = screenWidth;
    _screenHeight = screenHeight;
    _renderWidth = renderWidth;
    _renderHeight = renderHeight;
    
    releaseFramebuffers();
    platformReleaseFramebuffers();
    
    for (int i = 0; i < NUM_FRAMEBUFFERS; ++i)
    {
        TextureWrapper* framebuffer = createFramebuffer();
        NGTexture* texture = new NGTexture("framebuffer", NULL, false);
        texture->textureWrapper = framebuffer;
        _framebufferWrappers.push_back(texture);
    }
}

void RendererHelper::updateMatrix(float left, float right, float bottom, float top)
{
    mat4x4_identity(_matrix);
    mat4x4_ortho(_matrix, left, right, bottom, top, -1, 1);
}

NGTexture* RendererHelper::getFramebuffer(int index)
{
    return _framebufferWrappers[index];
}

void RendererHelper::releaseFramebuffers()
{
    for (std::vector<NGTexture*>::iterator i = _framebufferWrappers.begin(); i != _framebufferWrappers.end(); ++i)
    {
        NGTexture* texture = (*i);
        delete texture->textureWrapper;
    }
    
    NGSTDUtil::cleanUpVectorOfPointers(_framebufferWrappers);
}
