//
//  DirectXRendererHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXRendererHelper.h"

#include "framework/graphics/portable/NGTexture.h"
#include "framework/graphics/portable/TextureWrapper.h"
#include "framework/graphics/portable/ShaderProgramWrapper.h"
#include "framework/graphics/portable/NGShaderUniformInput.h"
#include "framework/graphics/portable/NGShaderVarInput.h"

#include "framework/graphics/portable/NGTextureDesc.h"
#include "framework/util/NGSTDUtil.h"
#include <framework/util/FrameworkConstants.h>
#include <framework/util/macros.h>
#include <framework/util/Config.h>

#include <string>
#include <assert.h>

using namespace DirectX;

DX::DirectXDeviceResources* DirectXRendererHelper::s_deviceResources = NULL;

void DirectXRendererHelper::init(DX::DirectXDeviceResources* deviceResources)
{
    s_deviceResources = deviceResources;
}

ID3D11Device* DirectXRendererHelper::getD3DDevice()
{
    return s_deviceResources->GetD3DDevice();
}

ID3D11DeviceContext* DirectXRendererHelper::getD3DContext()
{
    return s_deviceResources->GetD3DDeviceContext();
}

DirectXRendererHelper::DirectXRendererHelper() : RendererHelper(),
_blendState(NULL),
_screenBlendState(NULL),
_textureSamplerState1(NULL),
_textureSamplerState2(NULL),
_textureSamplerState3(NULL),
_textureSamplerState4(NULL),
_textureSamplerState5(NULL),
_textureSamplerState6(NULL),
_textureSamplerState7(NULL),
_textureSamplerState8(NULL),
_framebufferSamplerState(NULL),
_fbIndex(0)
{
	// Empty
}

DirectXRendererHelper::~DirectXRendererHelper()
{
	// Empty
}

void DirectXRendererHelper::createDeviceDependentResources()
{
    RendererHelper::createDeviceDependentResources();
    
    assert(_offscreenRenderTargets.size() == 0);
    assert(_offscreenRenderTargetViews.size() == 0);
    assert(_offscreenShaderResourceViews.size() == 0);
    
    createBlendStates();
    createSamplerStates();
}

void DirectXRendererHelper::releaseDeviceDependentResources()
{
    RendererHelper::releaseDeviceDependentResources();
    
    _blendState->Release();
    _screenBlendState->Release();
    _textureSamplerState1->Release();
    _textureSamplerState2->Release();
    _textureSamplerState3->Release();
    _textureSamplerState4->Release();
    _textureSamplerState5->Release();
    _textureSamplerState6->Release();
    _textureSamplerState7->Release();
    _textureSamplerState8->Release();
    _framebufferSamplerState->Release();
}

void DirectXRendererHelper::bindToOffscreenFramebuffer(int index)
{
    getD3DContext()->OMSetRenderTargets(1, &_offscreenRenderTargetViews[index], NULL);
    
    // Set the viewport.
    auto viewport = s_deviceResources->GetOffScreenViewport();
    getD3DContext()->RSSetViewports(1, &viewport);
    
	_fbIndex = index;
}

void DirectXRendererHelper::clearFramebufferWithColor(float r, float g, float b, float a)
{
    float color[] = { r, g, b, a };

    ID3D11RenderTargetView * targets[1] = {};
    if (_fbIndex < 0)
    {
		targets[0] = s_deviceResources->GetRenderTargetView();
    }
    else
    {
        targets[0] = _offscreenRenderTargetViews[_fbIndex];
    }
    
	getD3DContext()->ClearRenderTargetView(targets[0], color);
}

void DirectXRendererHelper::bindToScreenFramebuffer()
{
    ID3D11RenderTargetView *const targets[1] = { s_deviceResources->GetRenderTargetView() };
	getD3DContext()->OMSetRenderTargets(1, targets, NULL);
    
    // Set the viewport.
    auto viewport = s_deviceResources->GetScreenViewport();
    getD3DContext()->RSSetViewports(1, &viewport);
    
    _fbIndex = -1;
}

void DirectXRendererHelper::useNormalBlending()
{
    getD3DContext()->OMSetBlendState(_blendState, 0, 0xffffffff);
}

void DirectXRendererHelper::useScreenBlending()
{
    getD3DContext()->OMSetBlendState(_screenBlendState, 0, 0xffffffff);
}

void DirectXRendererHelper::useNoBlending()
{
    getD3DContext()->OMSetBlendState(NULL, 0, 0xffffffff);
}

void DirectXRendererHelper::bindInt4(NGShaderUniformInput* uniform, int4& inValue)
{
	bindConstantBuffer(uniform, inValue);
}

void DirectXRendererHelper::bindFloat4(NGShaderUniformInput* uniform, float4& inValue)
{
	bindConstantBuffer(uniform, inValue);
}

void DirectXRendererHelper::bindFloat4Array(NGShaderUniformInput* uniform, int count, float4* inValue)
{
	bindConstantBuffer(uniform, inValue);
}

void DirectXRendererHelper::bindMatrix(NGShaderUniformInput* uniform, mat4x4& inValue)
{
	bindConstantBuffer(uniform, &inValue);
}

void DirectXRendererHelper::bindMatrix(NGShaderUniformInput* uniform)
{
	bindConstantBuffer(uniform, &_matrix);
}

void DirectXRendererHelper::bindShader(ShaderProgramWrapper* shaderProgramWrapper)
{
    if (shaderProgramWrapper)
    {
        // set the shader objects as the active shaders
        getD3DContext()->VSSetShader(shaderProgramWrapper->_vertexShader, NULL, 0);
        getD3DContext()->IASetInputLayout(shaderProgramWrapper->_inputLayout);
        getD3DContext()->PSSetShader(shaderProgramWrapper->_pixelShader, NULL, 0);
    }
}

void DirectXRendererHelper::bindTexture(NGTextureSlot textureSlot, NGTexture* texture, NGShaderUniformInput* uniform)
{
    UNUSED(uniform);
    
    if (texture)
    {
        getD3DContext()->PSSetShaderResources(textureSlot, 1, &texture->textureWrapper->texture);
        ID3D11SamplerState* samplerState = getSamplerStateForTexture(texture);
        getD3DContext()->PSSetSamplers(textureSlot, 1, &samplerState);
    }
    else
    {
        ID3D11ShaderResourceView *pSRV[1] = { NULL };
        getD3DContext()->PSSetShaderResources(textureSlot, 1, pSRV);
    }
}

void DirectXRendererHelper::mapTextureVertices(std::vector<VERTEX_2D_TEXTURE>& vertices, bool isDynamic, int gpuBufferIndex)
{
    ID3D11Buffer* buffer = isDynamic ? _dynamicTextureVertexBuffers[gpuBufferIndex]->buffer : _staticTextureVertexBuffers[gpuBufferIndex]->buffer;
    
    mapVertexBuffer(buffer, &vertices[0], vertices.size(), sizeof(VERTEX_2D_TEXTURE));
}

void DirectXRendererHelper::mapVertices(std::vector<VERTEX_2D>& vertices, bool isDynamic, int gpuBufferIndex)
{
    ID3D11Buffer* buffer = isDynamic ? _dynamicVertexBuffers[gpuBufferIndex]->buffer : _staticVertexBuffers[gpuBufferIndex]->buffer;
    
    mapVertexBuffer(buffer, &vertices[0], vertices.size(), sizeof(VERTEX_2D));
}

void DirectXRendererHelper::bindTextureVertexBuffer(int gpuBufferIndex)
{
    ID3D11Buffer* buffer = _staticTextureVertexBuffers[gpuBufferIndex]->buffer;
    
    bindVertexBuffer(buffer, sizeof(VERTEX_2D_TEXTURE));
}

void DirectXRendererHelper::bindScreenVertexBuffer()
{
    ID3D11Buffer* buffer = _staticScreenVertexBuffer->buffer;
    
    bindVertexBuffer(buffer, sizeof(VERTEX_2D));
}

void DirectXRendererHelper::draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count)
{
    getD3DContext()->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(renderPrimitiveType));
    getD3DContext()->Draw(count, first);
}

void DirectXRendererHelper::drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count)
{
    getD3DContext()->IASetIndexBuffer(_indexBuffer->buffer, DXGI_FORMAT_R16_UINT, 0);
    getD3DContext()->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(renderPrimitiveType));
    getD3DContext()->DrawIndexed(count, first, 0);
}

GPUBufferWrapper* DirectXRendererHelper::createGPUBuffer(size_t size, const void *data, bool isDynamic, bool isVertex)
{
    D3D11_BUFFER_DESC bufferDesc = { 0 };
    bufferDesc.ByteWidth = size;
    bufferDesc.Usage = isVertex ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = isVertex ? D3D11_BIND_VERTEX_BUFFER : D3D11_BIND_INDEX_BUFFER;
    bufferDesc.CPUAccessFlags = isVertex ? D3D11_CPU_ACCESS_WRITE : 0;
    
    D3D11_SUBRESOURCE_DATA bufferData = { 0 };
    bufferData.pSysMem = data;
    
    ID3D11Buffer* buffer;
    DX::ThrowIfFailed(getD3DDevice()->CreateBuffer(&bufferDesc, &bufferData, &buffer));
    
    return new GPUBufferWrapper(buffer);
}

void DirectXRendererHelper::disposeGPUBuffer(GPUBufferWrapper* gpuBuffer)
{
	if (gpuBuffer && gpuBuffer->buffer)
	{
		gpuBuffer->buffer->Release();
		gpuBuffer->buffer = NULL;
	}
}

TextureWrapper* DirectXRendererHelper::createFramebuffer()
{
    ID3D11Texture2D* offscreenRenderTarget;
    ID3D11RenderTargetView* offscreenRenderTargetView;
    ID3D11ShaderResourceView* offscreenShaderResourceView;
    
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
    DX::ThrowIfFailed(getD3DDevice()->CreateTexture2D(&textureDesc, NULL, &offscreenRenderTarget));
    
    // Setup the description of the render target view.
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;
    
    // Create the render target view.
    DX::ThrowIfFailed(getD3DDevice()->CreateRenderTargetView(offscreenRenderTarget, &renderTargetViewDesc, &offscreenRenderTargetView));
    
    // Setup the description of the shader resource view.
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    
    // Create the shader resource view.
    DX::ThrowIfFailed(getD3DDevice()->CreateShaderResourceView(offscreenRenderTarget, &shaderResourceViewDesc, &offscreenShaderResourceView));
    
    _offscreenRenderTargets.push_back(offscreenRenderTarget);
    _offscreenRenderTargetViews.push_back(offscreenRenderTargetView);
    _offscreenShaderResourceViews.push_back(offscreenShaderResourceView);
    
    return new TextureWrapper(offscreenShaderResourceView);
}

void DirectXRendererHelper::platformReleaseFramebuffers()
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
}

void DirectXRendererHelper::createBlendStates()
{
    D3D11_BLEND_DESC bd;
    bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    bd.IndependentBlendEnable = FALSE;
    bd.AlphaToCoverageEnable = FALSE;
    
    ID3D11BlendState* blendState;
    getD3DDevice()->CreateBlendState(&bd, &blendState);
    _blendState = blendState;
    
    bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    
    ID3D11BlendState* screenBlendState;
    getD3DDevice()->CreateBlendState(&bd, &screenBlendState);
    _screenBlendState = screenBlendState;
}

void DirectXRendererHelper::createSamplerStates()
{
    D3D11_SAMPLER_DESC sd;
	sd.MaxAnisotropy = 16;
    sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.BorderColor[0] = 0.0f;
    sd.BorderColor[1] = 0.0f;
    sd.BorderColor[2] = 0.0f;
    sd.BorderColor[3] = 0.0f;
    sd.MinLOD = 0.0f;
    sd.MaxLOD = 0.0f;
    sd.MipLODBias = 0.0f;
    
    {
        std::string cfgFilterMin = NG_CFG->getString("FramebufferFilterMin");
        std::string cfgFilterMag = NG_CFG->getString("FramebufferFilterMag");
        
        sd.Filter = filterForMinAndMag(cfgFilterMin, cfgFilterMag, false);
        
        ID3D11SamplerState* samplerState;
        getD3DDevice()->CreateSamplerState(&sd, &samplerState);
        _framebufferSamplerState = samplerState;
    }
    
    {
        sd.MaxLOD = FLT_MAX;
        sd.Filter = filterForMinAndMag("NEAREST", "NEAREST", true);
        
        ID3D11SamplerState* samplerState;
        getD3DDevice()->CreateSamplerState(&sd, &samplerState);
        _textureSamplerState1 = samplerState;
    }
    
    {
        sd.MaxLOD = FLT_MAX;
        sd.Filter = filterForMinAndMag("LINEAR", "NEAREST", true);
        
        ID3D11SamplerState* samplerState;
        getD3DDevice()->CreateSamplerState(&sd, &samplerState);
        _textureSamplerState2 = samplerState;
    }
    
    {
        sd.MaxLOD = FLT_MAX;
        sd.Filter = filterForMinAndMag("LINEAR", "LINEAR", true);
        
        ID3D11SamplerState* samplerState;
        getD3DDevice()->CreateSamplerState(&sd, &samplerState);
        _textureSamplerState3 = samplerState;
    }
    
    {
        sd.MaxLOD = FLT_MAX;
        sd.Filter = filterForMinAndMag("NEAREST", "LINEAR", true);
        
        ID3D11SamplerState* samplerState;
        getD3DDevice()->CreateSamplerState(&sd, &samplerState);
        _textureSamplerState4 = samplerState;
    }
    
    {
        sd.MaxLOD = FLT_MAX;
        sd.Filter = filterForMinAndMag("NEAREST", "NEAREST", false);
        
        ID3D11SamplerState* samplerState;
        getD3DDevice()->CreateSamplerState(&sd, &samplerState);
        _textureSamplerState5 = samplerState;
    }
    
    {
        sd.MaxLOD = FLT_MAX;
        sd.Filter = filterForMinAndMag("LINEAR", "NEAREST", false);
        
        ID3D11SamplerState* samplerState;
        getD3DDevice()->CreateSamplerState(&sd, &samplerState);
        _textureSamplerState6 = samplerState;
    }
    
    {
        sd.MaxLOD = FLT_MAX;
        sd.Filter = filterForMinAndMag("LINEAR", "LINEAR", false);
        
        ID3D11SamplerState* samplerState;
        getD3DDevice()->CreateSamplerState(&sd, &samplerState);
        _textureSamplerState7 = samplerState;
    }
    
    {
        sd.MaxLOD = FLT_MAX;
        sd.Filter = filterForMinAndMag("NEAREST", "LINEAR", false);
        
        ID3D11SamplerState* samplerState;
        getD3DDevice()->CreateSamplerState(&sd, &samplerState);
        _textureSamplerState8 = samplerState;
    }
}

#define GL_NEAREST 0
#define GL_LINEAR 1
#define GL_NEAREST_MIPMAP_NEAREST 2
#define GL_LINEAR_MIPMAP_NEAREST 3
#define GL_NEAREST_MIPMAP_LINEAR 4
#define GL_LINEAR_MIPMAP_LINEAR 5

D3D11_FILTER DirectXRendererHelper::filterForMinAndMag(std::string cfgFilterMin, std::string cfgFilterMag, bool mipmap)
{
    int filterMin;
    if (mipmap)
    {
        filterMin = cfgFilterMin == "NEAREST" ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR;
    }
    else
    {
        filterMin = cfgFilterMin == "NEAREST" ? GL_NEAREST : GL_LINEAR;
    }
    int filterMag = cfgFilterMag == "NEAREST" ? GL_NEAREST : GL_LINEAR;
    
    D3D11_FILTER_TYPE dxMin = D3D11_FILTER_TYPE_POINT;
    D3D11_FILTER_TYPE dxMip = D3D11_FILTER_TYPE_POINT;
    switch (filterMin)
    {
        case GL_NEAREST:                dxMin = D3D11_FILTER_TYPE_POINT;  dxMip = D3D11_FILTER_TYPE_POINT;  break;
        case GL_LINEAR:                 dxMin = D3D11_FILTER_TYPE_LINEAR; dxMip = D3D11_FILTER_TYPE_POINT;  break;
        case GL_NEAREST_MIPMAP_NEAREST: dxMin = D3D11_FILTER_TYPE_POINT;  dxMip = D3D11_FILTER_TYPE_POINT;  break;
        case GL_LINEAR_MIPMAP_NEAREST:  dxMin = D3D11_FILTER_TYPE_LINEAR; dxMip = D3D11_FILTER_TYPE_POINT;  break;
        case GL_NEAREST_MIPMAP_LINEAR:  dxMin = D3D11_FILTER_TYPE_POINT;  dxMip = D3D11_FILTER_TYPE_LINEAR; break;
        case GL_LINEAR_MIPMAP_LINEAR:   dxMin = D3D11_FILTER_TYPE_LINEAR; dxMip = D3D11_FILTER_TYPE_LINEAR; break;
        default: assert(false);
    }
    
    D3D11_FILTER_TYPE dxMag = D3D11_FILTER_TYPE_POINT;
    switch (filterMag)
    {
        case GL_NEAREST: dxMag = D3D11_FILTER_TYPE_POINT;  break;
        case GL_LINEAR:  dxMag = D3D11_FILTER_TYPE_LINEAR; break;
        default: assert(false);
    }
    
    return D3D11_ENCODE_BASIC_FILTER(dxMin, dxMag, dxMip, static_cast<D3D11_COMPARISON_FUNC>(0));
}

ID3D11SamplerState* DirectXRendererHelper::getSamplerStateForTexture(NGTexture* texture)
{
    if (texture->_isFramebuffer)
    {
        return _framebufferSamplerState;
    }
    
    NGTextureDesc* textureDesc = texture->_desc;
    
    std::string& cfgFilterMin = textureDesc->_textureFilterMin;
    std::string& cfgFilterMag = textureDesc->_textureFilterMag;
    bool mipmap = textureDesc->_textureFilterMipMap;
    
    int filterMin;
    if (mipmap)
    {
        filterMin = cfgFilterMin == "NEAREST" ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR;
    }
    else
    {
        filterMin = cfgFilterMin == "NEAREST" ? GL_NEAREST : GL_LINEAR;
    }
    int filterMag = cfgFilterMag == "NEAREST" ? GL_NEAREST : GL_LINEAR;
    
    if (     filterMin == GL_LINEAR_MIPMAP_NEAREST && filterMag == GL_NEAREST)
    {
        return _textureSamplerState1;
    }
    else if (filterMin == GL_LINEAR_MIPMAP_LINEAR && filterMag == GL_NEAREST)
    {
        return _textureSamplerState2;
    }
    else if (filterMin == GL_LINEAR_MIPMAP_LINEAR && filterMag == GL_LINEAR)
    {
        return _textureSamplerState3;
    }
    else if (filterMin == GL_LINEAR_MIPMAP_NEAREST && filterMag == GL_LINEAR)
    {
        return _textureSamplerState4;
    }
    else if (filterMin == GL_NEAREST && filterMag == GL_NEAREST)
    {
        return _textureSamplerState5;
    }
    else if (filterMin == GL_LINEAR && filterMag == GL_NEAREST)
    {
        return _textureSamplerState6;
    }
    else if (filterMin == GL_LINEAR && filterMag == GL_LINEAR)
    {
        return _textureSamplerState7;
    }
    else
    {
        // filterMin == GL_NEAREST && filterMag == GL_LINEAR
        return _textureSamplerState8;
    }
}

void DirectXRendererHelper::mapVertexBuffer(ID3D11Buffer* buffer, const void *data, size_t size, UINT stride)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    
    // Disable GPU access to the vertex buffer data.
    getD3DContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    // Update the vertex buffer here.
    memcpy(mappedResource.pData, data, size * stride);
    
    // Reenable GPU access to the vertex buffer data.
    getD3DContext()->Unmap(buffer, 0);
    
    bindVertexBuffer(buffer, stride);
}

void DirectXRendererHelper::bindVertexBuffer(ID3D11Buffer* buffer, UINT stride)
{
    // Set the vertex buffer
    UINT offset = 0;
    getD3DContext()->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
}

void DirectXRendererHelper::bindConstantBuffer(NGShaderUniformInput* uniform, const void *pSrcData)
{
	if (uniform->_isFragment)
	{
		getD3DContext()->PSSetConstantBuffers(uniform->_index, 1, &uniform->_constantbuffer);
	}
	else
	{
		getD3DContext()->VSSetConstantBuffers(uniform->_index, 1, &uniform->_constantbuffer);
	}

	getD3DContext()->UpdateSubresource(uniform->_constantbuffer, 0, 0, pSrcData, 0, 0);
}
