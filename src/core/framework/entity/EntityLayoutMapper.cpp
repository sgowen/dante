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

void EntityLayoutMapper::sDeserializerFunc(const char* data)
{
    EntityLayoutMapper::getInstance()->initWithJson(data);
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
    jsonFile.setDeserializerFunc(sDeserializerFunc);
    jsonFile.load();
}

void EntityLayoutMapper::initWithJson(const char* json)
{
    _layouts.clear();
    
    using namespace rapidjson;
    
    Document d;
    d.Parse<0>(json);
    
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

EntityLayoutMapper::EntityLayoutMapper()
{
    // Empty
}

EntityLayoutMapper::~EntityLayoutMapper()
{
    _layouts.clear();
}
