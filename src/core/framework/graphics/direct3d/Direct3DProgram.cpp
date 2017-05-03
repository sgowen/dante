//
//  Direct3DProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Direct3DProgram.h"

#include "Direct3DManager.h"
#include "PlatformHelpers.h"
#include "ReadData.h"

Direct3DProgram::Direct3DProgram(const char* vertexShaderName, const char* pixelShaderName, bool useTextureCoords)
{
	char* vertexShaderFileName;
	{
		size_t len = strlen(vertexShaderName);

		vertexShaderFileName = new char[len + 5];

		strcpy_s(vertexShaderFileName, len + 5, vertexShaderName);
		vertexShaderFileName[len] = '.';
		vertexShaderFileName[len + 1] = 'c';
		vertexShaderFileName[len + 2] = 's';
		vertexShaderFileName[len + 3] = 'o';
		vertexShaderFileName[len + 4] = '\0';
	}

	char* pixelShaderFileName;
	{
		size_t len = strlen(pixelShaderName);

		pixelShaderFileName = new char[len + 5];

		strcpy_s(pixelShaderFileName, len + 5, pixelShaderName);
		pixelShaderFileName[len] = '.';
		pixelShaderFileName[len + 1] = 'c';
		pixelShaderFileName[len + 2] = 's';
		pixelShaderFileName[len + 3] = 'o';
		pixelShaderFileName[len + 4] = '\0';
	}

	const wchar_t* finalVertexShaderFileName;
	const wchar_t* finalPixelShaderFileName;
	wchar_t* wString1 = new wchar_t[4096];
	wchar_t* wString2 = new wchar_t[4096];
#if (_WIN32_WINNT == _WIN32_WINNT_WIN7)
	std::wstring s1(L"data\\shaders\\");
	MultiByteToWideChar(CP_ACP, 0, vertexShaderFileName, -1, wString1, 4096);
	s1 += std::wstring(wString1);
	finalVertexShaderFileName = s1.c_str();

	std::wstring s2(L"data\\shaders\\");
	MultiByteToWideChar(CP_ACP, 0, pixelShaderFileName, -1, wString2, 4096);
	s2 += std::wstring(wString2);
	finalPixelShaderFileName = s2.c_str();
#else
	MultiByteToWideChar(CP_ACP, 0, vertexShaderFileName, -1, wString1, 4096);
	finalVertexShaderFileName = wString1;
	MultiByteToWideChar(CP_ACP, 0, pixelShaderFileName, -1, wString2, 4096);
	finalPixelShaderFileName = wString2;
#endif

	ID3D11Device* d3dDevice = Direct3DManager::getD3dDevice();

	auto blob = DX::ReadData(finalVertexShaderFileName);
	DirectX::ThrowIfFailed(
		d3dDevice->CreateVertexShader(
			blob.data(),
			blob.size(),
			nullptr,
			&m_vertexShader
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
			blob.data(),
			blob.size(),
			&m_inputLayout
		)
	);

	blob = DX::ReadData(finalPixelShaderFileName);
	DirectX::ThrowIfFailed(
		d3dDevice->CreatePixelShader(
			blob.data(),
			blob.size(),
			nullptr,
			&m_pixelShader
		)
	);

	delete vertexShaderFileName;
	delete pixelShaderFileName;
	delete wString1;
	delete wString2;
}

Direct3DProgram::~Direct3DProgram()
{
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
}

void Direct3DProgram::bindShaders()
{
    ID3D11DeviceContext* d3dContext = Direct3DManager::getD3dContext();
    
    d3dContext->IASetInputLayout(m_inputLayout.Get());
    
    // set the shader objects as the active shaders
    d3dContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    d3dContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
}

void Direct3DProgram::bindMatrix()
{
    ID3D11DeviceContext* d3dContext = Direct3DManager::getD3dContext();
    
    d3dContext->VSSetConstantBuffers(0, 1, D3DManager->getMatrixConstantbuffer().GetAddressOf());
    
    // send the final matrix to video memory
    d3dContext->UpdateSubresource(D3DManager->getMatrixConstantbuffer().Get(), 0, 0, &D3DManager->getMatFinal(), 0, 0);
}

void Direct3DProgram::bindNormalSamplerState()
{
    ID3D11DeviceContext* d3dContext = Direct3DManager::getD3dContext();
    
    d3dContext->PSSetSamplers(0, 1, D3DManager->getSbSamplerState().GetAddressOf());
}

void Direct3DProgram::bindWrapSamplerState()
{
    ID3D11DeviceContext* d3dContext = Direct3DManager::getD3dContext();
    
    d3dContext->PSSetSamplers(0, 1, D3DManager->getSbWrapSamplerState().GetAddressOf());
}

void Direct3DProgram::createConstantBuffer(_COM_Outptr_opt_  ID3D11Buffer **ppBuffer)
{
	ID3D11Device* d3dDevice = Direct3DManager::getD3dDevice();

	D3D11_BUFFER_DESC bd = { 0 };

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = 16;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	d3dDevice->CreateBuffer(&bd, nullptr, ppBuffer);
}
