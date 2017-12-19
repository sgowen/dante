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

#include "framework/util/NGSTDUtil.h"
#include "PlatformHelpers.h"
#include <framework/util/FrameworkConstants.h>
#include <framework/util/macros.h>

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

DirectXRendererHelper::DirectXRendererHelper() : RendererHelper(), _framebufferIndex(0), _isBoundToScreen(false)
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
    createVertexBufferForSpriteBatcher();
    createVertexBufferForGeometryBatchers();
    createVertexBufferForScreen();
    createIndexBuffer();
    createConstantBuffer();
}

void DirectXRendererHelper::createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int numFramebuffers)
{
    _screenWidth = screenWidth;
    _screenHeight = screenHeight;
    _renderWidth = renderWidth;
    _renderHeight = renderHeight;
    _numFramebuffers = numFramebuffers;
    
    releaseFramebuffers();
    createFramebufferObjects();
}

void DirectXRendererHelper::releaseDeviceDependentResources()
{
    releaseFramebuffers();
    
    _blendState.Reset();
    _screenBlendState.Reset();
    _matrixConstantbuffer.Reset();
    _indexbuffer.Reset();
    
    _sbSamplerState.Reset();
    _sbWrapSamplerState.Reset();
    _textureVertexBuffer.Reset();
    _colorVertexBuffer.Reset();
    _screenVertexBuffer.Reset();
    
    _textureVertices.clear();
    _colorVertices.clear();
}

void DirectXRendererHelper::beginFrame()
{
    // Empty
}

void DirectXRendererHelper::endFrame()
{
    // Empty
}

NGTexture* DirectXRendererHelper::getFramebuffer(int index)
{
    _framebuffer->textureWrapper = _framebuffers[index];
    
    return _framebuffer;
}

void DirectXRendererHelper::updateMatrix(float left, float right, float bottom, float top)
{
    using namespace DirectX;
    
    XMMATRIX matFinal = XMMatrixOrthographicOffCenterRH(left, right, bottom, top, -1.0, 1.0);
    
    XMStoreFloat4x4(&_matFinal, matFinal);
}

void DirectXRendererHelper::bindToOffscreenFramebuffer(int index)
{
    s_d3dContext->OMSetRenderTargets(1, &_offscreenRenderTargetViews[index], NULL);
    
	_framebufferIndex = index;
    _isBoundToScreen = false;
}

void DirectXRendererHelper::clearFramebufferWithColor(float r, float g, float b, float a)
{
    float color[] = { r, g, b, a };

    ID3D11RenderTargetView * targets[1] = {};
    if (_isBoundToScreen)
    {
		targets[0] = s_d3dRenderTargetView;
    }
    else
    {
        targets[0] = _offscreenRenderTargetViews[_framebufferIndex];
    }
    
	s_d3dContext->ClearRenderTargetView(targets[0], color);
}

void DirectXRendererHelper::bindToScreenFramebuffer()
{
    ID3D11RenderTargetView *const targets[1] = { s_d3dRenderTargetView };
	s_d3dContext->OMSetRenderTargets(1, targets, NULL);
    
    _isBoundToScreen = true;
}

void DirectXRendererHelper::useNormalBlending()
{
    s_d3dContext->OMSetBlendState(_blendState.Get(), 0, 0xffffffff);
}

void DirectXRendererHelper::useScreenBlending()
{
    s_d3dContext->OMSetBlendState(_screenBlendState.Get(), 0, 0xffffffff);
}

void DirectXRendererHelper::bindMatrix(NGShaderUniformInput* uniform)
{
    UNUSED(uniform);
    
    s_d3dContext->VSSetConstantBuffers(0, 1, _matrixConstantbuffer.GetAddressOf());
    
    // send the final matrix to video memory
	s_d3dContext->UpdateSubresource(_matrixConstantbuffer.Get(), 0, 0, &_matFinal, 0, 0);
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

void DirectXRendererHelper::destroyTexture(TextureWrapper& textureWrapper)
{
    if (textureWrapper.texture)
    {
        textureWrapper.texture->Release();
    }
}

void DirectXRendererHelper::bindShaderProgram(ShaderProgramWrapper* shaderProgramWrapper)
{
    if (shaderProgramWrapper != NULL)
    {
        // set the shader objects as the active shaders
        s_d3dContext->VSSetShader(shaderProgramWrapper->_vertexShader, NULL, 0);
        s_d3dContext->IASetInputLayout(shaderProgramWrapper->_inputLayout);
        s_d3dContext->PSSetShader(shaderProgramWrapper->_pixelShader, NULL, 0);
    }
}

void DirectXRendererHelper::destroyShaderProgram(ShaderProgramWrapper* shaderProgramWrapper)
{
    assert(shaderProgramWrapper != NULL);
    assert(shaderProgramWrapper->_vertexShader != NULL);
    assert(shaderProgramWrapper->_inputLayout != NULL);
    assert(shaderProgramWrapper->_pixelShader != NULL);
    
    delete shaderProgramWrapper->_vertexShader;
    shaderProgramWrapper->_vertexShader = NULL;
    
    delete shaderProgramWrapper->_inputLayout;
    shaderProgramWrapper->_inputLayout = NULL;
    
    delete shaderProgramWrapper->_pixelShader;
    shaderProgramWrapper->_pixelShader = NULL;
}

void DirectXRendererHelper::mapScreenVertices(std::vector<NGShaderVarInput*>& inputLayout)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    
    //    Disable GPU access to the vertex buffer data.
	s_d3dContext->Map(_screenVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    //    Update the vertex buffer here.
    memcpy(mappedResource.pData, &_screenVertices.front(), sizeof(SCREEN_VERTEX) * _screenVertices.size());
    
    //    Reenable GPU access to the vertex buffer data.
	s_d3dContext->Unmap(_screenVertexBuffer.Get(), 0);
    
    // Set the vertex buffer
    UINT stride = sizeof(SCREEN_VERTEX);
    UINT offset = 0;
	s_d3dContext->IASetVertexBuffers(0, 1, _screenVertexBuffer.GetAddressOf(), &stride, &offset);
}

void DirectXRendererHelper::unmapScreenVertices()
{
    // Unused
}

void DirectXRendererHelper::mapTextureVertices(std::vector<NGShaderVarInput*>& inputLayout)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    
    //    Disable GPU access to the vertex buffer data.
	s_d3dContext->Map(_textureVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    //    Update the vertex buffer here.
    memcpy(mappedResource.pData, &_textureVertices.front(), sizeof(TEXTURE_VERTEX) * _textureVertices.size());
    
    //    Reenable GPU access to the vertex buffer data.
	s_d3dContext->Unmap(_textureVertexBuffer.Get(), 0);
    
    // Set the vertex buffer
    UINT stride = sizeof(TEXTURE_VERTEX);
    UINT offset = 0;
	s_d3dContext->IASetVertexBuffers(0, 1, _textureVertexBuffer.GetAddressOf(), &stride, &offset);
}

void DirectXRendererHelper::unmapTextureVertices()
{
    // Unused
}

void DirectXRendererHelper::mapColorVertices(std::vector<NGShaderVarInput*>& inputLayout)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    
    //    Disable GPU access to the vertex buffer data.
	s_d3dContext->Map(_colorVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    //    Update the vertex buffer here.
    memcpy(mappedResource.pData, &_colorVertices.front(), sizeof(COLOR_VERTEX) * _colorVertices.size());
    
    //    Reenable GPU access to the vertex buffer data.
	s_d3dContext->Unmap(_colorVertexBuffer.Get(), 0);
    
    // Set the vertex buffer
    UINT stride = sizeof(COLOR_VERTEX);
    UINT offset = 0;
	s_d3dContext->IASetVertexBuffers(0, 1, _colorVertexBuffer.GetAddressOf(), &stride, &offset);
}

void DirectXRendererHelper::unmapColorVertices()
{
    // Unused
}

void DirectXRendererHelper::draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count)
{
    s_d3dContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(renderPrimitiveType));
    s_d3dContext->Draw(count, first);
}

void DirectXRendererHelper::drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t count)
{
    s_d3dContext->IASetIndexBuffer(_indexbuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    
    s_d3dContext->IASetPrimitiveTopology(static_cast<D3D11_PRIMITIVE_TOPOLOGY>(renderPrimitiveType));
    s_d3dContext->DrawIndexed(count, 0, 0);
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

void DirectXRendererHelper::createSamplerStates()
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

void DirectXRendererHelper::createVertexBufferForSpriteBatcher()
{
    TEXTURE_VERTEX tv = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE; ++i)
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
    
    DirectX::ThrowIfFailed(s_d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &_textureVertexBuffer));
}

void DirectXRendererHelper::createVertexBufferForGeometryBatchers()
{
    COLOR_VERTEX cv = { 0, 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < MAX_BATCH_SIZE * VERTICES_PER_RECTANGLE; ++i)
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
    
    DirectX::ThrowIfFailed(s_d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &_colorVertexBuffer));
}

void DirectXRendererHelper::createVertexBufferForScreen()
{
    SCREEN_VERTEX tv = { 0, 0 };
    for (int i = 0; i < 4; ++i)
    {
        _screenVertices.push_back(tv);
    }
    
    D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
    vertexBufferDesc.ByteWidth = sizeof(SCREEN_VERTEX) * _screenVertices.size();
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;
    
    D3D11_SUBRESOURCE_DATA vertexBufferData;
    vertexBufferData.pSysMem = &_screenVertices[0];
    vertexBufferData.SysMemPitch = 0;
    vertexBufferData.SysMemSlicePitch = 0;
    
    DirectX::ThrowIfFailed(s_d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &_screenVertexBuffer));
}

void DirectXRendererHelper::createIndexBuffer()
{
    D3D11_BUFFER_DESC indexBufferDesc = { 0 };
    
    indexBufferDesc.ByteWidth = sizeof(short) * MAX_BATCH_SIZE * INDICES_PER_RECTANGLE;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    
    D3D11_SUBRESOURCE_DATA indexDataDesc = { 0 };
    
    indexDataDesc.pSysMem = &_indices[0];
    
    s_d3dDevice->CreateBuffer(&indexBufferDesc, &indexDataDesc, &_indexbuffer);
}

void DirectXRendererHelper::createConstantBuffer()
{
    D3D11_BUFFER_DESC bd = { 0 };
    
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = 64;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    
    DirectX::ThrowIfFailed(s_d3dDevice->CreateBuffer(&bd, NULL, &_matrixConstantbuffer));
}
