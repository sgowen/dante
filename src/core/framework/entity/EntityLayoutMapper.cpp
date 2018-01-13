//
//  EntityLayoutMapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/10/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "EntityLayoutMapper.h"

#include "framework/entity/Entity.h"

#include "framework/util/NGSTDUtil.h"
#include "framework/util/StringUtil.h"
#include <framework/file/portable/JsonFile.h>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

#include <assert.h>

EntityLayoutMapper* EntityLayoutMapper::getInstance()
{
    static EntityLayoutMapper instance = EntityLayoutMapper();
    return &instance;
}

void EntityLayoutMapper::sLayoutsDeserializerFunc(const char* data)
{
    EntityLayoutMapper::getInstance()->initWithJson(data);
}

void EntityLayoutMapper::sLayoutDeserializerFunc(const char* data)
{
    EntityLayoutMapper::getInstance()->loadEntityLayout(data);
}

void EntityLayoutMapper::initWithJsonFile(const char* path, bool isBundled, bool useEncryption)
{
    const char* finalPath;
#if defined __linux__ && !defined(__ANDROID__)
    std::string s("data/config/");
    s += std::string(path);
    finalPath = s.c_str();
#elif defined _WIN32
    std::string s("data\\config\\");
    s += std::string(path);
    finalPath = s.c_str();
#else
    finalPath = path;
#endif
    
    JsonFile jsonFile(finalPath, isBundled, useEncryption);
    jsonFile.setDeserializerFunc(sLayoutsDeserializerFunc);
    jsonFile.load();
}

void EntityLayoutMapper::initWithJson(const char* data)
{
    _layouts.clear();
    
    using namespace rapidjson;
    
    Document d;
    d.Parse<0>(data);
    
    assert(d.IsObject());
    for (Value::ConstMemberIterator i = d.MemberBegin(); i != d.MemberEnd(); ++i)
    {
        const Value& iv = i->value;
        assert(iv.IsString());
        
        std::string keyStr = i->name.GetString();
        assert(keyStr.length() == 4);
        
        const char* chars = keyStr.c_str();
        
        uint32_t key = (uint32_t)chars[0] << 24 |
        (uint32_t)chars[1] << 16 |
        (uint32_t)chars[2] << 8  |
        (uint32_t)chars[3];
        
        assert(_layouts.find(key) == _layouts.end());
        
        _layouts[key] = iv.GetString();
    }
}

void EntityLayoutMapper::loadEntityLayout(uint32_t name)
{
    _entityLayoutDef.layers.clear();
    _entityLayoutDef.staticEntities.clear();
    _entityLayoutDef.dynamicEntities.clear();
    
    std::string path = getJsonConfigFilePath(name);
    
    const char* finalPath;
#if defined __linux__ && !defined(__ANDROID__)
    std::string s("data/config/");
    s += path;
    finalPath = s.c_str();
#elif defined _WIN32
    std::string s("data\\config\\");
    s += path;
    finalPath = s.c_str();
#else
    finalPath = path.c_str();
#endif
    
    JsonFile jsonFile(finalPath);
    jsonFile.setDeserializerFunc(sLayoutDeserializerFunc);
    jsonFile.load();
}

EntityLayoutDef& EntityLayoutMapper::getEntityLayoutDef()
{
    return _entityLayoutDef;
}

void EntityLayoutMapper::loadEntityLayout(const char* data)
{
    using namespace rapidjson;
    
    Document d;
    d.Parse<0>(data);
    
    if (d.HasMember("layers"))
    {
        Value& v = d["layers"];
        
        assert(v.IsArray());
        for (SizeType i = 0; i < v.Size(); ++i)
        {
            const Value& iv = v[i];
            assert(iv.IsObject());
            
            std::string keyStr = iv["type"].GetString();
            assert(keyStr.length() == 4);
            
            const char* chars = keyStr.c_str();
            
            uint32_t key = (uint32_t)chars[0] << 24 |
            (uint32_t)chars[1] << 16 |
            (uint32_t)chars[2] << 8  |
            (uint32_t)chars[3];
            
            EntityPosDef entityPosDef;
            entityPosDef.type = key;
            entityPosDef.x = static_cast<float>(iv["x"].GetInt());
            entityPosDef.y = static_cast<float>(iv["y"].GetInt());
            
            _entityLayoutDef.layers.push_back(entityPosDef);
        }
    }
    
    if (d.HasMember("staticEntities"))
    {
        Value& v = d["staticEntities"];
        
        assert(v.IsArray());
        for (SizeType i = 0; i < v.Size(); ++i)
        {
            const Value& iv = v[i];
            assert(iv.IsObject());
            
            std::string keyStr = iv["type"].GetString();
            assert(keyStr.length() == 4);
            
            const char* chars = keyStr.c_str();
            
            uint32_t key = (uint32_t)chars[0] << 24 |
            (uint32_t)chars[1] << 16 |
            (uint32_t)chars[2] << 8  |
            (uint32_t)chars[3];
            
            EntityPosDef entityPosDef;
            entityPosDef.type = key;
            entityPosDef.x = static_cast<float>(iv["x"].GetInt());
            entityPosDef.y = static_cast<float>(iv["y"].GetInt());
            
            _entityLayoutDef.staticEntities.push_back(entityPosDef);
        }
    }
    
    if (d.HasMember("dynamicEntities"))
    {
        Value& v = d["dynamicEntities"];
        
        assert(v.IsArray());
        for (SizeType i = 0; i < v.Size(); ++i)
        {
            const Value& iv = v[i];
            assert(iv.IsObject());
            
            std::string keyStr = iv["type"].GetString();
            assert(keyStr.length() == 4);
            
            const char* chars = keyStr.c_str();
            
            uint32_t key = (uint32_t)chars[0] << 24 |
            (uint32_t)chars[1] << 16 |
            (uint32_t)chars[2] << 8  |
            (uint32_t)chars[3];
            
            EntityPosDef entityPosDef;
            entityPosDef.type = key;
            entityPosDef.x = static_cast<float>(iv["x"].GetInt());
            entityPosDef.y = static_cast<float>(iv["y"].GetInt());
            
            _entityLayoutDef.dynamicEntities.push_back(entityPosDef);
        }
    }
}

std::string EntityLayoutMapper::getJsonConfigFilePath(uint32_t inFourCCName)
{
    auto q = _layouts.find(inFourCCName);
    
    assert(q != _layouts.end());
    
    return q->second;
}

EntityLayoutMapper::EntityLayoutMapper()
{
    // Empty
}

EntityLayoutMapper::~EntityLayoutMapper()
{
    _layouts.clear();
}
