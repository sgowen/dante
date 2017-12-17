//
//  SpriteBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/25/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/SpriteBatcher.h"

#include "framework/graphics/portable/RendererHelper.h"
#include "framework/graphics/portable/TextureWrapper.h"
#include "framework/graphics/portable/GpuProgram.h"
#include "framework/graphics/portable/TextureRegion.h"
#include "framework/math/Color.h"

#include <framework/util/FrameworkConstants.h>

#include <math.h>

SpriteBatcher::SpriteBatcher(RendererHelper* inRendererHelper) : _rendererHelper(inRendererHelper), _numSprites(0)
{
    // Empty
}

SpriteBatcher::~SpriteBatcher()
{
    // Empty
}

void SpriteBatcher::beginBatch()
{
    _rendererHelper->clearTextureVertices();
    
    _numSprites = 0;
}

void SpriteBatcher::endBatch(TextureWrapper* textureWrapper, GpuProgram& gpuProgramWrapper)
{
    if (_numSprites > 0)
    {
        gpuProgramWrapper.bind(textureWrapper);
        
        _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, _numSprites * INDICES_PER_RECTANGLE);
        
        gpuProgramWrapper.unbind();
    }
}

void SpriteBatcher::renderSprite(float x, float y, float width, float height, float angle, TextureRegion& tr, bool flipX)
{
    renderSprite(x, y, width, height, angle, Color::WHITE, tr, flipX);
}

void SpriteBatcher::renderSprite(float x, float y, float width, float height, float angle, Color &c, TextureRegion& tr, bool flipX)
{
    if (angle != 0)
    {
        float halfWidth = width / 2;
        float halfHeight = height / 2;
        
        float rad = angle;
        float cos = cosf(rad);
        float sin = sinf(rad);
        
        float x1 = -halfWidth * cos - (-halfHeight) * sin;
        float y1 = -halfWidth * sin + (-halfHeight) * cos;
        
        float x2 = halfWidth * cos - (-halfHeight) * sin;
        float y2 = halfWidth * sin + (-halfHeight) * cos;
        
        float x3 = halfWidth * cos - halfHeight * sin;
        float y3 = halfWidth * sin + halfHeight * cos;
        
        float x4 = -halfWidth * cos - halfHeight * sin;
        float y4 = -halfWidth * sin + halfHeight * cos;
        
        x1 += x;
        y1 += y;
        
        x2 += x;
        y2 += y;
        
        x3 += x;
        y3 += y;
        
        x4 += x;
        y4 += y;
        
        _rendererHelper->addVertexCoordinate(x1, y1, 0, c.red, c.green, c.blue, c.alpha, flipX ? tr.u2 : tr.u1, tr.v2);
        _rendererHelper->addVertexCoordinate(x4, y4, 0, c.red, c.green, c.blue, c.alpha, flipX ? tr.u2 : tr.u1, tr.v1);
        _rendererHelper->addVertexCoordinate(x3, y3, 0, c.red, c.green, c.blue, c.alpha, flipX ? tr.u1 : tr.u2, tr.v1);
        _rendererHelper->addVertexCoordinate(x2, y2, 0, c.red, c.green, c.blue, c.alpha, flipX ? tr.u1 : tr.u2, tr.v2);
    }
    else
    {
        float halfWidth = width / 2;
        float halfHeight = height / 2;
        float x1 = x - halfWidth;
        float y1 = y - halfHeight;
        float x2 = x + halfWidth;
        float y2 = y + halfHeight;
        
        _rendererHelper->addVertexCoordinate(x1, y1, 0, c.red, c.green, c.blue, c.alpha, flipX ? tr.u2 : tr.u1, tr.v2);
        _rendererHelper->addVertexCoordinate(x1, y2, 0, c.red, c.green, c.blue, c.alpha, flipX ? tr.u2 : tr.u1, tr.v1);
        _rendererHelper->addVertexCoordinate(x2, y2, 0, c.red, c.green, c.blue, c.alpha, flipX ? tr.u1 : tr.u2, tr.v1);
        _rendererHelper->addVertexCoordinate(x2, y1, 0, c.red, c.green, c.blue, c.alpha, flipX ? tr.u1 : tr.u2, tr.v2);
    }
    
    _numSprites++;
}

