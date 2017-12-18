//
//  NGRectBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/25/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/NGRectBatcher.h"

#include "framework/graphics/portable/RendererHelper.h"
#include "framework/graphics/portable/GpuProgram.h"
#include "framework/math/NGRect.h"
#include "framework/graphics/portable/Color.h"

#include <framework/util/FrameworkConstants.h>

NGRectBatcher::NGRectBatcher(RendererHelper* inRendererHelper, bool isFill) : _rendererHelper(inRendererHelper), _isFill(isFill), _numNGRects(0)
{
    // Empty
}

NGRectBatcher::~NGRectBatcher()
{
    // Empty
}

void NGRectBatcher::beginBatch()
{
    _rendererHelper->clearColorVertices();
    _numNGRects = 0;
}

void NGRectBatcher::endBatch(GpuProgram &gpuProgramWrapper)
{
    if (_numNGRects > 0)
    {
        gpuProgramWrapper.bind();
        
        _rendererHelper->drawIndexed(_isFill ? NGPrimitiveType_Triangles : NGPrimitiveType_LineStrip, _numNGRects * INDICES_PER_RECTANGLE);
        
        gpuProgramWrapper.unbind();
    }
}

void NGRectBatcher::renderNGRect(NGRect &r, Color &c)
{
    float x1 = r.getLeft();
    float y1 = r.getBottom();
    float x2 = x1 + r.getWidth();
    float y2 = y1 + r.getHeight();
    
    _rendererHelper->addVertexCoordinate(x1, y1, 0, c.red, c.green, c.blue, c.alpha);
    _rendererHelper->addVertexCoordinate(x1, y2, 0, c.red, c.green, c.blue, c.alpha);
    _rendererHelper->addVertexCoordinate(x2, y2, 0, c.red, c.green, c.blue, c.alpha);
    _rendererHelper->addVertexCoordinate(x2, y1, 0, c.red, c.green, c.blue, c.alpha);
    
    _numNGRects++;
}
