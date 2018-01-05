//
//  Assets.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/portable/Assets.h"

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
    std::string s("data/textures/");
    s += std::string(path);
    finalPath = s.c_str();
#elif defined _WIN32
    std::string s("data\\textures\\");
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
    getTextureDescriptors().clear();
    getTextureRegionMap().clear();
    getAnimationsMap().clear();
    
    using namespace rapidjson;
    
    Document d;
    d.Parse<0>(json);
    
    assert(d.HasMember("textures"));
    assert(d.HasMember("textureRegions"));
    assert(d.HasMember("animations"));
    
    {
        Value& v = d["textures"];
        
        assert(v.IsArray());
        for (SizeType i = 0; i < v.Size(); ++i)
        {
            const Value& iv = v[i];
            std::string textureName = iv["textureName"].GetString();
            bool repeatS = iv["repeatS"].GetBool();
            bool isEncrypted = iv["isEncrypted"].GetBool();
            
            _textures.push_back(new NGTextureDesc(textureName, repeatS, isEncrypted));
        }
    }
    
    {
        Value& v = d["textureRegions"];
        
        assert(v.IsObject());
        for (Value::ConstMemberIterator i = v.MemberBegin(); i != v.MemberEnd(); ++i)
        {
            const Value& iv = i->value;
            assert(iv.IsObject());
            
            std::string key = i->name.GetString();
            std::string textureName = iv["textureName"].GetString();
            int x = iv["x"].GetInt();
            int y = iv["y"].GetInt();
            int regionWidth = iv["regionWidth"].GetInt();
            int regionHeight = iv["regionHeight"].GetInt();
            int textureWidth = iv["textureWidth"].GetInt();
            int textureHeight = iv["textureHeight"].GetInt();
            
            assert(!_textureRegions[key]);
            _textureRegions[key] = new TextureRegion(textureName, x, y, regionWidth, regionHeight, textureWidth, textureHeight);
        }
    }
    
    {
        Value& v = d["animations"];
        
        assert(v.IsObject());
        for (Value::ConstMemberIterator i = v.MemberBegin(); i != v.MemberEnd(); ++i)
        {
            const Value& iv = i->value;
            assert(iv.IsObject());
            
            std::string key = i->name.GetString();
            std::string textureName = iv["textureName"].GetString();
            int x = iv["x"].GetInt();
            int y = iv["y"].GetInt();
            int regionWidth = iv["regionWidth"].GetInt();
            int regionHeight = iv["regionHeight"].GetInt();
            int animationWidth = iv["animationWidth"].GetInt();
            int animationHeight = iv["animationHeight"].GetInt();
            int textureWidth = iv["textureWidth"].GetInt();
            int textureHeight = iv["textureHeight"].GetInt();
            bool looping = iv["looping"].GetBool();
            
            int firstLoopingFrame = iv["firstLoopingFrame"].GetInt();
            int xPadding = iv["xPadding"].GetInt();
            int yPadding = iv["yPadding"].GetInt();
            
            assert(!_animations[key]);
            
            if (iv.HasMember("frameTimes"))
            {
                const Value& va = iv["frameTimes"];
                assert(va.IsArray());
                
                std::vector<float> frameTimes;
                for (SizeType i = 0; i < va.Size(); ++i)
                {
                    const Value& iva = va[i];
                    frameTimes.push_back(iva.GetFloat());
                }
                
                _animations[key] = new NGAnimation(textureName, x, y, regionWidth, regionHeight, animationWidth, animationHeight, textureWidth, textureHeight, looping, firstLoopingFrame, xPadding, yPadding, frameTimes);
            }
            else
            {
                float frameTime = iv["frameTime"].GetFloat();
                int numFrames = iv["numFrames"].GetInt();
                
                _animations[key] = new NGAnimation(textureName, x, y, regionWidth, regionHeight, animationWidth, animationHeight, textureWidth, textureHeight, looping, firstLoopingFrame, xPadding, yPadding, frameTime, numFrames);
            }
        }
    }
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
    NGSTDUtil::cleanUpMapOfPointers(_textureRegions);
    NGSTDUtil::cleanUpMapOfPointers(_animations);
}
