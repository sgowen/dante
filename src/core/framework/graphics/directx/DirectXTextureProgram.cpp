//
//  DirectXTextureProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXTextureProgram.h"

#include "framework/graphics/directx/DirectXManager.h"

DirectXTextureProgram::DirectXTextureProgram(const char* vertexShaderName, const char* fragmentShaderName) : DirectXProgram(vertexShaderName, fragmentShaderName, true)
{
    // Empty
}

void DirectXTextureProgram::mapVertices()
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    
    ID3D11DeviceContext* d3dContext = DirectXManager::getD3dContext();
    
    //	Disable GPU access to the vertex buffer data.
    d3dContext->Map(DXManager->getSbVertexBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    //	Update the vertex buffer here.
    int numVertices = DXManager->getTextureVertices().size();
    memcpy(mappedResource.pData, &DXManager->getTextureVertices().front(), sizeof(TEXTURE_VERTEX) * numVertices);
    
    //	Reenable GPU access to the vertex buffer data.
    d3dContext->Unmap(DXManager->getSbVertexBuffer().Get(), 0);
    
    // Set the vertex buffer
    UINT stride = sizeof(TEXTURE_VERTEX);
    UINT offset = 0;
    d3dContext->IASetVertexBuffers(0, 1, DXManager->getSbVertexBuffer().GetAddressOf(), &stride, &offset);
}
