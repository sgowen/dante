//
//  AppleAssetDataHandler.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 2/25/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include <framework/file/apple/AppleAssetDataHandler.h>

#include <framework/file/apple/apple_asset_data_handler.h>

AppleAssetDataHandler* AppleAssetDataHandler::getInstance()
{
    static AppleAssetDataHandler instance = AppleAssetDataHandler();
    return &instance;
}

FileData AppleAssetDataHandler::getAssetData(const char* relativePath)
{
    return appleGetAssetData(relativePath);
}

void AppleAssetDataHandler::releaseAssetData(const FileData* fileData)
{
    appleReleaseAssetData(fileData);
}

AppleAssetDataHandler::AppleAssetDataHandler() : AssetDataHandler()
{
    // Empty
}
