//
//  LineBatcher.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/15/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__LineBatcher__
#define __noctisgames__LineBatcher__

#include <framework/graphics/portable/VertexProgramInput.h>

#include <vector>

class RendererHelper;
class NGShader;
class Line;
class Color;

class LineBatcher
{
public:
    LineBatcher(RendererHelper* inRendererHelper);
    ~LineBatcher();
    
    void beginBatch();
    void renderLine(Line &line, Color &c);
    void renderLine(float oX, float oY, float eX, float eY, Color &c);
    void endBatch(NGShader* shader);
    
private:
    RendererHelper* _rendererHelper;
    std::vector<COLOR_VERTEX> _vertices;
    int _numLines;
};

#endif /* defined(__noctisgames__LineBatcher__) */
