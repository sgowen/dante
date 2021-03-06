//
//  NGShaderUniformInput.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/graphics/portable/NGShaderUniformInput.h>

#include <framework/graphics/portable/ShaderProgramWrapper.h>

#if defined _WIN32
#include <framework/graphics/directx/DirectXRendererHelper.h>
#endif

NGShaderUniformInput::NGShaderUniformInput(const char* attribName, int index, int byteWidth, bool isFragment) :
_attribName(attribName),
_index(index),
_byteWidth(byteWidth),
_isFragment(isFragment),
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
_attribute(0)
#elif _WIN32
_constantbuffer(NULL)
#endif
{
    // Empty
}

void NGShaderUniformInput::build(ShaderProgramWrapper* inShaderProgramWrapper)
{
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    _attribute = glGetUniformLocation(inShaderProgramWrapper->_programObjectId, _attribName);
#elif defined _WIN32
    if (_byteWidth > 0)
    {
        D3D11_BUFFER_DESC bd = { 0 };
        
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = _byteWidth;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        
        ID3D11Device* d3dDevice = DirectXRendererHelper::getD3DDevice();
        
        ID3D11Buffer* constantbuffer;
        DX::ThrowIfFailed(d3dDevice->CreateBuffer(&bd, NULL, &constantbuffer));
        _constantbuffer = constantbuffer;
    }
#endif
}
