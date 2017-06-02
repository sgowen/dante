//
//  MainAssetsMapper.cpp
//  dante
//
//  Created by Stephen Gowen on 11/9/16.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "MainAssetsMapper.h"

#include "Assets.h"
#include "TextureRegion.h"
#include "Animation.h"

MainAssetsMapper::MainAssetsMapper() : IAssetsMapper()
{
    // Empty
}

MainAssetsMapper::~MainAssetsMapper()
{
    // Empty
}

void MainAssetsMapper::mapAssets()
{
    /// Texture Regions
    ASSETS->getTextureRegionMap()["Mario_Backdrop"] = new TextureRegion("texture_001", 1024, 0, 512, 444, TEXTURE_SIZE_2048);
    
    /// Animations
    ASSETS->getAnimationsMap()["Samus_Running"] = new Animation("texture_001", 630, 0, 36, 46, 144 + 16 * 3, 138 + 8 * 2, TEXTURE_SIZE_2048, true, 0.1f, 10, 0, 16, 8);
    
    ASSETS->getAnimationsMap()["Samus_Shooting"] = new Animation("texture_001", 630, 324, 36, 46, 144 + 16 * 3, 138 + 8 * 2, TEXTURE_SIZE_2048, true, 0.1f, 10, 0, 16, 8);
    
    ASSETS->getAnimationsMap()["Samus_Jumping"] = new Animation("texture_001", 630, 162, 36, 46, 144 + 16 * 3, 138 + 8 * 2, TEXTURE_SIZE_2048, false, 0.1f, 4, 0, 16, 8);
    
    ASSETS->getAnimationsMap()["Samus_Falling"] = new Animation("texture_001", 630, 216, 36, 46, 144 + 16 * 3, 138 + 8 * 2, TEXTURE_SIZE_2048, false, 0.1f, 4, 0, 16, 8);
    
    ASSETS->getAnimationsMap()["Samus_Landing"] = new Animation("texture_001", 630, 270, 36, 46, 144 + 16 * 3, 138 + 8 * 2, TEXTURE_SIZE_2048, false, 0.1f, 2, 0, 16, 8);
}
