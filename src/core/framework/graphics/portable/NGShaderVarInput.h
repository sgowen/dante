//
//  NGShaderVarInput.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGShaderVarInput__
#define __noctisgames__NGShaderVarInput__

#include <framework/graphics/portable/NGGraphics.h>

class ShaderProgramWrapper;

class NGShaderVarInput
{
public:
    const char* _attribName;
    int32_t _size;
    int32_t _offset;
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    GLint _attribute;
    int32_t _stride;
    void* _bufferOffset;
#elif defined _WIN32
    D3D11_INPUT_ELEMENT_DESC _attribute;
#endif
    
    NGShaderVarInput(const char* attribName, int size, int offset);
    
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    void build(ShaderProgramWrapper* inShaderProgramWrapper, int totalSize);
#endif
};

#endif /* defined(__noctisgames__NGShaderVarInput__) */
