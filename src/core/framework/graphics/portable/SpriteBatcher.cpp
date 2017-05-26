//
//  SpriteBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/25/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SpriteBatcher.h"

#include "GpuTextureWrapper.h"
#include "GpuProgramWrapper.h"
#include "TextureRegion.h"
#include "Color.h"

#include "macros.h"
#include "NGRect.h"
#include "Vector2D.h"

#include <math.h>

SpriteBatcher::SpriteBatcher() : m_iNumSprites(0)
{
    // Empty
}

SpriteBatcher::~SpriteBatcher()
{
    // Empty
}

void SpriteBatcher::drawSprite(float x, float y, float width, float height, float angle, TextureRegion& tr)
{
    renderSprite(x, y, width, height, angle, false, tr);
}

void SpriteBatcher::drawSprite(float x, float y, float width, float height, float angle, Color &c, TextureRegion& tr)
{
    renderSprite(x, y, width, height, angle, false, c, tr);
}

void SpriteBatcher::renderSprite(float x, float y, float width, float height, float angle, bool flipX, TextureRegion& tr)
{
    if (angle != 0)
    {
        float halfWidth = width / 2;
        float halfHeight = height / 2;
        
        float rad = DEGREES_TO_RADIANS(angle);
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
        
        addVertexCoordinate(x1, y1, 0, 1, 1, 1, 1, flipX ? tr.u2 : tr.u1, tr.v2);
        addVertexCoordinate(x4, y4, 0, 1, 1, 1, 1, flipX ? tr.u2 : tr.u1, tr.v1);
        addVertexCoordinate(x3, y3, 0, 1, 1, 1, 1, flipX ? tr.u1 : tr.u2, tr.v1);
        addVertexCoordinate(x2, y2, 0, 1, 1, 1, 1, flipX ? tr.u1 : tr.u2, tr.v2);
    }
    else
    {
        renderSprite(x, y, width, height, flipX, tr);
    }
    
    m_iNumSprites++;
}

void SpriteBatcher::renderSprite(float x, float y, float width, float height, float angle, bool flipX, Color &c, TextureRegion& tr)
{
    if (angle != 0)
    {
        float halfWidth = width / 2;
        float halfHeight = height / 2;
        
        float rad = DEGREES_TO_RADIANS(angle);
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
        
        addVertexCoordinate(x1, y1, 0, c.red, c.green, c.blue, c.alpha, flipX ? tr.u2 : tr.u1, tr.v2);
        addVertexCoordinate(x4, y4, 0, c.red, c.green, c.blue, c.alpha, flipX ? tr.u2 : tr.u1, tr.v1);
        addVertexCoordinate(x3, y3, 0, c.red, c.green, c.blue, c.alpha, flipX ? tr.u1 : tr.u2, tr.v1);
        addVertexCoordinate(x2, y2, 0, c.red, c.green, c.blue, c.alpha, flipX ? tr.u1 : tr.u2, tr.v2);
    }
    else
    {
        renderSprite(x, y, width, height, flipX, c, tr);
    }
    
    m_iNumSprites++;
}

#pragma private methods

void SpriteBatcher::renderSprite(float x, float y, float width, float height, bool flipX, TextureRegion& tr)
{
    float halfWidth = width / 2;
    float halfHeight = height / 2;
    float x1 = x - halfWidth;
    float y1 = y - halfHeight;
    float x2 = x + halfWidth;
    float y2 = y + halfHeight;
    
    addVertexCoordinate(x1, y1, 0, 1, 1, 1, 1, flipX ? tr.u2 : tr.u1, tr.v2);
    addVertexCoordinate(x1, y2, 0, 1, 1, 1, 1, flipX ? tr.u2 : tr.u1, tr.v1);
    addVertexCoordinate(x2, y2, 0, 1, 1, 1, 1, flipX ? tr.u1 : tr.u2, tr.v1);
    addVertexCoordinate(x2, y1, 0, 1, 1, 1, 1, flipX ? tr.u1 : tr.u2, tr.v2);
}

void SpriteBatcher::renderSprite(float x, float y, float width, float height, bool flipX, Color &c, TextureRegion& tr)
{
    float halfWidth = width / 2;
    float halfHeight = height / 2;
    float x1 = x - halfWidth;
    float y1 = y - halfHeight;
    float x2 = x + halfWidth;
    float y2 = y + halfHeight;
    
    addVertexCoordinate(x1, y1, 0, c.red, c.green, c.blue, c.alpha, flipX ? tr.u2 : tr.u1, tr.v2);
    addVertexCoordinate(x1, y2, 0, c.red, c.green, c.blue, c.alpha, flipX ? tr.u2 : tr.u1, tr.v1);
    addVertexCoordinate(x2, y2, 0, c.red, c.green, c.blue, c.alpha, flipX ? tr.u1 : tr.u2, tr.v1);
    addVertexCoordinate(x2, y1, 0, c.red, c.green, c.blue, c.alpha, flipX ? tr.u1 : tr.u2, tr.v2);
}
