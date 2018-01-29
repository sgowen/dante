//
//  Assets.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/file/portable/Assets.h"

#include "framework/audio/portable/SoundDesc.h"
#include "framework/graphics/portable/NGTextureDesc.h"
#include "framework/graphics/portable/TextureRegion.h"
#include "framework/graphics/portable/NGAnimation.h"

#include "framework/util/NGSTDUtil.h"
#include <framework/file/portable/JsonFile.h>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

#include <assert.h>

Assets* Assets::getInstance()
{
    static Assets instance = Assets();
    return &instance;
}

void Assets::sDeserializerFunc(const char* data)
{
    ASSETS->initWithJson(data);
}

void Assets::initWithJsonFile(const char* path, bool isBundled, bool useEncryption)
{
    const char* finalPath;
#if defined __linux__ && !defined(__ANDROID__)
    std::string s("assets/config/");
    s += std::string(path);
    finalPath = s.c_str();
#elif defined _WIN32
    std::string s("assets\\config\\");
    s += std::string(path);
    finalPath = s.c_str();
#else
    finalPath = path;
#endif
    
    JsonFile jsonFile(finalPath, isBundled, useEncryption);
    jsonFile.setDeserializerFunc(sDeserializerFunc);
    jsonFile.load();
}

void Assets::initWithJson(const char* json)
{
    NGSTDUtil::cleanUpVectorOfPointers(_sounds);
    NGSTDUtil::cleanUpVectorOfPointers(_textures);
    NGSTDUtil::cleanUpMapOfPointers(_textureRegions);
    NGSTDUtil::cleanUpMapOfPointers(_animations);
    
    using namespace rapidjson;
    
    Document d;
    d.Parse<kParseStopWhenDoneFlag>(json);
    
    if (d.HasMember("music"))
    {
        Value& v = d["music"];
        
        assert(v.IsString());
        std::string musicName = v.GetString();
        
        _sounds.push_back(new SoundDesc(1337, musicName, 1));
    }
    
    if (d.HasMember("sounds"))
    {
        Value& v = d["sounds"];
        
        assert(v.IsArray());
        for (SizeType i = 0; i < v.Size(); ++i)
        {
            const Value& iv = v[i];
            
            int soundId = iv["soundId"].GetInt();
            std::string soundName = iv["soundName"].GetString();
            int numInstances = iv["numInstances"].GetInt();
            
            _sounds.push_back(new SoundDesc(soundId, soundName, numInstances));
        }
    }
    
    if (d.HasMember("textures"))
    {
        Value& v = d["textures"];
        
        assert(v.IsArray());
        for (SizeType i = 0; i < v.Size(); ++i)
        {
            const Value& iv = v[i];
            std::string textureName = iv["textureName"].GetString();
            bool repeatS = iv.HasMember("repeatS") ? iv["repeatS"].GetBool() : false;
            bool isEncrypted = iv.HasMember("isEncrypted") ? iv["isEncrypted"].GetBool() : true;
            bool hasNormal = iv.HasMember("hasNormal") ? iv["hasNormal"].GetBool() : true;
            
            _textures.push_back(new NGTextureDesc(textureName, repeatS, isEncrypted, hasNormal));
            
            if (iv.HasMember("mappings"))
            {
                const Value& v = iv["mappings"];
                
                assert(v.IsObject());
                for (Value::ConstMemberIterator i = v.MemberBegin(); i != v.MemberEnd(); ++i)
                {
                    const Value& iv = i->value;
                    assert(iv.IsObject());
                    
                    std::string key = i->name.GetString();
                    int x = iv["x"].GetInt();
                    int y = iv["y"].GetInt();
                    int regionWidth = iv["regionWidth"].GetInt();
                    int regionHeight = iv["regionHeight"].GetInt();
                    int textureWidth = 2048;
                    int textureHeight = 2048;
                    if (iv.HasMember("textureWidth") && iv.HasMember("textureHeight"))
                    {
                        textureWidth = iv["textureWidth"].GetInt();
                        textureHeight = iv["textureHeight"].GetInt();
                    }
                    
                    if (iv.HasMember("frameTimes") || iv.HasMember("frameTime"))
                    {
                        bool looping = iv.HasMember("looping") ? iv["looping"].GetBool() : true;
                        int firstLoopingFrame = iv.HasMember("firstLoopingFrame") ? iv["firstLoopingFrame"].GetInt() : 0;
                        int xPadding = iv.HasMember("xPadding") ? iv["xPadding"].GetInt() : 0;
                        int yPadding = iv.HasMember("yPadding") ? iv["yPadding"].GetInt() : 0;
                        
                        assert(!_animations[key]);
                        
                        if (iv.HasMember("frameTimes"))
                        {
                            const Value& va = iv["frameTimes"];
                            assert(va.IsArray());
                            
                            std::vector<uint16_t> frameTimes;
                            for (SizeType i = 0; i < va.Size(); ++i)
                            {
                                const Value& iva = va[i];
                                frameTimes.push_back(iva.GetInt());
                            }
                            
                            int numFrames = static_cast<int>(frameTimes.size());
                            
                            int animationWidth = regionWidth * numFrames;
                            int animationHeight = regionHeight;
                            if (iv.HasMember("animationWidth") && iv.HasMember("animationHeight"))
                            {
                                animationWidth = iv["animationWidth"].GetInt();
                                animationHeight = iv["animationHeight"].GetInt();
                            }
                            
                            _animations[key] = new NGAnimation(textureName, x, y, regionWidth, regionHeight, animationWidth, animationHeight, textureWidth, textureHeight, looping, firstLoopingFrame, xPadding, yPadding, frameTimes);
                        }
                        else
                        {
                            uint16_t frameTime = iv["frameTime"].GetInt();
                            int numFrames = iv["numFrames"].GetInt();
                            
                            int animationWidth = regionWidth * numFrames;
                            int animationHeight = regionHeight;
                            if (iv.HasMember("animationWidth") && iv.HasMember("animationHeight"))
                            {
                                animationWidth = iv["animationWidth"].GetInt();
                                animationHeight = iv["animationHeight"].GetInt();
                            }
                            
                            _animations[key] = new NGAnimation(textureName, x, y, regionWidth, regionHeight, animationWidth, animationHeight, textureWidth, textureHeight, looping, firstLoopingFrame, xPadding, yPadding, frameTime, numFrames);
                        }
                    }
                    else
                    {
                        assert(!_textureRegions[key]);
                        _textureRegions[key] = new TextureRegion(textureName, x, y, regionWidth, regionHeight, textureWidth, textureHeight);
                    }
                }
            }
        }
    }
}

TextureRegion& Assets::findTextureRegion(std::string key, uint16_t stateTime)
{
    auto q = _textureRegions.find(key);
    
    if (q != _textureRegions.end())
    {
        TextureRegion* tr = q->second;
		assert(tr);
        
        return *tr;
    }
    else
    {
        auto q2 = _animations.find(key);
        
        assert(q2 != _animations.end());
        
        NGAnimation* anim = q2->second;
		assert(anim);
        
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

NGAnimation& Assets::findAnimation(std::string key)
{
    auto q = _animations.find(key);
    
    assert(q != _animations.end());
    
    NGAnimation* anim = q->second;
    
    return *anim;
}

std::vector<SoundDesc*>& Assets::getSoundDescriptors()
{
    return _sounds;
}

std::vector<NGTextureDesc*>& Assets::getTextureDescriptors()
{
    return _textures;
}

std::map<std::string, TextureRegion*>& Assets::getTextureRegionMap()
{
    return _textureRegions;
}

std::map<std::string, NGAnimation*>& Assets::getAnimationsMap()
{
    return _animations;
}

Assets::Assets()
{
    // Empty
}

Assets::~Assets()
{
    NGSTDUtil::cleanUpVectorOfPointers(_sounds);
    NGSTDUtil::cleanUpVectorOfPointers(_textures);
    NGSTDUtil::cleanUpMapOfPointers(_textureRegions);
    NGSTDUtil::cleanUpMapOfPointers(_animations);
}
