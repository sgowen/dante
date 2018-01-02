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
#include "framework/graphics/portable/NGTexture.h"
#include "framework/graphics/portable/NGShader.h"
#include "framework/graphics/portable/TextureRegion.h"
#include "framework/graphics/portable/Color.h"

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

void SpriteBatcher::renderSprite(float x, float y, float width, float height, float angle, TextureRegion& tr, bool flipX)
{
    renderSprite(x, y, width, height, angle, Color::WHITE, tr, flipX);
}

void SpriteBatcher::renderSprite(float x, float y, float width, float height, float angle, Color &c, TextureRegion& tr, bool flipX)
{
    float xFinal[4], yFinal[4], uFinal[4];
    
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
        
        xFinal[0] = x1;
        yFinal[0] = y1;
        xFinal[1] = x4;
        yFinal[1] = y4;
        xFinal[2] = x3;
        yFinal[2] = y3;
        xFinal[3] = x2;
        yFinal[3] = y2;
    }
    else
    {
        float halfWidth = width / 2;
        float halfHeight = height / 2;
        float x1 = x - halfWidth;
        float y1 = y - halfHeight;
        float x2 = x + halfWidth;
        float y2 = y + halfHeight;
        
        xFinal[0] = x1;
        yFinal[0] = y1;
        xFinal[1] = x1;
        yFinal[1] = y2;
        xFinal[2] = x2;
        yFinal[2] = y2;
        xFinal[3] = x2;
        yFinal[3] = y1;
    }
    
    if (flipX)
    {
        uFinal[0] = tr._u2;
        uFinal[1] = tr._u2;
        uFinal[2] = tr._u1;
        uFinal[3] = tr._u1;
    }
    else
    {
        uFinal[0] = tr._u1;
        uFinal[1] = tr._u1;
        uFinal[2] = tr._u2;
        uFinal[3] = tr._u2;
    }
    
    _rendererHelper->addVertexCoordinate(xFinal[0], yFinal[0], 0, c.red, c.green, c.blue, c.alpha, uFinal[0], tr._v2);
    _rendererHelper->addVertexCoordinate(xFinal[1], yFinal[1], 0, c.red, c.green, c.blue, c.alpha, uFinal[1], tr._v1);
    _rendererHelper->addVertexCoordinate(xFinal[2], yFinal[2], 0, c.red, c.green, c.blue, c.alpha, uFinal[2], tr._v1);
    _rendererHelper->addVertexCoordinate(xFinal[3], yFinal[3], 0, c.red, c.green, c.blue, c.alpha, uFinal[3], tr._v2);
    
    _numSprites++;
}

void SpriteBatcher::endBatch(NGTexture* texture, NGShader& gpuProgramWrapper)
{
    if (_numSprites > 0)
    {
        gpuProgramWrapper.bind(texture);
        
        _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, _numSprites * INDICES_PER_RECTANGLE);
        
        gpuProgramWrapper.unbind();
    }
}
