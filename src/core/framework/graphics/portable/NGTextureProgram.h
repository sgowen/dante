//
//  NGTextureProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGTextureProgram__
#define __noctisgames__NGTextureProgram__

#include "framework/graphics/portable/ShaderProgram.h"

class NGTextureProgram : public ShaderProgram
{
public:
    NGTextureProgram(RendererHelper& inRendererHelper, ShaderProgramLoader& inShaderProgramLoader, const char* vertexShaderName, const char* fragmentShaderName);
    
    virtual void bind(void* data = NULL);
    
    virtual void unbind();
};

#endif /* defined(__noctisgames__NGTextureProgram__) */
