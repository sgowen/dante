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

ID3D11Device* DirectXRendererHelper::s_d3dDevice = NULL;
ID3D11DeviceContext* DirectXRendererHelper::s_d3dContext = NULL;
ID3D11RenderTargetView* DirectXRendererHelper::s_d3dRenderTargetView = NULL;

void DirectXRendererHelper::init(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dContext, ID3D11RenderTargetView* d3dRenderTargetView)
{
    s_d3dDevice = d3dDevice;
    s_d3dContext = d3dContext;
    s_d3dRenderTargetView = d3dRenderTargetView;
}

ID3D11Device* DirectXRendererHelper::getD3dDevice()
{
    return s_d3dDevice;
}

ID3D11DeviceContext* DirectXRendererHelper::getD3dContext()
{
    return s_d3dContext;
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
    
    _sbSamplerState.Reset();
    _sbWrapSamplerState.Reset();
    
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
    s_d3dContext->OMSetRenderTargets(1, &_offscreenRenderTargetViews[index], NULL);
    
	_fbIndex = index;
}

void DirectXRendererHelper::clearFramebufferWithColor(float r, float g, float b, float a)
{
    float color[] = { r, g, b, a };

    ID3D11RenderTargetView * targets[1] = {};
    if (_fbIndex < 0)
    {
		targets[0] = s_d3dRenderTargetView;
    }
    else
    {
        targets[0] = _offscreenRenderTargetViews[_fbIndex];
    }
    
	s_d3dContext->ClearRenderTargetView(targets[0], color);
}

void DirectXRendererHelper::bindToScreenFramebuffer()
{
    ID3D11RenderTargetView *const targets[1] = { s_d3dRenderTargetView };
	s_d3dContext->OMSetRenderTargets(1, targets, NULL);
    
    _fbIndex = -1;
}

void DirectXRendererHelper::useNormalBlending()
{
    s_d3dContext->OMSetBlendState(_blendState.Get(), 0, 0xffffffff);
}

void DirectXRendererHelper::useScreenBlending()
{
    s_d3dContext->OMSetBlendState(_screenBlendState.Get(), 0, 0xffffffff);
}

void DirectXRendererHelper::useNoBlending()
{
    s_d3dContext->OMSetBlendState(NULL, 0, 0xffffffff);
}

void DirectXRendererHelper::bindInt4(NGShaderUniformInput* uniform, int4& inValue)
{
    if (uniform->_isFragment)
    {
        s_d3dContext->PSSetConstantBuffers(uniform->_index, 1, uniform->_constantbuffer.GetAddressOf());
    }
    else
    {
        s_d3dContext->VSSetConstantBuffers(uniform->_index, 1, uniform->_constantbuffer.GetAddressOf());
    }
    
    // send the final matrix to video memory
    s_d3dContext->UpdateSubresource(uniform->_constantbuffer.Get(), 0, 0, &inValue, 0, 0);
}

void DirectXRendererHelper::bindFloat4(NGShaderUniformInput* uniform, float4& inValue)
{
    if (uniform->_isFragment)
    {
        s_d3dContext->PSSetConstantBuffers(uniform->_index, 1, uniform->_constantbuffer.GetAddressOf());
    }
    else
    {
        s_d3dContext->VSSetConstantBuffers(uniform->_index, 1, uniform->_constantbuffer.GetAddressOf());
    }
    
    // send the final matrix to video memory
    s_d3dContext->UpdateSubresource(uniform->_constantbuffer.Get(), 0, 0, &inValue, 0, 0);
}

void DirectXRendererHelper::bindMatrix(NGShaderUniformInput* uniform, mat4x4& inValue)
{
    if (uniform->_isFragment)
    {
        s_d3dContext->PSSetConstantBuffers(uniform->_index, 1, uniform->_constantbuffer.GetAddressOf());
    }
    else
    {
        s_d3dContext->VSSetConstantBuffers(uniform->_index, 1, uniform->_constantbuffer.GetAddressOf());
    }
    
    // send the final matrix to video memory
    s_d3dContext->UpdateSubresource(uniform->_constantbuffer.Get(), 0, 0, &inValue, 0, 0);
}

void DirectXRendererHelper::bindMatrix(NGShaderUniformInput* uniform)
{
    s_d3dContext->VSSetConstantBuffers(uniform->_index, 1, uniform->_constantbuffer.GetAddressOf());
    
    // send the final matrix to video memory
    s_d3dContext->UpdateSubresource(uniform->_constantbuffer.Get(), 0, 0, &_matrix, 0, 0);
}

void DirectXRendererHelper::bindTexture(NGTextureSlot textureSlot, NGTexture* texture, NGShaderUniformInput* uniform)
{
    UNUSED(uniform);
    
    if (texture)
    {
        s_d3dContext->PSSetShaderResources(textureSlot, 1, &texture->textureWrapper->texture);
        s_d3dContext->PSSetSamplers(textureSlot, 1, texture->_repeatS ? _sbWrapSamplerState.GetAddressOf() : _sbSamplerState.GetAddressOf());
    }
    else
    {
        ID3D11ShaderResourceView *pSRV[1] = { NULL };
        s_d3dContext->PSSetShaderResources(textureSlot, 1, pSRV);
    }
}

void DirectXRendererHelper::bindNGShader(ShaderProgramWrapper* shaderProgramWrapper)
{
    if (shaderProgramWrapper)
    {
        // set the shader objects as the active shaders
        s_d3dContext->VSSetShader(shaderProgramWrapper->_vertexShader.Get(), NULL, 0);
        s_d3dContext->IASetInputLayout(shaderProgramWrapper->_inputLayout.Get());
        s_d3dContext->PSSetShader(shaderProgramWrapper->_pixelShader.Get(), NULL, 0);
    }
}

void DirectXRendererHelper::mapScreenVertices(std::vector<NGShaderVarInput*>& inputLayout, std::vector<VERTEX_2D>& vertices)
{
	_screenVertices.clear();
	_screenVertices.swap(vertices);

    mapVertices(_screenVertexBuffer, _screenVertices);
}

void DirectXRendererHelper::mapTextureVertices(std::vector<NGShaderVarInput*>& inputLayout, std::vector<VERTEX_2D_TEXTURE>& vertices)
{
	_textureVertices.clear();
	_textureVertices.swap(vertices);

    mapVertices(_textureVertexBuffer, _textureVertices);
}

void DirectXRendererHelper::mapColorVertices(std::vector<NGShaderVarInput*>& inputLayout, std::vector<VERTEX_2D>& vertices)
{
	_colorVertices.clear();
	_colorVertices.swap(vertices);

    mapVertices(_colorVertexBuffer, _colorVertices);
}

void DirectXRendererHelper::draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count)
{
    s_d3dContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(renderPrimitiveType));
    s_d3dContext->Draw(count, first);
}

void DirectXRendererHelper::drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count)
{
    s_d3dContext->IASetIndexBuffer(_indexbuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    
    s_d3dContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(renderPrimitiveType));
    s_d3dContext->DrawIndexed(count, first, 0);
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
    DX::ThrowIfFailed(s_d3dDevice->CreateTexture2D(&textureDesc, NULL, &_offscreenRenderTarget));
    
    // Setup the description of the render target view.
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;
    
    // Create the render target view.
    DX::ThrowIfFailed(s_d3dDevice->CreateRenderTargetView(_offscreenRenderTarget, &renderTargetViewDesc, &_offscreenRenderTargetView));
    
    // Setup the description of the shader resource view.
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    
    // Create the shader resource view.
    DX::ThrowIfFailed(s_d3dDevice->CreateShaderResourceView(_offscreenRenderTarget, &shaderResourceViewDesc, &_offscreenShaderResourceView));
    
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
    
    s_d3dDevice->CreateBlendState(&bd, &_blendState);
    
    bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    
    s_d3dDevice->CreateBlendState(&bd, &_screenBlendState);
}

void DirectXRendererHelper::createSamplerStates()
{
    std::string textureFilterMin = NG_CFG->getString("TextureFilterMin");
    std::string textureFilterMax = NG_CFG->getString("TextureFilterMax");
    int minFilter;
    if (mipmap)
    {
        minFilter = textureFilterMin == "NEAREST" ? 4 : 5;
    }
    else
    {
        minFilter = textureFilterMin == "NEAREST" ? 0 : 1;
    }
    int maxFilter = textureFilterMax == "NEAREST" ? 0 : 1;
    
    D3D11_FILTER_TYPE dxMin = D3D11_FILTER_TYPE_POINT;
    D3D11_FILTER_TYPE dxMip = D3D11_FILTER_TYPE_POINT;
    switch (minFilter)
    {
        case 0: dxMin = D3D11_FILTER_TYPE_POINT;  dxMip = D3D11_FILTER_TYPE_POINT;  break;
        case 1: dxMin = D3D11_FILTER_TYPE_LINEAR; dxMip = D3D11_FILTER_TYPE_POINT;  break;
        case 2: dxMin = D3D11_FILTER_TYPE_POINT;  dxMip = D3D11_FILTER_TYPE_POINT;  break;
        case 3: dxMin = D3D11_FILTER_TYPE_LINEAR; dxMip = D3D11_FILTER_TYPE_POINT;  break;
        case 4: dxMin = D3D11_FILTER_TYPE_POINT;  dxMip = D3D11_FILTER_TYPE_LINEAR; break;
        case 5: dxMin = D3D11_FILTER_TYPE_LINEAR; dxMip = D3D11_FILTER_TYPE_LINEAR; break;
        default: assert(false);
    }
    D3D11_FILTER_TYPE dxMag = D3D11_FILTER_TYPE_POINT;
    switch (magFilter)
    {
        case 0: dxMag = D3D11_FILTER_TYPE_POINT;  break;
        case 1: dxMag = D3D11_FILTER_TYPE_LINEAR; break;
        default: assert(false);
    }
    
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
    sd.MaxLOD = 0.0f;
    sd.MipLODBias = 0.0f;
    sd.Filter = D3D11_ENCODE_BASIC_FILTER(dxMin, dxMag, dxMip, static_cast<D3D11_COMPARISON_FUNC>(false));
    
    s_d3dDevice->CreateSamplerState(&sd, _sbSamplerState.GetAddressOf());
    
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    
    s_d3dDevice->CreateSamplerState(&sd, _sbWrapSamplerState.GetAddressOf());
}

void DirectXRendererHelper::createIndexBuffer()
{
    D3D11_BUFFER_DESC indexBufferDesc = { 0 };
    
    indexBufferDesc.ByteWidth = sizeof(short) * MAX_BATCH_SIZE * INDICES_PER_RECTANGLE;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    
    D3D11_SUBRESOURCE_DATA indexDataDesc = { 0 };
    
    indexDataDesc.pSysMem = &_indices[0];
    
    DX::ThrowIfFailed(s_d3dDevice->CreateBuffer(&indexBufferDesc, &indexDataDesc, &_indexbuffer));
}
