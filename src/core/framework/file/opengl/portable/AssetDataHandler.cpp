//
//  AssetDataHandler.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/2/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include "AssetDataHandler.h"

#if defined __APPLE__
#include "AppleAssetDataHandler.h"
#elif defined __ANDROID__
#include "AndroidAssetDataHandler.h"
#endif

#include <assert.h>

AssetDataHandler* AssetDataHandler::getAssetDataHandler()
{
#if defined __APPLE__
    return AppleAssetDataHandler::getInstance();
#elif defined __ANDROID__
    return AndroidAssetDataHandler::getInstance();
#endif
    
    assert(false);
}

AssetDataHandler::AssetDataHandler()
{
    // Empty
}
