//
//  CircleBatcher.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/15/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__CircleBatcher__
#define __noctisgames__CircleBatcher__

class Circle;
class GpuProgramWrapper;
class Color;

class CircleBatcher
{
public:
    CircleBatcher();
    
    virtual ~CircleBatcher();
    
    void renderCircle(Circle &circle, Color &c, GpuProgramWrapper &gpuProgramWrapper);
    
    void renderPartialCircle(Circle &circle, int arcDegrees, Color &c, GpuProgramWrapper &gpuProgramWrapper);
    
protected:
    int _numPoints;
    
    virtual void clearVertices() = 0;
    
    virtual void addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a) = 0;
    
    virtual void endBatch(GpuProgramWrapper &gpuProgramWrapper) = 0;
};

#endif /* defined(__noctisgames__CircleBatcher__) */
