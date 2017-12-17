//
//  OpenGLGeometryGpuProgramWrapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__OpenGLGeometryGpuProgramWrapper__
#define __noctisgames__OpenGLGeometryGpuProgramWrapper__

#include "framework/graphics/portable/GpuProgramWrapper.h"

class OpenGLRendererHelper;
class OpenGLGeometryProgram;

class OpenGLGeometryGpuProgramWrapper : public GpuProgramWrapper
{
public:
    OpenGLGeometryGpuProgramWrapper(OpenGLRendererHelper* inRendererHelper);
    
    virtual ~OpenGLGeometryGpuProgramWrapper();
    
    virtual void bind();
    
    virtual void unbind();
    
private:
    OpenGLGeometryProgram* _program;
};

#endif /* defined(__noctisgames__OpenGLGeometryGpuProgramWrapper__) */
