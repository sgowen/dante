//
//  DirectXProgramLoader.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXProgramLoader.h"

#include "framework/graphics/portable/ShaderProgramWrapper.h"
#include "framework/graphics/portable/NGShaderVarInput.h"
#include "framework/graphics/portable/NGShaderUniformInput.h"

#include "framework/file/portable/AssetDataHandler.h"
#include "framework/file/portable/FileData.h"
#include "framework/util/StringUtil.h"
#include <framework/graphics/directx/DirectXRendererHelper.h>

#include "PlatformHelpers.h"

DirectXProgramLoader::DirectXProgramLoader() : ShaderProgramLoader()
{
    // Empty
}

DirectXProgramLoader::~DirectXProgramLoader()
{
    // Empty
}

ShaderProgramWrapper* DirectXProgramLoader::loadShaderProgram(const char* vertexShaderName, const char* fragmentShaderName, std::vector<NGShaderUniformInput*>& uniforms, std::vector<NGShaderVarInput*>& inputLayout)
{
    std::string s1("data\\shaders\\");
	s1 += std::string(vertexShaderName);
	const char* finalVertexShaderFileName = s1.c_str();

	std::string s2("data\\shaders\\");
	s2 += std::string(fragmentShaderName);
	const char* finalFragmentShaderFileName = s2.c_str();

    const FileData vertex_shader_source = AssetDataHandler::getAssetDataHandler()->getAssetData(finalVertexShaderFileName);
    unsigned char* vertex_shader_source_output = (unsigned char*) malloc(vertex_shader_source.data_length);
    StringUtil::encryptDecrypt((unsigned char*)vertex_shader_source.data, vertex_shader_source_output, vertex_shader_source.data_length);
    
    ID3D11VertexShader* pVertexShader;
    ID3D11InputLayout* pInputLayout;
    ID3D11PixelShader* pPixelShader;
    
    ID3D11Device* d3dDevice = DirectXRendererHelper::getD3dDevice();
    
	DirectX::ThrowIfFailed(
		d3dDevice->CreateVertexShader(
			vertex_shader_source_output,
			vertex_shader_source.data_length,
			NULL,
			&pVertexShader
		)
	);
    
    const D3D11_INPUT_ELEMENT_DESC inputElementDescs[3] = {
        { "a_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "a_Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "a_TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs;
    inputElementDescs.reserve(inputLayout.size());
    
    for (std::vector<NGShaderVarInput*>::iterator i = inputLayout.begin(); i != inputLayout.end(); ++i)
    {
        NGShaderVarInput* svi = (*i);
        
        inputElementDescs.push_back(svi->_attribute);
    }

	DirectX::ThrowIfFailed(
		d3dDevice->CreateInputLayout(
            inputElementDescs,
			inputElementDescs.size(),
			vertex_shader_source_output,
			vertex_shader_source.data_length,
			&pInputLayout
		)
	);

    const FileData fragment_shader_source = AssetDataHandler::getAssetDataHandler()->getAssetData(finalFragmentShaderFileName);
    unsigned char* fragment_shader_source_output = (unsigned char*) malloc(fragment_shader_source.data_length);
    StringUtil::encryptDecrypt((unsigned char*)fragment_shader_source.data, fragment_shader_source_output, fragment_shader_source.data_length);
    
	DirectX::ThrowIfFailed(
		d3dDevice->CreatePixelShader(
			fragment_shader_source_output,
			fragment_shader_source.data_length,
			NULL,
			&pPixelShader
		)
	);
    
    AssetDataHandler::getAssetDataHandler()->releaseAssetData(&vertex_shader_source);
    AssetDataHandler::getAssetDataHandler()->releaseAssetData(&fragment_shader_source);
    
    free((void *)vertex_shader_source_output);
    free((void *)fragment_shader_source_output);
    
    return new ShaderProgramWrapper(pVertexShader, pInputLayout, pPixelShader);
}
