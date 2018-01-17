//
//  PolygonBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/25/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/PolygonBatcher.h"

#include "framework/graphics/portable/RendererHelper.h"
#include "framework/graphics/portable/NGShader.h"
#include "framework/math/NGRect.h"
#include "framework/graphics/portable/Color.h"
#include "Box2D/Common/b2Math.h"

#include <framework/util/FrameworkConstants.h>

PolygonBatcher::PolygonBatcher(RendererHelper* inRendererHelper, bool isFill) : _rendererHelper(inRendererHelper), _isFill(isFill), _numQuads(0), _numTriangles(0)
{
    _quadVertices.reserve(MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
    _triangleVertices.resize(MAX_BATCH_SIZE * VERTICES_PER_TRIANGLE);
}

PolygonBatcher::~PolygonBatcher()
{
    // Empty
}

void PolygonBatcher::beginBatch()
{
    _quadVertices.clear();
    _triangleVertices.clear();
    
    _numQuads = 0;
    _numTriangles = 0;
}

void PolygonBatcher::renderRect(NGRect &r, Color &c)
{
    float x1 = r.getLeft();
    float y1 = r.getBottom();
    float x2 = x1 + r.getWidth();
    float y2 = y1 + r.getHeight();
    
    _quadVertices.push_back(COLOR_VERTEX(x1, y1, c.red, c.green, c.blue, c.alpha));
    _quadVertices.push_back(COLOR_VERTEX(x1, y2, c.red, c.green, c.blue, c.alpha));
    _quadVertices.push_back(COLOR_VERTEX(x2, y2, c.red, c.green, c.blue, c.alpha));
    _quadVertices.push_back(COLOR_VERTEX(x2, y1, c.red, c.green, c.blue, c.alpha));
    
    ++_numQuads;
}

void PolygonBatcher::renderPolygon(const b2Vec2* vertices, int vertexCount, Color &c)
{
    if (vertexCount == 4)
    {
#ifdef _WIN32
		for (int i = (vertexCount - 1); i >= 0; --i)
		{
			_quadVertices.push_back(COLOR_VERTEX(vertices[i].x, vertices[i].y, c.red, c.green, c.blue, c.alpha));
		}
#else
		for (int i = 0; i < vertexCount; ++i)
		{
			_quadVertices.push_back(COLOR_VERTEX(vertices[i].x, vertices[i].y, c.red, c.green, c.blue, c.alpha));
		}
#endif
        
        ++_numQuads;
    }
    else if (vertexCount == 3)
    {
        for (int i = 0; i < vertexCount; ++i)
        {
            _triangleVertices.push_back(COLOR_VERTEX(vertices[i].x, vertices[i].y, c.red, c.green, c.blue, c.alpha));
        }
        
        ++_numTriangles;
    }
}

void PolygonBatcher::endBatch(NGShader* shader)
{
    assert(shader);
    
    if (_numQuads > 0)
    {
        shader->bind(&_quadVertices);
        
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
        
        shader->unbind();
    }
    
    if (_numTriangles > 0)
    {
        shader->bind(&_triangleVertices);
        
        if (_isFill)
        {
            _rendererHelper->draw(NGPrimitiveType_Triangles, 0, VERTICES_PER_TRIANGLE * _numTriangles);
        }
        else
        {
            _rendererHelper->draw(NGPrimitiveType_LineStrip, 0, VERTICES_PER_TRIANGLE * _numTriangles);
        }
        
        shader->unbind();
    }
}
