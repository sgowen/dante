//
//  Direct3DSpriteBatcher.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Direct3DSpriteBatcher__
#define __noctisgames__Direct3DSpriteBatcher__

#include "SpriteBatcher.h"

class Direct3DSpriteBatcher : public SpriteBatcher
{
public:
	Direct3DSpriteBatcher();
    
	virtual void beginBatch();

	virtual void endBatch(GpuTextureWrapper& textureWrapper, GpuProgramWrapper &gpuProgramWrapper);
    
protected:
    virtual void addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a, float u, float v);
};

#endif /* defined(__noctisgames__Direct3DSpriteBatcher__) */
