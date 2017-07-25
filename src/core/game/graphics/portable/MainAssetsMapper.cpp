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
    ASSETS->getTextureRegionMap()["Background1"] = new TextureRegion("texture_003", 0, 384, 1024, 640, TEXTURE_SIZE_1024);
    ASSETS->getTextureRegionMap()["Background2"] = new TextureRegion("texture_004", 0, 644, 1024, 248, TEXTURE_SIZE_1024);
    ASSETS->getTextureRegionMap()["Background3"] = new TextureRegion("texture_004", 0, 896, 1024, 128, TEXTURE_SIZE_1024);
    ASSETS->getTextureRegionMap()["Cover"] = new TextureRegion("texture_005", 0, 0, 1024, 1024, TEXTURE_SIZE_1024);
    
    ASSETS->getTextureRegionMap()["Projectile"] = new TextureRegion("texture_001", 0, 138, 16, 16, TEXTURE_SIZE_2048);
    
    ASSETS->getTextureRegionMap()["Crate"] = new TextureRegion("texture_001", 624, 0, 128, 128, TEXTURE_SIZE_2048);
    
    ASSETS->getTextureRegionMap()["Space_Pirate_Chunk_Top_Left"] = new TextureRegion("texture_001", 394, 0, 24, 28, TEXTURE_SIZE_2048);
    ASSETS->getTextureRegionMap()["Space_Pirate_Chunk_Top_Right"] = new TextureRegion("texture_001", 416, 0, 24, 28, TEXTURE_SIZE_2048);
    ASSETS->getTextureRegionMap()["Space_Pirate_Chunk_Bottom_Left"] = new TextureRegion("texture_001", 394, 26, 24, 28, TEXTURE_SIZE_2048);
    ASSETS->getTextureRegionMap()["Space_Pirate_Chunk_Bottom_Right"] = new TextureRegion("texture_001", 416, 26, 24, 28, TEXTURE_SIZE_2048);
    
    /// Animations
    ASSETS->getAnimationsMap()["Samus_Running"] = new Animation("texture_001", 0, 0, 36, 46, 360, 46, TEXTURE_SIZE_2048, true, 0.1f, 10, 0, 2);
    ASSETS->getAnimationsMap()["Samus_Running_Fast"] = new Animation("texture_001", 0, 0, 36, 46, 360, 46, TEXTURE_SIZE_2048, true, 0.075f, 10, 0, 2);
    
    ASSETS->getAnimationsMap()["Samus_Idle"] = new Animation("texture_001", 118, 58, 36, 46, 144 + 16 * 3, 138 + 8 * 2, TEXTURE_SIZE_2048, true, 0.1f, 3, 0, 16, 8);
    
    ASSETS->getAnimationsMap()["Samus_Shooting"] = new Animation("texture_001", 630, 324, 36, 46, 144 + 16 * 3, 138 + 8 * 2, TEXTURE_SIZE_2048, true, 0.1f, 10, 0, 16, 8);
    
    ASSETS->getAnimationsMap()["Samus_Jumping"] = new Animation("texture_001", 630, 162, 36, 46, 144 + 16 * 3, 138 + 8 * 2, TEXTURE_SIZE_2048, false, 0.1f, 4, 0, 16, 8);
    
    ASSETS->getAnimationsMap()["Samus_Falling"] = new Animation("texture_001", 630, 216, 36, 46, 144 + 16 * 3, 138 + 8 * 2, TEXTURE_SIZE_2048, false, 0.1f, 4, 0, 16, 8);
    
    ASSETS->getAnimationsMap()["Samus_Landing"] = new Animation("texture_001", 630, 270, 36, 46, 144 + 16 * 3, 138 + 8 * 2, TEXTURE_SIZE_2048, false, 0.1f, 2, 0, 16, 8);
    
    ASSETS->getAnimationsMap()["Explosion"] = new Animation("texture_001", 20, 138, 16, 16, 16 * 5 + 4 * 4, 16, TEXTURE_SIZE_2048, false, 0.1f, 5, 0, 4);
    
    ASSETS->getAnimationsMap()["Space_Pirate_Walking"] = new Animation("texture_001", 394, 0, 46, 54, 46 * 4 + 4 * 3, 54 * 2 + 4, TEXTURE_SIZE_2048, true, 0.1f, 8, 0, 4, 4);
}
