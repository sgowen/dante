//
//  DirectXFramebufferToScreenProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXFramebufferToScreenProgram.h"

#include "framework/graphics/directx/DirectXRendererHelper.h"

#include <assert.h>

const D3D11_INPUT_ELEMENT_DESC DirectXFramebufferToScreenProgram::VERTEX_DESC[3] = {
    { "a_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "a_Color", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "a_TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

DirectXFramebufferToScreenProgram::DirectXFramebufferToScreenProgram(DirectXRendererHelper* inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName) : DirectXProgram(inRendererHelper, vertexShaderName, fragmentShaderName, VERTEX_DESC, ARRAYSIZE(VERTEX_DESC))
{
    // Empty
}

void DirectXFramebufferToScreenProgram::bind(void* data)
{
    assert(data != NULL);
    
    // set the shader objects as the active shaders
    _d3dContext->VSSetShader(_vertexShader.Get(), NULL, 0);
    _d3dContext->IASetInputLayout(_inputLayout.Get());
    _d3dContext->PSSetShader(_pixelShader.Get(), NULL, 0);
    
    _rendererHelper->useScreenBlending();
    
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    
    // tell the GPU which texture to use
    _rendererHelper->bindTexture(NGTextureSlot_ZERO, static_cast<TextureWrapper*>(data));
    
    //	Disable GPU access to the vertex buffer data.
    _d3dContext->Map(_rendererHelper->getSbVertexBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    //	Update the vertex buffer here.
    std::vector<TEXTURE_VERTEX>& vertices = _rendererHelper->getTextureVertices();
    memcpy(mappedResource.pData, &vertices.front(), sizeof(TEXTURE_VERTEX) * vertices.size());
    
    //	Reenable GPU access to the vertex buffer data.
    _d3dContext->Unmap(_rendererHelper->getSbVertexBuffer().Get(), 0);
    
    // Set the vertex buffer
    UINT stride = sizeof(TEXTURE_VERTEX);
    UINT offset = 0;
    _d3dContext->IASetVertexBuffers(0, 1, _rendererHelper->getSbVertexBuffer().GetAddressOf(), &stride, &offset);
}

void DirectXFramebufferToScreenProgram::unbind()
{
    _rendererHelper->bindTexture(NGTextureSlot_ZERO, NULL);
}
