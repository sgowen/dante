//
//  Direct3DGeometryProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Direct3DGeometryProgram.h"

#include "DeviceResources.h"
#include "Direct3DManager.h"

Direct3DGeometryProgram::Direct3DGeometryProgram(_In_z_ const wchar_t* vertexShaderName, _In_z_ const wchar_t* pixelShaderName) : Direct3DProgram(vertexShaderName, pixelShaderName, false)
{
    // Empty
}

void Direct3DGeometryProgram::mapVertices()
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    
    DX::DeviceResources* deviceResources = Direct3DManager::getDeviceResources();
    
    //	Disable GPU access to the vertex buffer data.
    deviceResources->GetD3DDeviceContext()->Map(D3DManager->getGbVertexBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    //	Update the vertex buffer here.
    int numVertices = D3DManager->getColorVertices().size();
    memcpy(mappedResource.pData, &D3DManager->getColorVertices().front(), sizeof(COLOR_VERTEX) * numVertices);
    
    //	Reenable GPU access to the vertex buffer data.
    deviceResources->GetD3DDeviceContext()->Unmap(D3DManager->getGbVertexBuffer().Get(), 0);
    
    // Set the vertex buffer
    UINT stride = sizeof(COLOR_VERTEX);
    UINT offset = 0;
    deviceResources->GetD3DDeviceContext()->IASetVertexBuffers(0, 1, D3DManager->getGbVertexBuffer().GetAddressOf(), &stride, &offset);
}
