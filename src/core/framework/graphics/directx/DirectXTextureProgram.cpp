//
//  DirectXTextureProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXTextureProgram.h"

#include "framework/graphics/directx/DirectXRendererHelper.h"

#include <assert.h>

const D3D11_INPUT_ELEMENT_DESC DirectXTextureProgram::VERTEX_DESC[3] = {
    { "a_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "a_Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "a_TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

DirectXTextureProgram::DirectXTextureProgram(DirectXRendererHelper* inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName) : DirectXProgram(inRendererHelper, vertexShaderName, fragmentShaderName, VERTEX_DESC, ARRAYSIZE(VERTEX_DESC))
{
    // Empty
}

void DirectXTextureProgram::bind(void* data)
{
    assert(data != NULL);
    
    // set the shader objects as the active shaders
    _d3dContext->VSSetShader(_vertexShader.Get(), NULL, 0);
    _d3dContext->IASetInputLayout(_inputLayout.Get());
    _d3dContext->PSSetShader(_pixelShader.Get(), NULL, 0);
    
    _rendererHelper->useNormalBlending();
    
    _d3dContext->VSSetConstantBuffers(0, 1, _rendererHelper->getMatrixConstantbuffer().GetAddressOf());
    
    // send the final matrix to video memory
    _d3dContext->UpdateSubresource(_rendererHelper->getMatrixConstantbuffer().Get(), 0, 0, &_rendererHelper->getMatrix(), 0, 0);
    
    // tell the GPU which texture to use
    _rendererHelper->bindTexture(NGTextureSlot_ZERO, static_cast<NGTexture*>(data));
    
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    
    //	Disable GPU access to the vertex buffer data.
    _d3dContext->Map(_rendererHelper->getTextureVertexBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    //	Update the vertex buffer here.
    std::vector<TEXTURE_VERTEX>& vertices = _rendererHelper->getTextureVertices();
    memcpy(mappedResource.pData, &vertices.front(), sizeof(TEXTURE_VERTEX) * vertices.size());
    
    //	Reenable GPU access to the vertex buffer data.
    _d3dContext->Unmap(_rendererHelper->getTextureVertexBuffer().Get(), 0);
    
    // Set the vertex buffer
    UINT stride = sizeof(TEXTURE_VERTEX);
    UINT offset = 0;
    _d3dContext->IASetVertexBuffers(0, 1, _rendererHelper->getTextureVertexBuffer().GetAddressOf(), &stride, &offset);
}

void DirectXTextureProgram::unbind()
{
    _rendererHelper->bindTexture(NGTextureSlot_ZERO, NULL);
}
