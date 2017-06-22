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
    auto q = m_textureRegions.find(key);
    
    if (q != m_textureRegions.end())
    {
        TextureRegion* tr = q->second;
        
        return *tr;
    }
    else
    {
        auto q2 = m_animations.find(key);
        
        assert(q2 != m_animations.end());
        
        Animation* anim = q2->second;
        
        return anim->getTextureRegion(stateTime);
    }
}

TextureRegion& Assets::findTextureRegion(std::string key)
{
    auto q = m_textureRegions.find(key);
    
    assert(q != m_textureRegions.end());
    
    TextureRegion* tr = q->second;
    
    return *tr;
}

Animation& Assets::findAnimation(std::string key)
{
    auto q = m_animations.find(key);
    
    assert(q != m_animations.end());
    
    Animation* anim = q->second;
    
    return *anim;
}

std::unordered_map<std::string, TextureRegion*>& Assets::getTextureRegionMap()
{
    return m_textureRegions;
}

std::unordered_map<std::string, Animation*>& Assets::getAnimationsMap()
{
    return m_animations;
}

Assets::Assets()
{
    // Empty
}

Assets::~Assets()
{
    NGSTDUtil::cleanUpMapOfPointers(m_textureRegions);
    NGSTDUtil::cleanUpMapOfPointers(m_animations);
}
