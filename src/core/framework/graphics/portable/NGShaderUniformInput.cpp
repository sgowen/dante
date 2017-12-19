//
//  NGShaderUniformInput.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/NGShaderUniformInput.h"

#include "framework/graphics/portable/ShaderProgramWrapper.h"

#if defined _WIN32
#include "framework/graphics/directx/DirectXRendererHelper.h"
#include "PlatformHelpers.h"
#endif

NGShaderUniformInput::NGShaderUniformInput(const char* attribName, int byteWidth) : _attribName(attribName)
{
#if defined _WIN32
    if (byteWidth > 0)
    {
        D3D11_BUFFER_DESC bd = { 0 };
        
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = byteWidth;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        
		ID3D11Device* d3dDevice = DirectXRendererHelper::getD3dDevice();
        DirectX::ThrowIfFailed(d3dDevice->CreateBuffer(&bd, NULL, &_constantbuffer));
    }
#endif
}

#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
void NGShaderUniformInput::build(ShaderProgramWrapper* inShaderProgramWrapper)
{
    _attribute = glGetUniformLocation(inShaderProgramWrapper->_programObjectId, _attribName);
}
#endif
