//
//  OpenGLFramebufferToScreenGpuProgramWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLFramebufferToScreenGpuProgramWrapper__
#define __noctisgames__OpenGLFramebufferToScreenGpuProgramWrapper__

#include "framework/graphics/portable/GpuProgramWrapper.h"

class OpenGLRendererHelper;
class OpenGLFramebufferToScreenProgram;

class OpenGLFramebufferToScreenGpuProgramWrapper : public GpuProgramWrapper
{
public:
    OpenGLFramebufferToScreenGpuProgramWrapper(OpenGLRendererHelper* inRendererHelper);
    
    virtual ~OpenGLFramebufferToScreenGpuProgramWrapper();
    
    virtual void bind();
    
    virtual void unbind();
    
private:
    OpenGLFramebufferToScreenProgram* _program;
};

#endif /* defined(__noctisgames__OpenGLFramebufferToScreenGpuProgramWrapper__) */
