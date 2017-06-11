//
//  Direct3DFramebufferToScreenGpuProgramWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/7/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Direct3DFramebufferToScreenGpuProgramWrapper__
#define __noctisgames__Direct3DFramebufferToScreenGpuProgramWrapper__

#include "GpuProgramWrapper.h"

class Direct3DProgram;

class Direct3DFramebufferToScreenGpuProgramWrapper : public GpuProgramWrapper
{
public:
	Direct3DFramebufferToScreenGpuProgramWrapper();

    virtual ~Direct3DFramebufferToScreenGpuProgramWrapper();
    
	virtual void bind();

	virtual void unbind();

private:
    Direct3DProgram* m_program;
};

#endif /* defined(__noctisgames__Direct3DFramebufferToScreenGpuProgramWrapper__) */
