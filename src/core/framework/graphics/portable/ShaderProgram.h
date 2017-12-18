//
//  ShaderProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ShaderProgram__
#define __noctisgames__ShaderProgram__

#include <cstddef>

class RendererHelper;
struct ShaderProgramWrapper;

class ShaderProgram
{
public:
    ShaderProgram(RendererHelper& inRendererHelper, ShaderProgramWrapper* inShaderProgramWrapper);
    
    virtual ~ShaderProgram();
    
    virtual void bind(void* data = NULL) = 0;
    
    virtual void unbind() = 0;
    
protected:
    RendererHelper& _rendererHelper;
    ShaderProgramWrapper* _shaderProgramWrapper;
};

#endif /* defined(__noctisgames__ShaderProgram__) */
