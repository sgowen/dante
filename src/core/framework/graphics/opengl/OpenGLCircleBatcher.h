//
//  OpenGLCircleBatcher.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/15/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLCircleBatcher__
#define __noctisgames__OpenGLCircleBatcher__

#include "framework/graphics/portable/CircleBatcher.h"

class OpenGLCircleBatcher : public CircleBatcher
{
public:
    OpenGLCircleBatcher();
    
protected:
    virtual void clearVertices();
    
    virtual void addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a);
    
    virtual void endBatch(GpuProgramWrapper &gpuProgramWrapper);
};

#endif /* defined(__noctisgames__OpenGLCircleBatcher__) */
