//
//  DirectXTextureGpuProgramWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/30/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXTextureGpuProgramWrapper__
#define __noctisgames__DirectXTextureGpuProgramWrapper__

#include "framework/graphics/portable/GpuProgramWrapper.h"

class RendererHelper;
class DirectXProgram;

class DirectXTextureGpuProgramWrapper : public GpuProgramWrapper
{
public:
	DirectXTextureGpuProgramWrapper(DirectXRendererHelper* inRendererHelper);
    
    virtual ~DirectXTextureGpuProgramWrapper();

	virtual void bind();

	virtual void unbind();

private:
    DirectXProgram* _program;
};

#endif /* defined(__noctisgames__DirectXTextureGpuProgramWrapper__) */
