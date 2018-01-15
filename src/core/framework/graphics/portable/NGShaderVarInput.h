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

struct ShaderProgramWrapper;

class NGShaderVarInput
{
public:
    const char* _attribName;
    uint32_t _size;
    uint32_t _offset;
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    GLint _attribute;
    uint32_t _stride;
    void* _bufferOffset;
#elif defined _WIN32
    D3D11_INPUT_ELEMENT_DESC _attribute;
#endif
    
    NGShaderVarInput(const char* attribName, uint32_t size, uint32_t offset);
    
    void build(ShaderProgramWrapper* inShaderProgramWrapper, uint32_t totalSize);
};

#endif /* defined(__noctisgames__NGShaderVarInput__) */
