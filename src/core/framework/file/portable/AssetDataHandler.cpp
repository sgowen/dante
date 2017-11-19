//
//  AssetDataHandler.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/2/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/file/portable/AssetDataHandler.h"

#if defined __APPLE__
#include "framework/file/apple/AppleAssetDataHandler.h"
#elif defined __ANDROID__
#include "framework/file/android/AndroidAssetDataHandler.h"
#elif defined _WIN32
#include "framework/file/windows/WindowsAssetDataHandler.h"
#endif

#include <assert.h>

AssetDataHandler* AssetDataHandler::getAssetDataHandler()
{
#if defined __APPLE__
    return AppleAssetDataHandler::getInstance();
#elif defined __ANDROID__
    return AndroidAssetDataHandler::getInstance();
#elif defined _WIN32
    return WindowsAssetDataHandler::getInstance();
#endif
    
    assert(false);
}

AssetDataHandler::AssetDataHandler()
{
    // Empty
}
