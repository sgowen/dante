//
//  Box2DDebugRenderer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/22/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Box2DDebugRenderer__
#define __noctisgames__Box2DDebugRenderer__

#include "Box2D/Common/b2Draw.h"

class QuadBatcher;
class LineBatcher;
class CircleBatcher;
class NGShader;
class b2World;

class Box2DDebugRenderer : public b2Draw
{
public:
    Box2DDebugRenderer(QuadBatcher& fillQuadBatcher, QuadBatcher& boundsQuadBatcher, LineBatcher& lineBatcher, CircleBatcher& circleBatcher);
    virtual ~Box2DDebugRenderer();
    
    /// Draw a closed polygon provided in CCW order.
    virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
    
    /// Draw a solid closed polygon provided in CCW order.
    virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
    
    /// Draw a circle.
    virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
    
    /// Draw a solid circle.
    virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
    
    /// Draw a line segment.
    virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
    
    /// Draw a transform. Choose your own length scale.
    /// @param xf a transform.
    virtual void DrawTransform(const b2Transform& xf);
    
    /// Draw a point.
    virtual void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);
    
    void render(b2World* world, NGShader& shaderProgram);
    
private:
    QuadBatcher& _fillQuadBatcher;
    QuadBatcher& _boundsQuadBatcher;
    LineBatcher& _lineBatcher;
    CircleBatcher& _circleBatcher;
};

#endif /* defined(__noctisgames__Box2DDebugRenderer__) */
