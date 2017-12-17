//
//  OpenGLTextureGpuProgramWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLTextureGpuProgramWrapper__
#define __noctisgames__OpenGLTextureGpuProgramWrapper__

#include "framework/graphics/portable/GpuProgramWrapper.h"

class OpenGLRendererHelper;
class OpenGLTextureProgram;

class OpenGLTextureGpuProgramWrapper : public GpuProgramWrapper
{
public:
    OpenGLTextureGpuProgramWrapper(OpenGLRendererHelper* inRendererHelper);
    
    virtual ~OpenGLTextureGpuProgramWrapper();
    
    virtual void bind();
    
    virtual void unbind();
    
private:
    OpenGLTextureProgram* _program;
};

#endif /* defined(__noctisgames__OpenGLTextureGpuProgramWrapper__) */
