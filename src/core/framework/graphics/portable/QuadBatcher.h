//
//  QuadBatcher.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/25/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__QuadBatcher__
#define __noctisgames__QuadBatcher__

#include <framework/graphics/portable/VertexProgramInput.h>

#include <vector>

class RendererHelper;
class NGShader;
class NGRect;
class Color;

class QuadBatcher
{
public:
    QuadBatcher(RendererHelper* inRendererHelper, bool isFill);
    ~QuadBatcher();
    
    void beginBatch();
    
    void renderRect(NGRect &r, Color &c);
    void renderQuad(float x1, float y1,
                    float x2, float y2,
                    float x3, float y3,
                    float x4, float y4, Color &c);
    
    void endBatch(NGShader &shader);
    
private:
    RendererHelper* _rendererHelper;
    std::vector<COLOR_VERTEX> _vertices;
    bool _isFill;
    int _numQuads;
};

#endif /* defined(__noctisgames__QuadBatcher__) */
