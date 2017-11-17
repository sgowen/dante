//
//  Assets.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Assets__
#define __noctisgames__Assets__

#include <map>
#include <string>

class AssetsMapper;
class TextureRegion;
class Animation;

#define ASSETS (Assets::getInstance())

class Assets
{
public:
	static Assets* getInstance();
    
    void init(AssetsMapper* assetsMapper);
    
    TextureRegion& findTextureRegion(std::string key, float stateTime);
    
    TextureRegion& findTextureRegion(std::string key);
    
    Animation& findAnimation(std::string key);
    
    std::map<std::string, TextureRegion*>& getTextureRegionMap();
    
    std::map<std::string, Animation*>& getAnimationsMap();

private:
    std::map<std::string, TextureRegion*> _textureRegions;
    std::map<std::string, Animation*> _animations;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Assets();
    ~Assets();
    Assets(const Assets&);
    Assets& operator=(const Assets&);
};

#endif /* defined(__noctisgames__Assets__) */
