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

ID3D11Device* DirectXRendererHelper::getD3DDevice()
{
    return s_deviceResources->GetD3DDevice();
}

ID3D11DeviceContext* DirectXRendererHelper::getD3DContext()
{
    return s_deviceResources->GetD3DDeviceContext();
}

DirectXRendererHelper::DirectXRendererHelper() : RendererHelper(),
_textureVertexBuffer(NULL),
_vertexBuffer(NULL),
_indexbuffer(NULL),
_blendState(NULL),
_screenBlendState(NULL),
_textureSamplerState(NULL),
_textureWrapSamplerState(NULL),
_framebufferSamplerState(NULL),
_fbIndex(0)
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
    
    createVertexBuffer<VERTEX_2D_TEXTURE>(&_textureVertexBuffer, MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
    createVertexBuffer<VERTEX_2D>(&_vertexBuffer, MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE);
    createIndexBuffer();
    
    createBlendStates();
    createSamplerStates();
}

void DirectXRendererHelper::releaseDeviceDependentResources()
{
    platformReleaseFramebuffers();
    
    if (_textureVertexBuffer &&
        _vertexBuffer &&
        _indexbuffer &&
        _blendState &&
        _screenBlendState &&
        _textureSamplerState &&
        _textureWrapSamplerState &&
        _framebufferSamplerState)
    {
        _textureVertexBuffer->Release();
        _vertexBuffer->Release();
        _indexbuffer->Release();
        _blendState->Release();
        _screenBlendState->Release();
        _textureSamplerState->Release();
        _textureWrapSamplerState->Release();
        _framebufferSamplerState->Release();
        
        _textureVertexBuffer = NULL;
        _vertexBuffer = NULL;
        _indexbuffer = NULL;
        _blendState = NULL;
        _screenBlendState = NULL;
        _textureSamplerState = NULL;
        _textureWrapSamplerState = NULL;
        _framebufferSamplerState = NULL;
    }
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
		ID3D11SamplerState *const *ppSamplers = texture->_isFramebuffer ? &_framebufferSamplerState : texture->_repeatS ? &_textureWrapSamplerState : &_textureSamplerState;
        getD3DContext()->PSSetSamplers(textureSlot, 1, ppSamplers);
    }
    else
    {
        ID3D11ShaderResourceView *pSRV[1] = { NULL };
        getD3DContext()->PSSetShaderResources(textureSlot, 1, pSRV);
    }
}

void DirectXRendererHelper::mapTextureVertices(std::vector<VERTEX_2D_TEXTURE>& vertices)
{
	mapVertices(_textureVertexBuffer, vertices);
}

void DirectXRendererHelper::mapVertices(std::vector<VERTEX_2D>& vertices)
{
	mapVertices(_vertexBuffer, vertices);
}

void DirectXRendererHelper::draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count)
{
    getD3DContext()->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(renderPrimitiveType));
    getD3DContext()->Draw(count, first);
}

void DirectXRendererHelper::drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count)
{
    getD3DContext()->IASetIndexBuffer(_indexbuffer, DXGI_FORMAT_R16_UINT, 0);
    
    getD3DContext()->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(renderPrimitiveType));
    getD3DContext()->DrawIndexed(count, first, 0);
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

void DirectXRendererHelper::createIndexBuffer()
{
    size_t size = MAX_BATCH_SIZE * INDICES_PER_RECTANGLE;
    std::vector<uint16_t> indices;
    indices.reserve(size);
    
    uint16_t j = 0;
    for (int i = 0; i < size; i += INDICES_PER_RECTANGLE, j += VERTICES_PER_RECTANGLE)
    {
        indices.push_back(j);
        indices.push_back(j + 1);
        indices.push_back(j + 2);
        indices.push_back(j + 2);
        indices.push_back(j + 3);
        indices.push_back(j + 0);
    }
    
    D3D11_BUFFER_DESC indexBufferDesc = { 0 };
    indexBufferDesc.ByteWidth = sizeof(uint16_t) * indices.size();
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    
    D3D11_SUBRESOURCE_DATA indexDataDesc = { 0 };
    indexDataDesc.pSysMem = &indices[0];
    
    ID3D11Buffer* indexbuffer;
    DX::ThrowIfFailed(getD3DDevice()->CreateBuffer(&indexBufferDesc, &indexDataDesc, &indexbuffer));
    _indexbuffer = indexbuffer;
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
        
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        ID3D11SamplerState* textureSamplerState;
        getD3DDevice()->CreateSamplerState(&sd, &textureSamplerState);
        _textureSamplerState = textureSamplerState;
        
        sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        ID3D11SamplerState* textureWrapSamplerState;
        getD3DDevice()->CreateSamplerState(&sd, &textureWrapSamplerState);
        _textureWrapSamplerState = textureWrapSamplerState;
    }
    
    {
        std::string cfgFilterMin = NG_CFG->getString("FramebufferFilterMin");
        std::string cfgFilterMag = NG_CFG->getString("FramebufferFilterMag");
        
        sd.Filter = filterForMinAndMag(cfgFilterMin, cfgFilterMag);
        
        sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        ID3D11SamplerState* framebufferSamplerState;
        getD3DDevice()->CreateSamplerState(&sd, &framebufferSamplerState);
        _framebufferSamplerState = framebufferSamplerState;
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
