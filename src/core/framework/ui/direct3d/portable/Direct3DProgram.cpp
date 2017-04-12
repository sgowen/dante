//
//  Direct3DProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Direct3DProgram.h"

#include "DeviceResources.h"
#include "Direct3DManager.h"
#include "PlatformHelpers.h"
#include "ReadData.h"

Direct3DProgram::Direct3DProgram(_In_z_ const wchar_t* vertexShaderName, _In_z_ const wchar_t* pixelShaderName, bool useTextureCoords)
{
	DX::DeviceResources* deviceResources = Direct3DManager::getDeviceResources();

	auto blob = DX::ReadData(vertexShaderName);
	DirectX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreateVertexShader(
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
		deviceResources->GetD3DDevice()->CreateInputLayout(
            useTextureCoords ? textureVertexDesc : geometryVertexDesc,
			useTextureCoords ? ARRAYSIZE(textureVertexDesc) : ARRAYSIZE(geometryVertexDesc),
			blob.data(),
			blob.size(),
			&m_inputLayout
		)
	);

	blob = DX::ReadData(pixelShaderName);
	DirectX::ThrowIfFailed(
		deviceResources->GetD3DDevice()->CreatePixelShader(
			blob.data(),
			blob.size(),
			nullptr,
			&m_pixelShader
		)
	);
}

Direct3DProgram::~Direct3DProgram()
{
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
}

void Direct3DProgram::bindShaders()
{
    DX::DeviceResources* deviceResources = Direct3DManager::getDeviceResources();
    
    deviceResources->GetD3DDeviceContext()->IASetInputLayout(m_inputLayout.Get());
    
    // set the shader objects as the active shaders
    deviceResources->GetD3DDeviceContext()->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    deviceResources->GetD3DDeviceContext()->PSSetShader(m_pixelShader.Get(), nullptr, 0);
}

void Direct3DProgram::bindMatrix()
{
    DX::DeviceResources* deviceResources = Direct3DManager::getDeviceResources();
    
    deviceResources->GetD3DDeviceContext()->VSSetConstantBuffers(0, 1, D3DManager->getMatrixConstantbuffer().GetAddressOf());
    
    // send the final matrix to video memory
    deviceResources->GetD3DDeviceContext()->UpdateSubresource(D3DManager->getMatrixConstantbuffer().Get(), 0, 0, &D3DManager->getMatFinal(), 0, 0);
}

void Direct3DProgram::bindNormalSamplerState()
{
    DX::DeviceResources* deviceResources = Direct3DManager::getDeviceResources();
    
    deviceResources->GetD3DDeviceContext()->PSSetSamplers(0, 1, D3DManager->getSbSamplerState().GetAddressOf());
}

void Direct3DProgram::bindWrapSamplerState()
{
    DX::DeviceResources* deviceResources = Direct3DManager::getDeviceResources();
    
    deviceResources->GetD3DDeviceContext()->PSSetSamplers(0, 1, D3DManager->getSbWrapSamplerState().GetAddressOf());
}

void Direct3DProgram::createConstantBuffer(_COM_Outptr_opt_  ID3D11Buffer **ppBuffer)
{
	DX::DeviceResources* deviceResources = Direct3DManager::getDeviceResources();

	D3D11_BUFFER_DESC bd = { 0 };

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = 16;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	deviceResources->GetD3DDevice()->CreateBuffer(&bd, nullptr, ppBuffer);
}
