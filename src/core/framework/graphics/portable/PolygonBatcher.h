//
//  PolygonBatcher.h
//  noctisgames
//
//  Created by Stephen Gowen on 9/25/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__PolygonBatcher__
#define __noctisgames__PolygonBatcher__

#include <framework/graphics/portable/VertexProgramInput.h>

#include <vector>

class RendererHelper;
class NGShader;
class NGRect;
class Color;
struct b2Vec2;

class PolygonBatcher
{
public:
    PolygonBatcher(RendererHelper* inRendererHelper, bool isFill);
    ~PolygonBatcher();
    
    void beginBatch();
    void renderRect(NGRect &r);
    void renderPolygon(const b2Vec2* vertices, int vertexCount);
    void endBatch(NGShader* shader, Color &c);
    
private:
    RendererHelper* _rendererHelper;
    std::vector<VERTEX_2D> _quadVertices;
    std::vector<VERTEX_2D> _triangleVertices;
    bool _isFill;
    int _numQuads;
    int _numTriangles;
};

#endif /* defined(__noctisgames__PolygonBatcher__) */
