//
//  Direct3DTextureLoader.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Direct3DTextureLoader.h"

#include "DDSTextureLoader.h"
#include "macros.h"
#include "Direct3DManager.h"
#include "GpuTextureDataWrapper.h"
#include "GpuTextureWrapper.h"
#include "PlatformHelpers.h"

#include <string>

Direct3DTextureLoader::Direct3DTextureLoader() : ITextureLoader()
{
    // Empty
}

Direct3DTextureLoader::~Direct3DTextureLoader()
{
    // Empty
}

GpuTextureDataWrapper* Direct3DTextureLoader::loadTextureData(const char* textureName)
{
    size_t len = strlen(textureName);
    
    char* textureFileName = new char[len + 5];
    
	strcpy_s(textureFileName, len + 5, textureName);
    textureFileName[len] = '.';
    textureFileName[len + 1] = 'd';
    textureFileName[len + 2] = 'd';
    textureFileName[len + 3] = 's';
    textureFileName[len + 4] = '\0';
    
    wchar_t* wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, textureFileName, -1, wString, 4096);
    
    ID3D11ShaderResourceView *pShaderResourceView;
    
	ID3D11Device* d3dDevice = Direct3DManager::getD3dDevice();
    DirectX::ThrowIfFailed(DirectX::CreateDDSTextureFromFile(d3dDevice, wString, nullptr, &pShaderResourceView));
    
    GpuTextureDataWrapper* tdw = new GpuTextureDataWrapper(pShaderResourceView);
    
    delete wString;
    delete textureFileName;
    
    return tdw;
}

GpuTextureWrapper* Direct3DTextureLoader::loadTexture(GpuTextureDataWrapper* textureData, bool repeatS)
{
    UNUSED(repeatS);
    
    return new GpuTextureWrapper(textureData->texture);
}
