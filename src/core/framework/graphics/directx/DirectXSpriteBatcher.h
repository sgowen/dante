//
//  DirectXSpriteBatcher.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXSpriteBatcher__
#define __noctisgames__DirectXSpriteBatcher__

#include "SpriteBatcher.h"

class DirectXSpriteBatcher : public SpriteBatcher
{
public:
	DirectXSpriteBatcher();
    
	virtual void beginBatch();

	virtual void endBatch(TextureWrapper& textureWrapper, GpuProgramWrapper& gpuProgramWrapper);
    
protected:
    virtual void addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a, float u, float v);
    
private:
    void bindNormalSamplerState();
    
    void bindWrapSamplerState();
};

#endif /* defined(__noctisgames__DirectXSpriteBatcher__) */
