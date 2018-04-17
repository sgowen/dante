//
//  Box2DDebugRenderer.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 12/22/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/Box2DDebugRenderer.h>

#include <framework/graphics/portable/PolygonBatcher.h>
#include <framework/graphics/portable/LineBatcher.h>
#include <framework/graphics/portable/CircleBatcher.h>
#include <framework/graphics/portable/NGShader.h>
#include <Box2D/Box2D.h>

#include <framework/math/NGRect.h>
#include <framework/math/Circle.h>
#include <framework/graphics/portable/Color.h>

Box2DDebugRenderer::Box2DDebugRenderer(PolygonBatcher& fillPolygonBatcher, PolygonBatcher& boundsPolygonBatcher, LineBatcher& lineBatcher, CircleBatcher& circleBatcher) :
_fillPolygonBatcher(fillPolygonBatcher),
_boundsPolygonBatcher(boundsPolygonBatcher),
_lineBatcher(lineBatcher),
_circleBatcher(circleBatcher),
_shader(NULL)
{
    // Empty
}

Box2DDebugRenderer::~Box2DDebugRenderer()
{
    // Empty
}

void Box2DDebugRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    Color c = Color(color.r, color.g, color.b, 0.3f);
    _boundsPolygonBatcher.beginBatch();
    _boundsPolygonBatcher.renderPolygon(vertices, vertexCount);
    _boundsPolygonBatcher.endBatch(_shader, c);
}

void Box2DDebugRenderer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    {
        Color c = Color(color.r, color.g, color.b, 0.3f);
        _fillPolygonBatcher.beginBatch();
        _fillPolygonBatcher.renderPolygon(vertices, vertexCount);
        _fillPolygonBatcher.endBatch(_shader, c);
    }
    
    {
        Color c = Color(1, 0, 0, 0.3f);
        _boundsPolygonBatcher.beginBatch();
        _boundsPolygonBatcher.renderPolygon(vertices, vertexCount);
        _boundsPolygonBatcher.endBatch(_shader, c);
    }
}

void Box2DDebugRenderer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
    {
        Color c = Color(color.r, color.g, color.b, 0.3f);
        _circleBatcher.beginBatch();
        _circleBatcher.renderCircle(center.x, center.y, radius);
        _circleBatcher.endBatch(_shader, c);
    }
}

void Box2DDebugRenderer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
    {
        Color c = Color(color.r, color.g, color.b, 0.3f);
        _circleBatcher.beginBatch();
        _circleBatcher.renderCircle(center.x, center.y, radius);
        _circleBatcher.endBatch(_shader, c);
    }
}

void Box2DDebugRenderer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    // TODO
}

void Box2DDebugRenderer::DrawTransform(const b2Transform& xf)
{
    // TODO
}

void Box2DDebugRenderer::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
    // TODO
}

void Box2DDebugRenderer::render(b2World* world, NGShader* shader)
{
    assert(world);
    assert(shader);
    
    _shader = shader;
    
    world->SetDebugDraw(this);
    AppendFlags(e_shapeBit);
    
    world->DrawDebugData();
}
