//
//  DirectXTextureLoader.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXTextureLoader.h"

#include "framework/util/macros.h"
#include "framework/graphics/directx/DirectXManager.h"
#include "framework/graphics/portable/GpuTextureDataWrapper.h"
#include "framework/graphics/portable/GpuTextureWrapper.h"
#include "framework/file/portable/AssetDataHandler.h"
#include "framework/file/portable/FileData.h"
#include "framework/util/StringUtil.h"

#include "DDSTextureLoader.h"
#include "PlatformHelpers.h"

#include <string>

DirectXTextureLoader::DirectXTextureLoader() : TextureLoader()
{
    // Empty
}

DirectXTextureLoader::~DirectXTextureLoader()
{
    // Empty
}

GpuTextureDataWrapper* DirectXTextureLoader::loadTextureData(const char* textureName)
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
    
    const FileData dds_file = AssetDataHandler::getAssetDataHandler()->getAssetData(finalPath);
    void* output = malloc(dds_file.data_length);
    StringUtil::encryptDecrypt((unsigned char*)dds_file.data, (unsigned char*) output, dds_file.data_length);
    
    ID3D11ShaderResourceView *pShaderResourceView;
    
	ID3D11Device* d3dDevice = DirectXManager::getD3dDevice();
    DirectX::ThrowIfFailed(DirectX::CreateDDSTextureFromMemory(d3dDevice, (const uint8_t*)output, dds_file.data_length, nullptr, &pShaderResourceView));
    
    AssetDataHandler::getAssetDataHandler()->releaseAssetData(&dds_file);
    
    GpuTextureDataWrapper* tdw = new GpuTextureDataWrapper(pShaderResourceView);
    
    delete textureFileName;
    
    free((void *)output);
    
    return tdw;
}

GpuTextureWrapper* DirectXTextureLoader::loadTexture(GpuTextureDataWrapper* textureData, bool repeatS)
{
    UNUSED(repeatS);
    
    return new GpuTextureWrapper(textureData->texture);
}
