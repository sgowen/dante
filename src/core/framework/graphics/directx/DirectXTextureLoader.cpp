//
//  DirectXTextureLoader.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXTextureLoader.h"

#include "framework/graphics/portable/GpuTextureDataWrapper.h"
#include <framework/graphics/portable/TextureWrapper.h>
#include "framework/graphics/portable/GpuTextureWrapper.h"

#include "framework/util/macros.h"
#include "framework/graphics/directx/DirectXRendererHelper.h"
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

GpuTextureDataWrapper* DirectXTextureLoader::loadTextureData(TextureWrapper* textureWrapper)
{
    const char* textureName = textureWrapper->name.c_str();

	std::string s("data\\textures\\");
	s += std::string(textureName);
	const char* finalPath = s.c_str();
    
    const FileData fileData = AssetDataHandler::getAssetDataHandler()->getAssetData(finalPath);
    void* output = NULL;
    if (textureWrapper->_isEncrypted)
    {
        output = malloc(fileData.data_length);
        StringUtil::encryptDecrypt((unsigned char*)fileData.data, (unsigned char*) output, fileData.data_length);
    }
    else
    {
        output = (void*) fileData.data;
    }
    
    ID3D11ShaderResourceView *pShaderResourceView;
    
	ID3D11Device* d3dDevice = DirectXRendererHelper::getD3dDevice();
    DirectX::ThrowIfFailed(DirectX::CreateDDSTextureFromMemory(d3dDevice, (const uint8_t*)output, fileData.data_length, NULL, &pShaderResourceView));
    
    AssetDataHandler::getAssetDataHandler()->releaseAssetData(&fileData);
    
    GpuTextureDataWrapper* tdw = new GpuTextureDataWrapper(pShaderResourceView);
    
    if (textureWrapper->_isEncrypted)
    {
        free((void *)output);
    }
    
    return tdw;
}

GpuTextureWrapper* DirectXTextureLoader::loadTexture(GpuTextureDataWrapper* textureData, bool repeatS)
{
    UNUSED(repeatS);
    
    return new GpuTextureWrapper(textureData->texture);
}
