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
#include "framework/graphics/portable/NGShader.h"
#include "framework/graphics/portable/Color.h"

#include "framework/util/FrameworkConstants.h"
#include "framework/util/macros.h"

#include <math.h>

#define DEGREE_SPACING 6

CircleBatcher::CircleBatcher(RendererHelper* inRendererHelper) : _rendererHelper(inRendererHelper), _numPoints(0)
{
    _vertices.reserve(MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
}

CircleBatcher::~CircleBatcher()
{
    // Empty
}

void CircleBatcher::beginBatch()
{
    _vertices.clear();
    
    _circles.clear();
}

void CircleBatcher::renderCircle(Circle &circle, Color &c)
{
    _numPoints = 0;
    
    for (int i = 0; i <= 360; i += DEGREE_SPACING)
    {
        float rad = DEGREES_TO_RADIANS(i);
        float cos = cosf(rad);
        float sin = sinf(rad);
        
        addVertexCoordinate(cos * circle.getRadius() + circle.getCenter().getX(), sin * circle.getRadius() + circle.getCenter().getY(), c.red, c.green, c.blue, c.alpha);
        
        addVertexCoordinate(circle.getCenter().getX(), circle.getCenter().getY(), c.red, c.green, c.blue, c.alpha);
    }
    
    _circles.push_back(_numPoints);
}

void CircleBatcher::renderPartialCircle(Circle &circle, int arcDegrees, Color &c)
{
    _numPoints = 0;
    
    for (int i = 90; i < (450 - arcDegrees); i += DEGREE_SPACING)
    {
        float rad = DEGREES_TO_RADIANS(i);
        float cos = cosf(rad);
        float sin = sinf(rad);
        
        addVertexCoordinate(cos * circle.getRadius() + circle.getCenter().getX(), sin * circle.getRadius() + circle.getCenter().getY(), c.red, c.green, c.blue, c.alpha);
        
        addVertexCoordinate(circle.getCenter().getX(), circle.getCenter().getY(), c.red, c.green, c.blue, c.alpha);
    }
    
    float rad = DEGREES_TO_RADIANS(450 - arcDegrees);
    float cos = cosf(rad);
    float sin = sinf(rad);
    
    addVertexCoordinate(cos * circle.getRadius() + circle.getCenter().getX(), sin * circle.getRadius() + circle.getCenter().getY(), c.red, c.green, c.blue, c.alpha);
    
    addVertexCoordinate(circle.getCenter().getX(), circle.getCenter().getY(), c.red, c.green, c.blue, c.alpha);
    
    _circles.push_back(_numPoints);
}

void CircleBatcher::endBatch(NGShader &shader)
{
	if (_circles.size() > 0)
	{
		shader.bind(&_vertices);

		int offset = 0;
		for (std::vector<int>::iterator i = _circles.begin(); i != _circles.end(); ++i)
		{
			int numPoints = (*i);
			_rendererHelper->draw(NGPrimitiveType_TriangleStrip, offset, numPoints);
			offset += numPoints;
		}

		shader.unbind();
	}
}

void CircleBatcher::addVertexCoordinate(float x, float y, float r, float g, float b, float a)
{
    _vertices.push_back(COLOR_VERTEX(x, y, r, g, b, a));
    
    ++_numPoints;
}
