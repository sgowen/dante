//
//  DirectXCircleBatcher.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/16/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXCircleBatcher__
#define __noctisgames__DirectXCircleBatcher__

#include "CircleBatcher.h"

#include "pch.h"

class DirectXCircleBatcher : public CircleBatcher
{
public:
	DirectXCircleBatcher();

	virtual void renderCircle(Circle &circle, Color &c, GpuProgramWrapper &gpuProgramWrapper);

	virtual void renderPartialCircle(Circle &circle, int arcDegrees, Color &c, GpuProgramWrapper &gpuProgramWrapper);

protected:
	virtual void endBatch(GpuProgramWrapper &gpuProgramWrapper);

private:
	void addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a);
};

#endif /* defined(__noctisgames__DirectXCircleBatcher__) */
