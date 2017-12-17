//
//  DirectXProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXProgram.h"

#include "framework/graphics/directx/DirectXRendererHelper.h"
#include "framework/util/StringUtil.h"
#include "framework/file/portable/AssetDataHandler.h"
#include "framework/file/portable/FileData.h"

#include "PlatformHelpers.h"

DirectXProgram::DirectXProgram(DirectXRendererHelper* inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName, const D3D11_INPUT_ELEMENT_DESC* inputElementDescs, UINT numElements) : _rendererHelper(inRendererHelper), _d3dDevice(DirectXRendererHelper::getD3dDevice()), _d3dContext(DirectXRendererHelper::getD3dContext())
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
    
	DirectX::ThrowIfFailed(
		_d3dDevice->CreateVertexShader(
			vertex_shader_source_output,
			vertex_shader_source.data_length,
			NULL,
			&_vertexShader
		)
	);

	DirectX::ThrowIfFailed(
		_d3dDevice->CreateInputLayout(
            inputElementDescs,
			numElements,
			vertex_shader_source_output,
			vertex_shader_source.data_length,
			&_inputLayout
		)
	);

    const FileData fragment_shader_source = AssetDataHandler::getAssetDataHandler()->getAssetData(finalFragmentShaderFileName);
    unsigned char* fragment_shader_source_output = (unsigned char*) malloc(fragment_shader_source.data_length);
    StringUtil::encryptDecrypt((unsigned char*)fragment_shader_source.data, fragment_shader_source_output, fragment_shader_source.data_length);
    
	DirectX::ThrowIfFailed(
		_d3dDevice->CreatePixelShader(
			fragment_shader_source_output,
			fragment_shader_source.data_length,
			NULL,
			&_pixelShader
		)
	);
    
    AssetDataHandler::getAssetDataHandler()->releaseAssetData(&vertex_shader_source);
    AssetDataHandler::getAssetDataHandler()->releaseAssetData(&fragment_shader_source);
    
    free((void *)vertex_shader_source_output);
    free((void *)fragment_shader_source_output);
}

DirectXProgram::~DirectXProgram()
{
	_vertexShader.Reset();
	_inputLayout.Reset();
	_pixelShader.Reset();
}

void DirectXProgram::bind(void* data)
{
    _d3dContext->IASetInputLayout(_inputLayout.Get());
    
    // set the shader objects as the active shaders
    _d3dContext->VSSetShader(_vertexShader.Get(), NULL, 0);
    _d3dContext->PSSetShader(_pixelShader.Get(), NULL, 0);
}

void DirectXProgram::unbind()
{
    // Empty
}

void DirectXProgram::createConstantBuffer(_COM_Outptr_opt_  ID3D11Buffer **ppBuffer)
{
    D3D11_BUFFER_DESC bd = { 0 };

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = 16;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	_d3dDevice->CreateBuffer(&bd, NULL, ppBuffer);
}
