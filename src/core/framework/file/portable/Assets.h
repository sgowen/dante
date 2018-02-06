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
#include <vector>

struct SoundDesc;
struct NGTextureDesc;
class TextureRegion;
class NGAnimation;

#define ASSETS (Assets::getInstance())

class Assets
{
public:
	static Assets* getInstance();
    
    static void sDeserializerFunc(const char* data);
    
    void initWithJsonFile(const char* fileName, bool isBundled = true, bool useEncryption = false);
    void initWithJson(const char* json);
    TextureRegion& findTextureRegion(std::string key, uint16_t stateTime);
    TextureRegion& findTextureRegion(std::string key);
    NGAnimation& findAnimation(std::string key);
    std::vector<SoundDesc*>& getSoundDescriptors();
    std::vector<NGTextureDesc*>& getTextureDescriptors();
    std::map<std::string, TextureRegion*>& getTextureRegionMap();
    std::map<std::string, NGAnimation*>& getAnimationsMap();

private:
    std::vector<SoundDesc*> _sounds;
    std::vector<NGTextureDesc*> _textures;
    std::map<std::string, TextureRegion*> _textureRegions;
    std::map<std::string, NGAnimation*> _animations;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Assets();
    ~Assets();
    Assets(const Assets&);
    Assets& operator=(const Assets&);
};

#endif /* defined(__noctisgames__Assets__) */
