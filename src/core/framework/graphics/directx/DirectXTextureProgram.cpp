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

DirectXTextureProgram::DirectXTextureProgram(DirectXRendererHelper* inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName) : DirectXProgram(inRendererHelper, vertexShaderName, fragmentShaderName, true)
{
    // Empty
}

void DirectXTextureProgram::bind()
{
    DirectXProgram::bind();
    
    _rendererHelper->useNormalBlending();
    
    bindMatrix();
    
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    
    ID3D11DeviceContext* d3dContext = DirectXRendererHelper::getD3dContext();
    
    //	Disable GPU access to the vertex buffer data.
    d3dContext->Map(_rendererHelper->getSbVertexBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    //	Update the vertex buffer here.
    int numVertices = _rendererHelper->getTextureVertices().size();
    memcpy(mappedResource.pData, &_rendererHelper->getTextureVertices().front(), sizeof(TEXTURE_VERTEX) * numVertices);
    
    //	Reenable GPU access to the vertex buffer data.
    d3dContext->Unmap(_rendererHelper->getSbVertexBuffer().Get(), 0);
    
    // Set the vertex buffer
    UINT stride = sizeof(TEXTURE_VERTEX);
    UINT offset = 0;
    d3dContext->IASetVertexBuffers(0, 1, _rendererHelper->getSbVertexBuffer().GetAddressOf(), &stride, &offset);
}
