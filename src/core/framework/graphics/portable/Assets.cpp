//
//  Assets.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Assets.h"

#include "IAssetsMapper.h"
#include "TextureRegion.h"
#include "Animation.h"
#include "NGSTDUtil.h"

#include <assert.h>

Assets* Assets::getInstance()
{
    static Assets instance = Assets();
    return &instance;
}

void Assets::init(IAssetsMapper* assetsMapper)
{
	getTextureRegionMap().clear();
	getAnimationsMap().clear();

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
        
        Animation* anim = q2->second;
        
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

Animation& Assets::findAnimation(std::string key)
{
    auto q = _animations.find(key);
    
    assert(q != _animations.end());
    
    Animation* anim = q->second;
    
    return *anim;
}

std::map<std::string, TextureRegion*>& Assets::getTextureRegionMap()
{
    return _textureRegions;
}

std::map<std::string, Animation*>& Assets::getAnimationsMap()
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
