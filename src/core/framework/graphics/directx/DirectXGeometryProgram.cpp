//
//  DirectXGeometryProgram.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXGeometryProgram.h"

#include "framework/graphics/directx/DirectXRendererHelper.h"

const D3D11_INPUT_ELEMENT_DESC DirectXGeometryProgram::VERTEX_DESC[2] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

DirectXGeometryProgram::DirectXGeometryProgram(DirectXRendererHelper* inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName) : DirectXProgram(inRendererHelper, vertexShaderName, fragmentShaderName, VERTEX_DESC, ARRAYSIZE(VERTEX_DESC))
{
    // Empty
}

void DirectXGeometryProgram::bind(void* data)
{
    DirectXProgram::bind(data);
    
    useNormalBlending();
    
    _d3dContext->VSSetConstantBuffers(0, 1, _rendererHelper->getMatrixConstantbuffer().GetAddressOf());
    
    // send the final matrix to video memory
    _d3dContext->UpdateSubresource(_rendererHelper->getMatrixConstantbuffer().Get(), 0, 0, &_rendererHelper->getMatFinal(), 0, 0);
    
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    
    //	Disable GPU access to the vertex buffer data.
    _d3dContext->Map(_rendererHelper->getGbVertexBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    
    //	Update the vertex buffer here.
    std::vector<COLOR_VERTEX>& vertices = _rendererHelper->getColorVertices();
    memcpy(mappedResource.pData, &vertices.front(), sizeof(COLOR_VERTEX) * vertices.size());
    
    //	Reenable GPU access to the vertex buffer data.
    _d3dContext->Unmap(_rendererHelper->getGbVertexBuffer().Get(), 0);
    
    // Set the vertex buffer
    UINT stride = sizeof(COLOR_VERTEX);
    UINT offset = 0;
    _d3dContext->IASetVertexBuffers(0, 1, _rendererHelper->getGbVertexBuffer().GetAddressOf(), &stride, &offset);
}
