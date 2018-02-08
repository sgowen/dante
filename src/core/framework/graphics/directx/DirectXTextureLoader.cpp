//
//  DirectXTextureLoader.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/directx/DirectXTextureLoader.h"

#include "framework/graphics/portable/TextureDataWrapper.h"
#include <framework/graphics/portable/NGTexture.h>
#include "framework/graphics/portable/TextureWrapper.h"
#include "framework/graphics/portable/NGTextureDesc.h"

#include "framework/file/portable/AssetDataHandler.h"
#include "framework/file/portable/FileData.h"
#include "framework/util/StringUtil.h"
#include "framework/util/macros.h"
#include "framework/graphics/directx/DirectXRendererHelper.h"

#include "DDSTextureLoader.h"

#include <string>

DirectXTextureLoader::DirectXTextureLoader() : TextureLoader()
{
    // Empty
}

TextureDataWrapper* DirectXTextureLoader::loadTextureData(NGTexture* texture)
{
    const FileData fileData = AssetDataHandler::getAssetDataHandler()->getAssetData(texture->filePath.c_str());
    void* output = NULL;
    if (texture->_desc->_isEncrypted)
    {
        output = malloc(fileData.data_length);
        StringUtil::encryptDecrypt((unsigned char*)fileData.data, (unsigned char*) output, fileData.data_length);
    }
    else
    {
        output = (void*) fileData.data;
    }
    
    ID3D11ShaderResourceView *pShaderResourceView;
    
	ID3D11Device* d3dDevice = DirectXRendererHelper::getD3DDevice();
	DX::ThrowIfFailed(DirectX::CreateDDSTextureFromMemory(d3dDevice, (const uint8_t*)output, fileData.data_length, NULL, &pShaderResourceView));
    
    AssetDataHandler::getAssetDataHandler()->releaseAssetData(&fileData);
    
    TextureDataWrapper* tdw = new TextureDataWrapper(pShaderResourceView);
    
    if (texture->_desc->_isEncrypted)
    {
        free((void *)output);
    }
    
    return tdw;
}

TextureWrapper* DirectXTextureLoader::loadTexture(TextureDataWrapper* textureData, NGTextureDesc* textureDesc)
{
    UNUSED(textureDesc);
    
    return new TextureWrapper(textureData->texture);
}

void DirectXTextureLoader::destroyTexture(TextureWrapper& textureWrapper)
{
    textureWrapper.texture->Release();
}
