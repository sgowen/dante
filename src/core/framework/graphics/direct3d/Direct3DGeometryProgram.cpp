//
//  Direct3DGeometryProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Direct3DGeometryProgram.h"

#include "Direct3DManager.h"

Direct3DGeometryProgram::Direct3DGeometryProgram(const char* vertexShaderName, const char* pixelShaderName) : Direct3DProgram(vertexShaderName, pixelShaderName, false)
{
    // Empty
}

void Direct3DGeometryProgram::mapVertices()
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    
    ID3D11DeviceContext* d3dContext = Direct3DManager::getD3dContext();
    
    //	Disable GPU access to the vertex buffer data.
    d3dContext->Map(D3DManager->getGbVertexBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    //	Update the vertex buffer here.
    int numVertices = D3DManager->getColorVertices().size();
    memcpy(mappedResource.pData, &D3DManager->getColorVertices().front(), sizeof(COLOR_VERTEX) * numVertices);
    
    //	Reenable GPU access to the vertex buffer data.
    d3dContext->Unmap(D3DManager->getGbVertexBuffer().Get(), 0);
    
    // Set the vertex buffer
    UINT stride = sizeof(COLOR_VERTEX);
    UINT offset = 0;
    d3dContext->IASetVertexBuffers(0, 1, D3DManager->getGbVertexBuffer().GetAddressOf(), &stride, &offset);
}
