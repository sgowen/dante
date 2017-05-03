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
#include "ReadData.h"
#include "StringUtil.h"

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
    textureFileName[len + 1] = 'n';
    textureFileName[len + 2] = 'g';
    textureFileName[len + 3] = 't';
    textureFileName[len + 4] = '\0';

	const char* finalPath;
#if (_WIN32_WINNT == _WIN32_WINNT_WIN7)
	std::string s("data\\textures\\");
	s += std::string(textureFileName);
	finalPath = s.c_str();
#else
	finalPath = textureFileName;
#endif
    
    wchar_t* wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, finalPath, -1, wString, 4096);
    
    auto blob = DX::ReadData(wString);
    unsigned char* output = (unsigned char*) malloc(blob.size());
    StringUtil::encryptDecrypt((unsigned char*)blob.data(), output, blob.size());
    
    ID3D11ShaderResourceView *pShaderResourceView;
    
	ID3D11Device* d3dDevice = Direct3DManager::getD3dDevice();
    DirectX::ThrowIfFailed(DirectX::CreateDDSTextureFromMemory(d3dDevice, (const uint8_t*)output, blob.size(), nullptr, &pShaderResourceView));
    
    GpuTextureDataWrapper* tdw = new GpuTextureDataWrapper(pShaderResourceView);
    
    delete wString;
    delete textureFileName;
    
    free((void *)output);
    
    return tdw;
}

GpuTextureWrapper* Direct3DTextureLoader::loadTexture(GpuTextureDataWrapper* textureData, bool repeatS)
{
    UNUSED(repeatS);
    
    return new GpuTextureWrapper(textureData->texture);
}
