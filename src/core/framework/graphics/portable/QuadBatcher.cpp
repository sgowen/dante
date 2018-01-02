//
//  QuadBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/25/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/QuadBatcher.h"

#include "framework/graphics/portable/RendererHelper.h"
#include "framework/graphics/portable/ShaderProgram.h"
#include "framework/math/NGRect.h"
#include "framework/graphics/portable/Color.h"

#include <framework/util/FrameworkConstants.h>

QuadBatcher::QuadBatcher(RendererHelper* inRendererHelper, bool isFill) : _rendererHelper(inRendererHelper), _isFill(isFill), _numQuads(0)
{
    // Empty
}

QuadBatcher::~QuadBatcher()
{
    // Empty
}

void QuadBatcher::beginBatch()
{
    _rendererHelper->clearColorVertices();
    _numQuads = 0;
}

void QuadBatcher::renderRect(NGRect &r, Color &c)
{
    float x1 = r.getLeft();
    float y1 = r.getBottom();
    float x2 = x1 + r.getWidth();
    float y2 = y1 + r.getHeight();
    
    _rendererHelper->addVertexCoordinate(x1, y1, 0, c.red, c.green, c.blue, c.alpha);
    _rendererHelper->addVertexCoordinate(x1, y2, 0, c.red, c.green, c.blue, c.alpha);
    _rendererHelper->addVertexCoordinate(x2, y2, 0, c.red, c.green, c.blue, c.alpha);
    _rendererHelper->addVertexCoordinate(x2, y1, 0, c.red, c.green, c.blue, c.alpha);
    
    _numQuads++;
}

void QuadBatcher::renderQuad(float x1, float y1,
                             float x2, float y2,
                             float x3, float y3,
                             float x4, float y4,
                             Color &c)
{
    _rendererHelper->addVertexCoordinate(x1, y1, 0, c.red, c.green, c.blue, c.alpha);
    _rendererHelper->addVertexCoordinate(x2, y2, 0, c.red, c.green, c.blue, c.alpha);
    _rendererHelper->addVertexCoordinate(x3, y3, 0, c.red, c.green, c.blue, c.alpha);
    _rendererHelper->addVertexCoordinate(x4, y4, 0, c.red, c.green, c.blue, c.alpha);
    
    _numQuads++;
}

void QuadBatcher::endBatch(ShaderProgram &gpuProgramWrapper)
{
    if (_numQuads > 0)
    {
        gpuProgramWrapper.bind();
        
        if (_isFill)
        {
            _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, _numQuads * INDICES_PER_RECTANGLE);
        }
        else
        {
            for (uint32_t i = 0; i < _numQuads; ++i)
            {
                _rendererHelper->drawIndexed(NGPrimitiveType_LineStrip, i * INDICES_PER_RECTANGLE, INDICES_PER_RECTANGLE);
            }
        }
        
        gpuProgramWrapper.unbind();
    }
}
