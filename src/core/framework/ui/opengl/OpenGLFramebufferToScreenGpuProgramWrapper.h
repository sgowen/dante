//
//  OpenGLFramebufferToScreenGpuProgramWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLFramebufferToScreenGpuProgramWrapper__
#define __noctisgames__OpenGLFramebufferToScreenGpuProgramWrapper__

#include "GpuProgramWrapper.h"

class OpenGLFramebufferToScreenProgram;

class OpenGLFramebufferToScreenGpuProgramWrapper : public GpuProgramWrapper
{
public:
    OpenGLFramebufferToScreenGpuProgramWrapper();
    
    virtual ~OpenGLFramebufferToScreenGpuProgramWrapper();
    
    virtual void bind();
    
    virtual void unbind();
    
private:
    OpenGLFramebufferToScreenProgram* m_program;
};

#endif /* defined(__noctisgames__OpenGLFramebufferToScreenGpuProgramWrapper__) */
