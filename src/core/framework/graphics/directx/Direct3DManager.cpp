//
//  Direct3DManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 11/17/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Direct3DManager.h"

#include "Direct3DTextureGpuProgramWrapper.h"
#include "Direct3DGeometryGpuProgramWrapper.h"
#include "Direct3DFramebufferToScreenGpuProgramWrapper.h"
#include "GpuTextureWrapper.h"
#include "PlatformHelpers.h"
#include "NGSTDUtil.h"
#include "GpuTextureWrapper.h"

#include <assert.h>

using namespace DirectX;

Direct3DManager* Direct3DManager::s_pInstance = nullptr;

ID3D11Device* Direct3DManager::s_d3dDevice = nullptr;
ID3D11DeviceContext* Direct3DManager::s_d3dContext = nullptr;
ID3D11RenderTargetView* Direct3DManager::s_d3dRenderTargetView = nullptr;
XMFLOAT4X4 Direct3DManager::s_orientation;

void Direct3DManager::init(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dContext, ID3D11RenderTargetView* d3dRenderTargetView, XMFLOAT4X4 orientation)
{
	s_d3dDevice = d3dDevice;
	s_d3dContext = d3dContext;
	s_d3dRenderTargetView = d3dRenderTargetView;
	s_orientation = orientation;
}

ID3D11Device* Direct3DManager::getD3dDevice()
{
	return s_d3dDevice;
}

ID3D11DeviceContext* Direct3DManager::getD3dContext()
{
	return s_d3dContext;
}

ID3D11RenderTargetView* Direct3DManager::getD3dRenderTargetView()
{
	return s_d3dRenderTargetView;
}

void Direct3DManager::create()
{
    assert(!s_pInstance);
    
    s_pInstance = new Direct3DManager();
}

void Direct3DManager::destroy()
{
    assert(s_pInstance);
    
    delete s_pInstance;
    s_pInstance = nullptr;
}

Direct3DManager * Direct3DManager::getInstance()
{
    return s_pInstance;
}

void Direct3DManager::createDeviceDependentResources(int maxBatchSize)
{
	assert(_offscreenRenderTargets.size() == 0);
	assert(_offscreenRenderTargetViews.size() == 0);
	assert(_offscreenShaderResourceViews.size() == 0);
    
    createBlendStates();
	createSamplerStates();
	createVertexBufferForSpriteBatcher(maxBatchSize);
	createVertexBufferForGeometryBatchers(maxBatchSize);
	createIndexBuffer(maxBatchSize);
	createConstantBuffer();
}

void Direct3DManager::createWindowSizeDependentResources(int renderWidth, int renderHeight, int numFramebuffers)
{
    _renderWidth = renderWidth;
    _renderHeight = renderHeight;
    _numFramebuffers = numFramebuffers;

	releaseFramebuffers();
    createFramebufferObjects();
}

void Direct3DManager::releaseDeviceDependentResources()
{
    releaseFramebuffers();

	_blendState.Reset();
	_screenBlendState.Reset();
	_matrixConstantbuffer.Reset();
	_indexbuffer.Reset();
	_sbSamplerState.Reset();
	_sbVertexBuffer.Reset();
	_textureVertices.clear();
	_gbVertexBuffer.Reset();
	_colorVertices.clear();
}

void Direct3DManager::updateMatrix(float left, float right, float bottom, float top)
{
	using namespace DirectX;

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&s_orientation);

	XMMATRIX matFinal = XMMatrixOrthographicOffCenterRH(left, right, bottom, top, -1.0, 1.0) * orientationMatrix;

	XMStoreFloat4x4(&_matFinal, matFinal);
}

void Direct3DManager::addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a, float u, float v)
{
	TEXTURE_VERTEX tv = { x, y, z, r, g, b, a, u, v };
	_textureVertices.push_back(tv);
}

void Direct3DManager::addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a)
{
	COLOR_VERTEX cv = { x, y, z, r, g, b, a };
	_colorVertices.push_back(cv);
}

void Direct3DManager::useNormalBlending()
{
    s_d3dContext->OMSetBlendState(D3DManager->_blendState.Get(), 0, 0xffffffff);
}

void Direct3DManager::useScreenBlending()
{
    s_d3dContext->OMSetBlendState(D3DManager->_screenBlendState.Get(), 0, 0xffffffff);
}

std::vector<ID3D11Texture2D*>& Direct3DManager::getOffscreenRenderTargets()
{
	return _offscreenRenderTargets;
}

std::vector<ID3D11RenderTargetView*>& Direct3DManager::getOffscreenRenderTargetViews()
{
	return _offscreenRenderTargetViews;
}

std::vector<ID3D11ShaderResourceView*>& Direct3DManager::getOffscreenShaderResourceViews()
{
	return _offscreenShaderResourceViews;
}

std::vector<GpuTextureWrapper *>& Direct3DManager::getFramebuffers()
{
    return _framebuffers;
}

Microsoft::WRL::ComPtr<ID3D11BlendState>& Direct3DManager::getBlendState()
{
	return _blendState;
}

Microsoft::WRL::ComPtr<ID3D11BlendState>& Direct3DManager::getScreenBlendState()
{
	return _screenBlendState;
}

Microsoft::WRL::ComPtr<ID3D11Buffer>& Direct3DManager::getMatrixConstantbuffer()
{
	return _matrixConstantbuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer>& Direct3DManager::getIndexbuffer()
{
	return _indexbuffer;
}

Microsoft::WRL::ComPtr<ID3D11SamplerState>& Direct3DManager::getSbSamplerState()
{
	return _sbSamplerState;
}

Microsoft::WRL::ComPtr<ID3D11SamplerState>& Direct3DManager::getSbWrapSamplerState()
{
	return _sbWrapSamplerState;
}

Microsoft::WRL::ComPtr<ID3D11Buffer>& Direct3DManager::getSbVertexBuffer()
{
	return _sbVertexBuffer;
}

std::vector<TEXTURE_VERTEX>& Direct3DManager::getTextureVertices()
{
	return _textureVertices;
}

Microsoft::WRL::ComPtr<ID3D11Buffer>& Direct3DManager::getGbVertexBuffer()
{
	return _gbVertexBuffer;
}

std::vector<COLOR_VERTEX>& Direct3DManager::getColorVertices()
{
	return _colorVertices;
}

DirectX::XMFLOAT4X4& Direct3DManager::getMatFinal()
{
	return _matFinal;
}

bool Direct3DManager::isWindowsMobile()
{
    return _isWindowsMobile;
}

// private

void Direct3DManager::createBlendStates()
{
	{
		D3D11_BLEND_DESC bd;
		bd.RenderTarget[0].BlendEnable = TRUE;
		bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		bd.IndependentBlendEnable = FALSE;
		bd.AlphaToCoverageEnable = FALSE;

		s_d3dDevice->CreateBlendState(&bd, &_blendState);
	}

	{
		D3D11_BLEND_DESC bd;
		bd.RenderTarget[0].BlendEnable = TRUE;
		bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		bd.IndependentBlendEnable = FALSE;
		bd.AlphaToCoverageEnable = FALSE;

		s_d3dDevice->CreateBlendState(&bd, &_screenBlendState);
	}
}

void Direct3DManager::createSamplerStates()
{
	{
		D3D11_SAMPLER_DESC sd;
		sd.Filter = D3D11_FILTER_ANISOTROPIC;
		sd.MaxAnisotropy = 16;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.BorderColor[0] = 0.0f;
		sd.BorderColor[1] = 0.0f;
		sd.BorderColor[2] = 0.0f;
		sd.BorderColor[3] = 0.0f;
		sd.MinLOD = 0.0f;
		sd.MaxLOD = FLT_MAX;
		sd.MipLODBias = 0.0f;
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // linear filtering
		sd.MinLOD = 5.0f; // mip level 5 will appear blurred

		s_d3dDevice->CreateSamplerState(&sd, _sbSamplerState.GetAddressOf());
	}

	{
		D3D11_SAMPLER_DESC sd;
		sd.Filter = D3D11_FILTER_ANISOTROPIC;
		sd.MaxAnisotropy = 16;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.BorderColor[0] = 0.0f;
		sd.BorderColor[1] = 0.0f;
		sd.BorderColor[2] = 0.0f;
		sd.BorderColor[3] = 0.0f;
		sd.MinLOD = 0.0f;
		sd.MaxLOD = FLT_MAX;
		sd.MipLODBias = 0.0f;
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // linear filtering
		sd.MinLOD = 5.0f; // mip level 5 will appear blurred

		s_d3dDevice->CreateSamplerState(&sd, _sbWrapSamplerState.GetAddressOf());
	}
}

void Direct3DManager::createVertexBufferForSpriteBatcher(int maxBatchSize)
{
	_textureVertices.reserve(maxBatchSize * VERTICES_PER_RECTANGLE);
	TEXTURE_VERTEX tv = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < maxBatchSize * VERTICES_PER_RECTANGLE; ++i)
	{
		_textureVertices.push_back(tv);
	}

	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	vertexBufferDesc.ByteWidth = sizeof(TEXTURE_VERTEX) * _textureVertices.size();
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	vertexBufferData.pSysMem = &_textureVertices[0];
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	DirectX::ThrowIfFailed(s_d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &_sbVertexBuffer));
}

void Direct3DManager::createVertexBufferForGeometryBatchers(int maxBatchSize)
{
	_colorVertices.reserve(maxBatchSize * VERTICES_PER_RECTANGLE);
	COLOR_VERTEX cv = { 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < maxBatchSize * VERTICES_PER_RECTANGLE; ++i)
	{
		_colorVertices.push_back(cv);
	}

	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	vertexBufferDesc.ByteWidth = sizeof(COLOR_VERTEX) * _colorVertices.size();
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	vertexBufferData.pSysMem = &_colorVertices[0];
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	DirectX::ThrowIfFailed(s_d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &_gbVertexBuffer));
}

void Direct3DManager::createIndexBuffer(int maxBatchSize)
{
	D3D11_BUFFER_DESC indexBufferDesc = { 0 };

	indexBufferDesc.ByteWidth = sizeof(short) * maxBatchSize * INDICES_PER_RECTANGLE;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	auto indexValues = createIndexValues(maxBatchSize);

	D3D11_SUBRESOURCE_DATA indexDataDesc = { 0 };

	indexDataDesc.pSysMem = &indexValues.front();

	s_d3dDevice->CreateBuffer(&indexBufferDesc, &indexDataDesc, &_indexbuffer);

	s_d3dContext->IASetIndexBuffer(_indexbuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
}

void Direct3DManager::createConstantBuffer()
{
	D3D11_BUFFER_DESC bd = { 0 };

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = 64;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	DirectX::ThrowIfFailed(s_d3dDevice->CreateBuffer(&bd, nullptr, &_matrixConstantbuffer));
}

std::vector<short> Direct3DManager::createIndexValues(int maxBatchSize)
{
	std::vector<short> indices;

	indices.reserve(maxBatchSize * INDICES_PER_RECTANGLE);

	short j = 0;
	for (int i = 0; i < maxBatchSize * INDICES_PER_RECTANGLE; i += INDICES_PER_RECTANGLE, j += VERTICES_PER_RECTANGLE)
	{
		indices.push_back(j);
		indices.push_back(j + 1);
		indices.push_back(j + 2);
		indices.push_back(j + 2);
		indices.push_back(j + 3);
		indices.push_back(j + 0);
	}

	return indices;
}

void Direct3DManager::createFramebufferObjects()
{
    for (int i = 0; i < _numFramebuffers; ++i)
    {
        createFramebufferObject();
    }
}

void Direct3DManager::createFramebufferObject()
{
    ID3D11Texture2D* _offscreenRenderTarget;
    ID3D11RenderTargetView* _offscreenRenderTargetView;
    ID3D11ShaderResourceView* _offscreenShaderResourceView;
    
    D3D11_TEXTURE2D_DESC textureDesc;
    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    
    // Initialize the render target texture description.
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    
    // Setup the render target texture description.
    textureDesc.Width = _renderWidth;
    textureDesc.Height = _renderHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;
    
    // Create the render target texture.
    DirectX::ThrowIfFailed(s_d3dDevice->CreateTexture2D(&textureDesc, NULL, &_offscreenRenderTarget));
    
    // Setup the description of the render target view.
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;
    
    // Create the render target view.
    DirectX::ThrowIfFailed(s_d3dDevice->CreateRenderTargetView(_offscreenRenderTarget, &renderTargetViewDesc, &_offscreenRenderTargetView));
    
    // Setup the description of the shader resource view.
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    
    // Create the shader resource view.
    DirectX::ThrowIfFailed(s_d3dDevice->CreateShaderResourceView(_offscreenRenderTarget, &shaderResourceViewDesc, &_offscreenShaderResourceView));
    
    _offscreenRenderTargets.push_back(_offscreenRenderTarget);
    _offscreenRenderTargetViews.push_back(_offscreenRenderTargetView);
    _offscreenShaderResourceViews.push_back(_offscreenShaderResourceView);
    
    _framebuffers.push_back(new GpuTextureWrapper(_offscreenShaderResourceView));
}

void Direct3DManager::releaseFramebuffers()
{
    for (std::vector<ID3D11Texture2D*>::iterator i = _offscreenRenderTargets.begin(); i != _offscreenRenderTargets.end(); ++i)
    {
        (*i)->Release();
    }
    
    for (std::vector<ID3D11RenderTargetView*>::iterator i = _offscreenRenderTargetViews.begin(); i != _offscreenRenderTargetViews.end(); ++i)
    {
        (*i)->Release();
    }
    
    for (std::vector<ID3D11ShaderResourceView*>::iterator i = _offscreenShaderResourceViews.begin(); i != _offscreenShaderResourceViews.end(); ++i)
    {
        (*i)->Release();
    }
    
    _offscreenRenderTargets.clear();
    _offscreenRenderTargetViews.clear();
    _offscreenShaderResourceViews.clear();
    
    NGSTDUtil::cleanUpVectorOfPointers(_framebuffers);
}

Direct3DManager::Direct3DManager() :
_renderWidth(-1),
_renderHeight(-1),
_numFramebuffers(-1),
_isWindowsMobile(false)
{
	// Hide Constructor for Singleton
#if !defined(WINAPI_FAMILY) || WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
	_isWindowsMobile = false;
#elif defined (WINAPI_FAMILY_SYSTEM)
	Windows::System::Profile::AnalyticsVersionInfo^ api = Windows::System::Profile::AnalyticsInfo::VersionInfo;
	_isWindowsMobile = api->DeviceFamily->Equals("Windows.Mobile");
#elif WINAPI_FAMILY == WINAPI_FAMILY_PC_APP
	_isWindowsMobile = false;
#else
	_isWindowsMobile = true;
#endif
}

Direct3DManager::~Direct3DManager()
{
	releaseDeviceDependentResources();
}
