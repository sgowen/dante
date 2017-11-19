//
//  DirectXProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXProgram.h"

#include "framework/graphics/directx/DirectXManager.h"
#include "framework/util/StringUtil.h"

#include "PlatformHelpers.h"

DirectXProgram::DirectXProgram(const char* vertexShaderName, const char* pixelShaderName, bool useTextureCoords)
{
	char* vertexShaderFileName;
	{
		size_t len = strlen(vertexShaderName);

		vertexShaderFileName = new char[len + 5];

		strcpy_s(vertexShaderFileName, len + 5, vertexShaderName);
		vertexShaderFileName[len] = '.';
		vertexShaderFileName[len + 1] = 'n';
		vertexShaderFileName[len + 2] = 'g';
		vertexShaderFileName[len + 3] = 's';
		vertexShaderFileName[len + 4] = '\0';
	}

	char* fragmentShaderFileName;
	{
		size_t len = strlen(pixelShaderName);

		fragmentShaderFileName = new char[len + 5];

		strcpy_s(fragmentShaderFileName, len + 5, pixelShaderName);
		fragmentShaderFileName[len] = '.';
		fragmentShaderFileName[len + 1] = 'n';
		fragmentShaderFileName[len + 2] = 'g';
		fragmentShaderFileName[len + 3] = 's';
		fragmentShaderFileName[len + 4] = '\0';
	}
    
    const char* finalVertexShaderFileName;
    const char* finalFragmentShaderFileName;
#if (_WIN32_WINNT == _WIN32_WINNT_WIN7)
    {
        std::string s("data\\shaders\\");
        s += std::string(vertexShaderFileName);
        finalVertexShaderFileName = s.c_str();
    }
    
    {
        std::string s("data\\shaders\\");
        s += std::string(fragmentShaderFileName);
        finalFragmentShaderFileName = s.c_str();
    }
#else
    finalVertexShaderFileName = vertexShaderFileName;
    finalFragmentShaderFileName = fragmentShaderFileName;
#endif

	ID3D11Device* d3dDevice = DirectXManager::getD3dDevice();

    const FileData vertex_shader_source = AssetDataHandler::getAssetDataHandler()->getAssetData(finalVertexShaderFileName);
    unsigned char* vertex_shader_source_output = (unsigned char*) malloc(vertex_shader_source.data_length);
    StringUtil::encryptDecrypt((unsigned char*)vertex_shader_source.data, vertex_shader_source_output, vertex_shader_source.data_length);
    
	DirectX::ThrowIfFailed(
		d3dDevice->CreateVertexShader(
			vertex_shader_source_output,
			vertex_shader_source.data_length,
			nullptr,
			&_vertexShader
		)
	);

    static const D3D11_INPUT_ELEMENT_DESC geometryVertexDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    
	static const D3D11_INPUT_ELEMENT_DESC textureVertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	DirectX::ThrowIfFailed(
		d3dDevice->CreateInputLayout(
            useTextureCoords ? textureVertexDesc : geometryVertexDesc,
			useTextureCoords ? ARRAYSIZE(textureVertexDesc) : ARRAYSIZE(geometryVertexDesc),
			vertex_shader_source_output,
			vertex_shader_source.data_length,
			&_inputLayout
		)
	);

    const FileData fragment_shader_source = AssetDataHandler::getAssetDataHandler()->getAssetData(finalFragmentShaderFileName);
    unsigned char* fragment_shader_source_output = (unsigned char*) malloc(fragment_shader_source.data_length);
    StringUtil::encryptDecrypt((unsigned char*)fragment_shader_source.data, fragment_shader_source_output, fragment_shader_source.data_length);
    
	DirectX::ThrowIfFailed(
		d3dDevice->CreatePixelShader(
			fragment_shader_source_output,
			fragment_shader_source.data_length,
			nullptr,
			&_pixelShader
		)
	);

	delete vertexShaderFileName;
	delete fragmentShaderFileName;
    
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

void DirectXProgram::bindShaders()
{
    ID3D11DeviceContext* d3dContext = DirectXManager::getD3dContext();
    
    d3dContext->IASetInputLayout(_inputLayout.Get());
    
    // set the shader objects as the active shaders
    d3dContext->VSSetShader(_vertexShader.Get(), nullptr, 0);
    d3dContext->PSSetShader(_pixelShader.Get(), nullptr, 0);
}

void DirectXProgram::bindMatrix()
{
    ID3D11DeviceContext* d3dContext = DirectXManager::getD3dContext();
    
    d3dContext->VSSetConstantBuffers(0, 1, DXManager->getMatrixConstantbuffer().GetAddressOf());
    
    // send the final matrix to video memory
    d3dContext->UpdateSubresource(DXManager->getMatrixConstantbuffer().Get(), 0, 0, &DXManager->getMatFinal(), 0, 0);
}

void DirectXProgram::createConstantBuffer(_COM_Outptr_opt_  ID3D11Buffer **ppBuffer)
{
	ID3D11Device* d3dDevice = DirectXManager::getD3dDevice();

	D3D11_BUFFER_DESC bd = { 0 };

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = 16;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	d3dDevice->CreateBuffer(&bd, nullptr, ppBuffer);
}
