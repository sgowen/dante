//
//  CircleBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/15/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/CircleBatcher.h"

#include "framework/graphics/portable/RendererHelper.h"
#include "framework/math/Circle.h"
#include "framework/graphics/portable/GpuProgramWrapper.h"
#include "framework/math/Color.h"

#include "framework/util/FrameworkConstants.h"
#include "framework/util/macros.h"

#include <math.h>

#define DEGREE_SPACING 6

CircleBatcher::CircleBatcher(RendererHelper* inRendererHelper) : _rendererHelper(inRendererHelper), _numPoints(0)
{
    // Empty
}

CircleBatcher::~CircleBatcher()
{
    // Empty
}

void CircleBatcher::renderCircle(Circle &circle, Color &c, GpuProgramWrapper &gpuProgramWrapper)
{
    clearVertices();
    
    _numPoints = 0;
    
    for (int i = 0; i <= 360; i += DEGREE_SPACING)
    {
        float rad = DEGREES_TO_RADIANS(i);
        float cos = cosf(rad);
        float sin = sinf(rad);
        
        addVertexCoordinate(cos * circle.getRadius() + circle.getCenter().getX(), sin * circle.getRadius() + circle.getCenter().getY(), 0, c.red, c.green, c.blue, c.alpha);
        
        addVertexCoordinate(circle.getCenter().getX(), circle.getCenter().getY(), 0, c.red, c.green, c.blue, c.alpha);
    }
    
    endBatch(gpuProgramWrapper);
}

void CircleBatcher::renderPartialCircle(Circle &circle, int arcDegrees, Color &c, GpuProgramWrapper &gpuProgramWrapper)
{
    clearVertices();
    
    _numPoints = 0;
    
    for (int i = 90; i < (450 - arcDegrees); i += DEGREE_SPACING)
    {
        float rad = DEGREES_TO_RADIANS(i);
        float cos = cosf(rad);
        float sin = sinf(rad);
        
        addVertexCoordinate(cos * circle.getRadius() + circle.getCenter().getX(), sin * circle.getRadius() + circle.getCenter().getY(), 0, c.red, c.green, c.blue, c.alpha);
        
        addVertexCoordinate(circle.getCenter().getX(), circle.getCenter().getY(), 0, c.red, c.green, c.blue, c.alpha);
    }
    
    float rad = DEGREES_TO_RADIANS(450 - arcDegrees);
    float cos = cosf(rad);
    float sin = sinf(rad);
    
    addVertexCoordinate(cos * circle.getRadius() + circle.getCenter().getX(), sin * circle.getRadius() + circle.getCenter().getY(), 0, c.red, c.green, c.blue, c.alpha);
    
    addVertexCoordinate(circle.getCenter().getX(), circle.getCenter().getY(), 0, c.red, c.green, c.blue, c.alpha);
    
    endBatch(gpuProgramWrapper);
}

void CircleBatcher::clearVertices()
{
    _rendererHelper->clearColorVertices();
}

void CircleBatcher::endBatch(GpuProgramWrapper &gpuProgramWrapper)
{
    gpuProgramWrapper.bind();
    
    _rendererHelper->draw(NGPrimitiveType_TriangleStrip, 0, _numPoints);
    
    gpuProgramWrapper.unbind();
}

void CircleBatcher::addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a)
{
    _rendererHelper->addVertexCoordinate(x, y, z, r, g, b, a);
    
    _numPoints++;
}
