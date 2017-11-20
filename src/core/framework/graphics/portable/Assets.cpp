//
//  Assets.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/Assets.h"

#include "framework/graphics/portable/AssetsMapper.h"
#include "framework/graphics/portable/TextureRegion.h"
#include "framework/graphics/portable/NGAnimation.h"
#include "framework/util/NGSTDUtil.h"

#include <assert.h>

Assets* Assets::getInstance()
{
    static Assets instance = Assets();
    return &instance;
}

void Assets::init(AssetsMapper* assetsMapper)
{
	getTextureRegionMap().clear();
	getNGAnimationsMap().clear();

    assetsMapper->mapAssets();
    
    delete assetsMapper;
}

TextureRegion& Assets::findTextureRegion(std::string key, float stateTime)
{
    auto q = _textureRegions.find(key);
    
    if (q != _textureRegions.end())
    {
        TextureRegion* tr = q->second;
        
        return *tr;
    }
    else
    {
        auto q2 = _animations.find(key);
        
        assert(q2 != _animations.end());
        
        NGAnimation* anim = q2->second;
        
        return anim->getTextureRegion(stateTime);
    }
}

TextureRegion& Assets::findTextureRegion(std::string key)
{
    auto q = _textureRegions.find(key);
    
    assert(q != _textureRegions.end());
    
    TextureRegion* tr = q->second;
    
    return *tr;
}

NGAnimation& Assets::findNGAnimation(std::string key)
{
    auto q = _animations.find(key);
    
    assert(q != _animations.end());
    
    NGAnimation* anim = q->second;
    
    return *anim;
}

std::map<std::string, TextureRegion*>& Assets::getTextureRegionMap()
{
    return _textureRegions;
}

std::map<std::string, NGAnimation*>& Assets::getNGAnimationsMap()
{
    return _animations;
}

Assets::Assets()
{
    // Empty
}

Assets::~Assets()
{
    NGSTDUtil::cleanUpMapOfPointers(_textureRegions);
    NGSTDUtil::cleanUpMapOfPointers(_animations);
}
