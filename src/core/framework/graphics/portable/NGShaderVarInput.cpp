//
//  NGShaderVarInput.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/NGShaderVarInput.h>

#include <framework/graphics/portable/ShaderProgramWrapper.h>

#include <framework/util/macros.h>

NGShaderVarInput::NGShaderVarInput(const char* attribName, uint32_t size, uint32_t offset) : _attribName(attribName), _size(size), _offset(offset)
{
#if defined _WIN32
    DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    if (_size == 3)
    {
        format = DXGI_FORMAT_R32G32B32_FLOAT;
    }
    else if (_size == 2)
    {
        format = DXGI_FORMAT_R32G32_FLOAT;
    }
    
    _attribute = { attribName, 0, format, 0, _offset * 4, D3D11_INPUT_PER_VERTEX_DATA, 0 };
#endif
}

void NGShaderVarInput::build(ShaderProgramWrapper* inShaderProgramWrapper, uint32_t totalSize)
{
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    _stride = totalSize * sizeof(GLfloat);
    _bufferOffset = BUFFER_OFFSET(_offset * sizeof(GL_FLOAT));
    
    _attribute = glGetAttribLocation(inShaderProgramWrapper->_programObjectId, _attribName);
    glEnableVertexAttribArray(_attribute);
#endif
}
