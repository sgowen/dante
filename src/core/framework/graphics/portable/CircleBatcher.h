//
//  CircleBatcher.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/15/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__CircleBatcher__
#define __noctisgames__CircleBatcher__

#include <framework/graphics/portable/VertexProgramInput.h>

#include <vector>

class RendererHelper;
class Circle;
class NGShader;
class Color;

class CircleBatcher
{
public:
    CircleBatcher(RendererHelper* inRendererHelper);
    ~CircleBatcher();
    
    void beginBatch();
    
    void renderCircle(Circle &circle, Color &c);
    void renderPartialCircle(Circle &circle, int arcDegrees, Color &c);
    
    void endBatch(NGShader &shader);
    
private:
    RendererHelper* _rendererHelper;
    std::vector<COLOR_VERTEX> _vertices;
    std::vector<int> _circles;
    int _numPoints;
    
    void addVertexCoordinate(float x, float y, float r, float g, float b, float a);
};

#endif /* defined(__noctisgames__CircleBatcher__) */
