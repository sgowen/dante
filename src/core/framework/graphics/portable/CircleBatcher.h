//
//  CircleBatcher.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/15/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__CircleBatcher__
#define __noctisgames__CircleBatcher__

class RendererHelper;
class Circle;
class ShaderProgram;
class Color;

class CircleBatcher
{
public:
    CircleBatcher(RendererHelper* inRendererHelper);
    
    ~CircleBatcher();
    
    void renderCircle(Circle &circle, Color &c, ShaderProgram &gpuProgramWrapper);
    
    void renderPartialCircle(Circle &circle, int arcDegrees, Color &c, ShaderProgram &gpuProgramWrapper);
    
private:
    RendererHelper* _rendererHelper;
    int _numPoints;
    
    void addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a);
    
    void endBatch(ShaderProgram &gpuProgramWrapper);
};

#endif /* defined(__noctisgames__CircleBatcher__) */
