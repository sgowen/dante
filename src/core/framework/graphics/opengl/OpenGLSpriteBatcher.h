//
//  OpenGLSpriteBatcher.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLSpriteBatcher__
#define __noctisgames__OpenGLSpriteBatcher__

#include "SpriteBatcher.h"

class OpenGLSpriteBatcher : public SpriteBatcher
{
public:
    OpenGLSpriteBatcher();
    
    virtual void beginBatch();
    
    virtual void endBatch(GpuTextureWrapper& textureWrapper, GpuProgramWrapper &gpuProgramWrapper);
    
protected:
    virtual void addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a, float u, float v);
};

#endif /* defined(__noctisgames__OpenGLSpriteBatcher__) */
