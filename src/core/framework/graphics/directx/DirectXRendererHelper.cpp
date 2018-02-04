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

ID3D11Device* DirectXRendererHelper::getD3dDevice()
{
    return s_deviceResources->GetD3DDevice();
}

ID3D11DeviceContext* DirectXRendererHelper::getD3dContext()
{
    return s_deviceResources->GetD3DDeviceContext();
}

DirectXRendererHelper::DirectXRendererHelper() : RendererHelper(), _fbIndex(0)
{
	// Empty
}

DirectXRendererHelper::~DirectXRendererHelper()
{
	releaseDeviceDependentResources();
}

void DirectXRendererHelper::createDeviceDependentResources()
{
    assert(_offscreenRenderTargets.size() == 0);
    assert(_offscreenRenderTargetViews.size() == 0);
    assert(_offscreenShaderResourceViews.size() == 0);
    
    createBlendStates();
    createSamplerStates();
    createVertexBuffer(_textureVertexBuffer, _textureVertices, MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
    createVertexBuffer(_colorVertexBuffer, _colorVertices, MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
    createVertexBuffer(_screenVertexBuffer, _screenVertices, 4);
    createIndexBuffer();
}

void DirectXRendererHelper::releaseDeviceDependentResources()
{
    releaseFramebuffers();
    
    _blendState.Reset();
    _screenBlendState.Reset();
    
    _textureSamplerState.Reset();
    _textureWrapSamplerState.Reset();
    
    _textureVertexBuffer.Reset();
    _colorVertexBuffer.Reset();
    _screenVertexBuffer.Reset();
    
    _indexbuffer.Reset();
}

NGTexture* DirectXRendererHelper::getFramebuffer(int index)
{
    _framebufferWrappers[index]->textureWrapper = _framebuffers[index];
    
    return _framebufferWrappers[index];
}

void DirectXRendererHelper::bindToOffscreenFramebuffer(int index)
{
    s_deviceResources->GetD3DDeviceContext()->OMSetRenderTargets(1, &_offscreenRenderTargetViews[index], NULL);
    
    // Set the viewport.
    auto viewport = s_deviceResources->GetOffScreenViewport();
    s_deviceResources->GetD3DDeviceContext()->RSSetViewports(1, &viewport);
    
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
    
	s_deviceResources->GetD3DDeviceContext()->ClearRenderTargetView(targets[0], color);
}

void DirectXRendererHelper::bindToScreenFramebuffer()
{
    ID3D11RenderTargetView *const targets[1] = { s_deviceResources->GetRenderTargetView() };
	s_deviceResources->GetD3DDeviceContext()->OMSetRenderTargets(1, targets, NULL);
    
    // Set the viewport.
    auto viewport = s_deviceResources->GetScreenViewport();
    s_deviceResources->GetD3DDeviceContext()->RSSetViewports(1, &viewport);
    
    _fbIndex = -1;
}

void DirectXRendererHelper::useNormalBlending()
{
    s_deviceResources->GetD3DDeviceContext()->OMSetBlendState(_blendState.Get(), 0, 0xffffffff);
}

void DirectXRendererHelper::useScreenBlending()
{
    s_deviceResources->GetD3DDeviceContext()->OMSetBlendState(_screenBlendState.Get(), 0, 0xffffffff);
}

void DirectXRendererHelper::useNoBlending()
{
    s_deviceResources->GetD3DDeviceContext()->OMSetBlendState(NULL, 0, 0xffffffff);
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

void DirectXRendererHelper::bindTexture(NGTextureSlot textureSlot, NGTexture* texture, NGShaderUniformInput* uniform)
{
    UNUSED(uniform);
    
    if (texture)
    {
        s_deviceResources->GetD3DDeviceContext()->PSSetShaderResources(textureSlot, 1, &texture->textureWrapper->texture);
		ID3D11SamplerState *const *ppSamplers = texture->_isFramebuffer ? _framebufferSamplerState.GetAddressOf() : texture->_repeatS ? _textureWrapSamplerState.GetAddressOf() : _textureSamplerState.GetAddressOf();
        s_deviceResources->GetD3DDeviceContext()->PSSetSamplers(textureSlot, 1, ppSamplers);
    }
    else
    {
        ID3D11ShaderResourceView *pSRV[1] = { NULL };
        s_deviceResources->GetD3DDeviceContext()->PSSetShaderResources(textureSlot, 1, pSRV);
    }
}

void DirectXRendererHelper::bindNGShader(ShaderProgramWrapper* shaderProgramWrapper)
{
    if (shaderProgramWrapper)
    {
        // set the shader objects as the active shaders
        s_deviceResources->GetD3DDeviceContext()->VSSetShader(shaderProgramWrapper->_vertexShader.Get(), NULL, 0);
        s_deviceResources->GetD3DDeviceContext()->IASetInputLayout(shaderProgramWrapper->_inputLayout.Get());
        s_deviceResources->GetD3DDeviceContext()->PSSetShader(shaderProgramWrapper->_pixelShader.Get(), NULL, 0);
    }
}

void DirectXRendererHelper::mapScreenVertices(std::vector<NGShaderVarInput*>& inputLayout, std::vector<VERTEX_2D>& vertices)
{
	_screenVertices.clear();
    _screenVertices.insert(_screenVertices.end(), vertices.begin(), vertices.end());

    mapVertices(_screenVertexBuffer, _screenVertices);
}

void DirectXRendererHelper::mapTextureVertices(std::vector<NGShaderVarInput*>& inputLayout, std::vector<VERTEX_2D_TEXTURE>& vertices)
{
	_textureVertices.clear();
    _textureVertices.insert(_textureVertices.end(), vertices.begin(), vertices.end());

    mapVertices(_textureVertexBuffer, _textureVertices);
}

void DirectXRendererHelper::mapColorVertices(std::vector<NGShaderVarInput*>& inputLayout, std::vector<VERTEX_2D>& vertices)
{
	_colorVertices.clear();
    _colorVertices.insert(_colorVertices.end(), vertices.begin(), vertices.end());

    mapVertices(_colorVertexBuffer, _colorVertices);
}

void DirectXRendererHelper::draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count)
{
    s_deviceResources->GetD3DDeviceContext()->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(renderPrimitiveType));
    s_deviceResources->GetD3DDeviceContext()->Draw(count, first);
}

void DirectXRendererHelper::drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count)
{
    s_deviceResources->GetD3DDeviceContext()->IASetIndexBuffer(_indexbuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    
    s_deviceResources->GetD3DDeviceContext()->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(renderPrimitiveType));
    s_deviceResources->GetD3DDeviceContext()->DrawIndexed(count, first, 0);
}

void DirectXRendererHelper::createFramebufferObject()
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
    DX::ThrowIfFailed(s_deviceResources->GetD3DDevice()->CreateTexture2D(&textureDesc, NULL, &_offscreenRenderTarget));
    
    // Setup the description of the render target view.
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;
    
    // Create the render target view.
    DX::ThrowIfFailed(s_deviceResources->GetD3DDevice()->CreateRenderTargetView(_offscreenRenderTarget, &renderTargetViewDesc, &_offscreenRenderTargetView));
    
    // Setup the description of the shader resource view.
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    
    // Create the shader resource view.
    DX::ThrowIfFailed(s_deviceResources->GetD3DDevice()->CreateShaderResourceView(_offscreenRenderTarget, &shaderResourceViewDesc, &_offscreenShaderResourceView));
    
    _offscreenRenderTargets.push_back(_offscreenRenderTarget);
    _offscreenRenderTargetViews.push_back(_offscreenRenderTargetView);
    _offscreenShaderResourceViews.push_back(_offscreenShaderResourceView);
    
    _framebuffers.push_back(new TextureWrapper(_offscreenShaderResourceView));
}

void DirectXRendererHelper::releaseFramebuffers()
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
    
    s_deviceResources->GetD3DDevice()->CreateBlendState(&bd, &_blendState);
    
    bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    
    s_deviceResources->GetD3DDevice()->CreateBlendState(&bd, &_screenBlendState);
}

void DirectXRendererHelper::createSamplerStates()
{
    D3D11_SAMPLER_DESC sd;
    sd.Filter = D3D11_FILTER_ANISOTROPIC;
    sd.MaxAnisotropy = 16;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.BorderColor[0] = 0.0f;
    sd.BorderColor[1] = 0.0f;
    sd.BorderColor[2] = 0.0f;
    sd.BorderColor[3] = 0.0f;
    sd.MinLOD = 0.0f;
    sd.MaxLOD = FLT_MAX;
    sd.MipLODBias = 0.0f;
    
    {
        std::string cfgFilterMin = NG_CFG->getString("TextureFilterMin");
        std::string cfgFilterMag = NG_CFG->getString("TextureFilterMag");
        
        sd.Filter = filterForMinAndMag(cfgFilterMin, cfgFilterMag);
        
        sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        s_deviceResources->GetD3DDevice()->CreateSamplerState(&sd, _textureWrapSamplerState.GetAddressOf());
        
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        s_deviceResources->GetD3DDevice()->CreateSamplerState(&sd, _textureSamplerState.GetAddressOf());
    }
    
    {
        std::string cfgFilterMin = NG_CFG->getString("FramebufferFilterMin");
        std::string cfgFilterMag = NG_CFG->getString("FramebufferFilterMag");
        
        sd.Filter = filterForMinAndMag(cfgFilterMin, cfgFilterMag);
        
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        s_deviceResources->GetD3DDevice()->CreateSamplerState(&sd, _framebufferSamplerState.GetAddressOf());
    }
}

D3D11_FILTER DirectXRendererHelper::filterForMinAndMag(std::string& cfgFilterMin, std::string& cfgFilterMag)
{
    static const int GL_NEAREST = 0;
    static const int GL_LINEAR = 1;
    static const int GL_NEAREST_MIPMAP_NEAREST = 2;
    static const int GL_LINEAR_MIPMAP_NEAREST = 3;
    static const int GL_NEAREST_MIPMAP_LINEAR = 4;
    static const int GL_LINEAR_MIPMAP_LINEAR = 5;
    
    int filterMin = cfgFilterMin == "NEAREST" ? GL_NEAREST : GL_LINEAR;
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

void DirectXRendererHelper::createIndexBuffer()
{
    D3D11_BUFFER_DESC indexBufferDesc = { 0 };
    
    indexBufferDesc.ByteWidth = sizeof(short) * MAX_BATCH_SIZE * INDICES_PER_RECTANGLE;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    
    D3D11_SUBRESOURCE_DATA indexDataDesc = { 0 };
    
    indexDataDesc.pSysMem = &_indices[0];
    
    DX::ThrowIfFailed(s_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexDataDesc, &_indexbuffer));
}

void DirectXRendererHelper::bindConstantBuffer(NGShaderUniformInput* uniform, const void *pSrcData)
{
	if (uniform->_isFragment)
	{
		s_deviceResources->GetD3DDeviceContext()->PSSetConstantBuffers(uniform->_index, 1, uniform->_constantbuffer.GetAddressOf());
	}
	else
	{
		s_deviceResources->GetD3DDeviceContext()->VSSetConstantBuffers(uniform->_index, 1, uniform->_constantbuffer.GetAddressOf());
	}

	s_deviceResources->GetD3DDeviceContext()->UpdateSubresource(uniform->_constantbuffer.Get(), 0, 0, pSrcData, 0, 0);
}
