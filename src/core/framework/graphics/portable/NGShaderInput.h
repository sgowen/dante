//
//  NGShaderInput.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__VertexProgramInput__
#define __noctisgames__VertexProgramInput__

#include <framework/graphics/portable/NGGraphics.h>

class ShaderProgramWrapper;

class NGShaderInput
{
public:
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    GLint _attribute;
#elif defined _WIN32
    D3D11_INPUT_ELEMENT_DESC _attribute;
#endif
    
    int32_t _size;
    int32_t _offset;
    
    NGShaderInput(ShaderProgramWrapper* shaderProgramWrapper, const char* attribName, int size, int offset);
};

#endif /* defined(__noctisgames__VertexProgramInput__) */
