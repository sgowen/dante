//
//  Box2DDebugRenderer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/22/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/Box2DDebugRenderer.h>

#include "framework/graphics/portable/QuadBatcher.h"
#include "framework/graphics/portable/LineBatcher.h"
#include "framework/graphics/portable/CircleBatcher.h"
#include <framework/graphics/portable/NGShader.h>
#include <Box2D/Box2D.h>

#include "framework/math/NGRect.h"
#include "framework/math/Circle.h"
#include "framework/graphics/portable/Color.h"

Box2DDebugRenderer::Box2DDebugRenderer(QuadBatcher& fillQuadBatcher, QuadBatcher& boundsQuadBatcher, LineBatcher& lineBatcher, CircleBatcher& circleBatcher) :
_fillQuadBatcher(fillQuadBatcher),
_boundsQuadBatcher(boundsQuadBatcher),
_lineBatcher(lineBatcher),
_circleBatcher(circleBatcher),
_world(NULL)
{
    // Empty
}

Box2DDebugRenderer::~Box2DDebugRenderer()
{
    // Empty
}

void Box2DDebugRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    if (vertexCount == 4)
    {
        Color c = Color(color.r, color.g, color.b, color.a / 2);
        _boundsQuadBatcher.renderQuad(vertices[0].x, vertices[0].y,
                                      vertices[1].x, vertices[1].y,
                                      vertices[2].x, vertices[2].y,
                                      vertices[3].x, vertices[3].y, c);
    }
}

void Box2DDebugRenderer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    if (vertexCount == 4)
    {
        Color c = Color(color.r, color.g, color.b, color.a / 2);
        _fillQuadBatcher.renderQuad(vertices[0].x, vertices[0].y,
                                    vertices[1].x, vertices[1].y,
                                    vertices[2].x, vertices[2].y,
                                    vertices[3].x, vertices[3].y, c);
    }
}

void Box2DDebugRenderer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
    // TODO
}

void Box2DDebugRenderer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
    // TODO
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

void Box2DDebugRenderer::render(NGShader& shaderProgram)
{
    if (!_world)
    {
        return;
    }
    
    _boundsQuadBatcher.beginBatch();
    _fillQuadBatcher.beginBatch();
    _lineBatcher.beginBatch();
    _circleBatcher.beginBatch();
    
    _world->DrawDebugData();
    
    _boundsQuadBatcher.endBatch(shaderProgram);
    _fillQuadBatcher.endBatch(shaderProgram);
    _lineBatcher.endBatch(shaderProgram);
    _circleBatcher.endBatch(shaderProgram);
}

void Box2DDebugRenderer::setWorld(b2World* world)
{
    _world = world;
    _world->SetDebugDraw(this);
    AppendFlags(e_shapeBit);
}
