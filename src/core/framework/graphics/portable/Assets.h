//
//  Assets.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Assets__
#define __noctisgames__Assets__

#include <unordered_map>
#include <string>

class IAssetsMapper;
class TextureRegion;
class Animation;

#define ASSETS (Assets::getInstance())

class Assets
{
public:
	static Assets* getInstance();
    
    void init(IAssetsMapper* assetsMapper);
    
    TextureRegion& findTextureRegion(std::string key, float stateTime);
    
    TextureRegion& findTextureRegion(std::string key);
    
    Animation& findAnimation(std::string key);
    
    std::unordered_map<std::string, TextureRegion*>& getTextureRegionMap();
    
    std::unordered_map<std::string, Animation*>& getAnimationsMap();

private:
    std::unordered_map<std::string, TextureRegion*> m_textureRegions;
    std::unordered_map<std::string, Animation*> m_animations;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Assets();
    ~Assets();
    Assets(const Assets&);
    Assets& operator=(const Assets&);
};

#endif /* defined(__noctisgames__Assets__) */
