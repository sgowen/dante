//
//  DirectXGeometryGpuProgramWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/30/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXGeometryGpuProgramWrapper__
#define __noctisgames__DirectXGeometryGpuProgramWrapper__

#include "framework/graphics/portable/GpuProgramWrapper.h"

class DirectXRendererHelper;
class DirectXProgram;

class DirectXGeometryGpuProgramWrapper : public GpuProgramWrapper
{
public:
	DirectXGeometryGpuProgramWrapper(DirectXRendererHelper* inRendererHelper);
    
    virtual ~DirectXGeometryGpuProgramWrapper();
    
	virtual void bind();

	virtual void unbind();

private:
    DirectXProgram* _program;
};

#endif /* defined(__noctisgames__DirectXGeometryGpuProgramWrapper__) */
