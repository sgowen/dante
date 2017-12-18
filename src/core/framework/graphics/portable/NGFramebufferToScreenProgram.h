//
//  NGFramebufferToScreenProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGFramebufferToScreenProgram__
#define __noctisgames__NGFramebufferToScreenProgram__

#include "framework/graphics/portable/ShaderProgram.h"

class NGFramebufferToScreenProgram : public ShaderProgram
{
public:
    NGFramebufferToScreenProgram(RendererHelper& inRendererHelper, ShaderProgramLoader& inShaderProgramLoader, const char* vertexShaderName, const char* fragmentShaderName);
    
    virtual void bind(void* data = NULL);
    
    virtual void unbind();
};

#endif /* defined(__noctisgames__NGFramebufferToScreenProgram__) */
