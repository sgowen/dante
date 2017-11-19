//
//  DirectXGeometryProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXGeometryProgram.h"

#include "framework/graphics/directx/DirectXManager.h"

DirectXGeometryProgram::DirectXGeometryProgram(const char* vertexShaderName, const char* pixelShaderName) : DirectXProgram(vertexShaderName, pixelShaderName, false)
{
    // Empty
}

void DirectXGeometryProgram::mapVertices()
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    
    ID3D11DeviceContext* d3dContext = DirectXManager::getD3dContext();
    
    //	Disable GPU access to the vertex buffer data.
    d3dContext->Map(DXManager->getGbVertexBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    //	Update the vertex buffer here.
    int numVertices = DXManager->getColorVertices().size();
    memcpy(mappedResource.pData, &DXManager->getColorVertices().front(), sizeof(COLOR_VERTEX) * numVertices);
    
    //	Reenable GPU access to the vertex buffer data.
    d3dContext->Unmap(DXManager->getGbVertexBuffer().Get(), 0);
    
    // Set the vertex buffer
    UINT stride = sizeof(COLOR_VERTEX);
    UINT offset = 0;
    d3dContext->IASetVertexBuffers(0, 1, DXManager->getGbVertexBuffer().GetAddressOf(), &stride, &offset);
}
