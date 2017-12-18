//
//  NGShaderUniformInput.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGShaderUniformInput__
#define __noctisgames__NGShaderUniformInput__

#include <framework/graphics/portable/NGGraphics.h>

class ShaderProgramWrapper;

class NGShaderUniformInput
{
public:
    const char* _attribName;
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    GLint _attribute;
#endif
    
    NGShaderUniformInput(const char* attribName);
    
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    void build(ShaderProgramWrapper* inShaderProgramWrapper);
#endif
};

#endif /* defined(__noctisgames__NGShaderUniformInput__) */
