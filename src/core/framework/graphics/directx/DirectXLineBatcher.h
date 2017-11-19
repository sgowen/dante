//
//  DirectXLineBatcher.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/16/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXLineBatcher__
#define __noctisgames__DirectXLineBatcher__

#include "framework/graphics/portable/LineBatcher.h"

#include "pch.h"

class DirectXLineBatcher : public LineBatcher
{
public:
	DirectXLineBatcher();
    
    virtual void beginBatch();

	virtual void endBatch(GpuProgramWrapper &gpuProgramWrapper);
    
    virtual void renderLine(float originX, float originY, float endX, float endY, Color &color);
};

#endif /* defined(__noctisgames__DirectXLineBatcher__) */
