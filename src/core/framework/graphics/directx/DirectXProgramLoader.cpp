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

DirectXProgramLoader::DirectXProgramLoader() : NGShaderLoader()
{
    // Empty
}

DirectXProgramLoader::~DirectXProgramLoader()
{
    // Empty
}

ShaderProgramWrapper* DirectXProgramLoader::loadNGShader(const char* vertexShaderName, const char* fragmentShaderName, std::vector<NGShaderUniformInput*>& uniforms, std::vector<NGShaderVarInput*>& inputLayout)
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
    
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
    
    ID3D11Device* d3dDevice = DirectXRendererHelper::getD3dDevice();
    
	DirectX::ThrowIfFailed(
		d3dDevice->CreateVertexShader(
			vertex_shader_source_output,
			vertex_shader_source.data_length,
			NULL,
			&pVertexShader
		)
	);
    
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs;
    inputElementDescs.reserve(inputLayout.size());
    
    for (std::vector<NGShaderVarInput*>::iterator i = inputLayout.begin(); i != inputLayout.end(); ++i)
    {
        NGShaderVarInput* svi = (*i);
        
        inputElementDescs.push_back(svi->_attribute);
    }

	DirectX::ThrowIfFailed(
		d3dDevice->CreateInputLayout(
            &inputElementDescs.front(),
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
    
    ShaderProgramWrapper* ret = new ShaderProgramWrapper(pVertexShader, pInputLayout, pPixelShader);
    
    for (std::vector<NGShaderVarInput*>::iterator i = inputLayout.begin(); i != inputLayout.end(); ++i)
    {
        (*i)->build(ret, totalSize);
    }
    
    for (std::vector<NGShaderUniformInput*>::iterator i = uniforms.begin(); i != uniforms.end(); ++i)
    {
        (*i)->build(ret);
    }
    
    return ret;
}

void DirectXProgramLoader::destroyNGShader(ShaderProgramWrapper* shaderProgramWrapper, std::vector<NGShaderUniformInput*>& uniforms, std::vector<NGShaderVarInput*>& inputLayout)
{
    assert(shaderProgramWrapper != NULL);
    
    shaderProgramWrapper->_vertexShader.Reset();
    shaderProgramWrapper->_inputLayout.Reset();
    shaderProgramWrapper->_pixelShader.Reset();
    
    for (std::vector<NGShaderUniformInput*>::iterator i = uniforms.begin(); i != uniforms.end(); ++i)
    {
        NGShaderUniformInput* sui = (*i);
        
        sui->_constantbuffer.Reset();
    }
}
