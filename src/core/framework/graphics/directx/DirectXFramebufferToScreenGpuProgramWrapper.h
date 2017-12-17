//
//  DirectXFramebufferToScreenGpuProgramWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/7/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXFramebufferToScreenGpuProgramWrapper__
#define __noctisgames__DirectXFramebufferToScreenGpuProgramWrapper__

#include "framework/graphics/portable/GpuProgramWrapper.h"

class RendererHelper;
class DirectXProgram;

class DirectXFramebufferToScreenGpuProgramWrapper : public GpuProgramWrapper
{
public:
	DirectXFramebufferToScreenGpuProgramWrapper(DirectXRendererHelper* inRendererHelper);

    virtual ~DirectXFramebufferToScreenGpuProgramWrapper();
    
	virtual void bind();

	virtual void unbind();

private:
    DirectXProgram* _program;
};

#endif /* defined(__noctisgames__DirectXFramebufferToScreenGpuProgramWrapper__) */
