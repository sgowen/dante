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
    ASSETS->getTextureRegionMap()["Pellet"] = new TextureRegion("texture_001", 0, 512, 16, 16, TEXTURE_SIZE_2048);
    ASSETS->getTextureRegionMap()["TopSecretFolder"] = new TextureRegion("texture_001", 48, 512, 128, 128, TEXTURE_SIZE_2048);
    
    /// Animations
    ASSETS->getAnimationsMap()["Samus_Running"] = new Animation("texture_001", 0, 0, 36, 46, 720, 46, TEXTURE_SIZE_2048, true, 0.1f, 20);
    ASSETS->getAnimationsMap()["CharacterHoldingGun"] = new Animation("texture_001", 0, 1664, 198, 152, 1980, 304, TEXTURE_SIZE_2048, true, 0.1f, 11);
}
