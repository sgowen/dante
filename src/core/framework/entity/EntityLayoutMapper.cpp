//
//  EntityLayoutMapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/10/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/entity/EntityLayoutMapper.h>

#include <framework/entity/Entity.h>

#include <framework/util/NGSTDUtil.h>
#include <framework/util/StringUtil.h>
#include <framework/file/portable/JsonFile.h>
#include <framework/file/portable/FileUtil.h>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

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

const char* EntityLayoutMapper::sLayoutSerializerFunc()
{
    return EntityLayoutMapper::getInstance()->save();
}

void EntityLayoutMapper::initWithJsonFile(const char* fileName, bool isBundled, bool useEncryption)
{
    std::string filePath = FileUtil::filePathForConfig(fileName);
    
    JsonFile jsonFile(filePath.c_str(), isBundled, useEncryption);
    jsonFile.setDeserializerFunc(sLayoutsDeserializerFunc);
    jsonFile.load();
}

void EntityLayoutMapper::initWithJson(const char* data)
{
    _maps.clear();
    _layouts.clear();
    
    using namespace rapidjson;
    
    Document d;
    d.Parse<kParseStopWhenDoneFlag>(data);
    
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
        
        std::string fileName = iv.GetString();
        
        _maps.push_back(MapDef(key, keyStr, fileName));
        _layouts[key] = fileName;
    }
}

void EntityLayoutMapper::loadEntityLayout(uint32_t name)
{
    std::string path = getJsonConfigFilePath(name);
    std::string filePath = FileUtil::filePathForConfig(path.c_str());
    
    loadEntityLayout(filePath);
}

void EntityLayoutMapper::loadEntityLayout(std::string filePath)
{
    _entityLayoutDef.entities.clear();
    
    JsonFile jsonFile(filePath.c_str());
    jsonFile.setDeserializerFunc(sLayoutDeserializerFunc);
    jsonFile.load();
}

EntityLayoutDef& EntityLayoutMapper::getEntityLayoutDef()
{
    return _entityLayoutDef;
}

std::vector<MapDef>& EntityLayoutMapper::getMaps()
{
    return _maps;
}

void EntityLayoutMapper::loadEntityLayout(const char* data)
{
    using namespace rapidjson;
    
    Document d;
    d.Parse<kParseStopWhenDoneFlag>(data);
    
    if (d.HasMember("entities"))
    {
        Value& v = d["entities"];
        
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
            
            EntityPosDef epd;
            epd.type = key;
            epd.x = iv["x"].GetInt();
            epd.y = iv["y"].GetInt();
            epd.w = iv.HasMember("w") ? iv["w"].GetInt() : 0;
            epd.h = iv.HasMember("h") ? iv["h"].GetInt() : 0;
            
            _entityLayoutDef.entities.push_back(epd);
        }
    }
}

const char* EntityLayoutMapper::save()
{
    assert(_layoutToSave);
    
    using namespace rapidjson;
    
    static StringBuffer s;
    PrettyWriter<StringBuffer> w(s);
    
    s.Clear();

    w.StartObject();
    
    if (_layoutToSave->entities.size() > 0)
    {
        w.String("entities");
        w.StartArray();
        for (EntityPosDef epd : _layoutToSave->entities)
        {
            w.StartObject();
            {
                w.String("type");
                char chars[5];
                chars[4] = '\0';
                chars[3] = (char)(epd.type & 0xFF);
                chars[2] = (char)(epd.type >> 8 & 0xFF);
                chars[1] = (char)(epd.type >> 16 & 0xFF);
                chars[0] = (char)(epd.type >> 24 & 0xFF);
                std::string type = std::string(chars);
                w.String(type.c_str());
            }
            {
                w.String("x");
                w.Int(epd.x);
            }
            {
                w.String("y");
                w.Int(epd.y);
            }
            if (epd.w > 0)
            {
                w.String("w");
                w.Int(epd.w);
            }
            if (epd.h > 0)
            {
                w.String("h");
                w.Int(epd.h);
            }
            w.EndObject();
        }
        w.EndArray();
    }
    
    w.EndObject();
    
    return s.GetString();
}

void EntityLayoutMapper::saveEntityLayout(uint32_t name, EntityLayoutDef* layout)
{
    std::string path = getJsonConfigFilePath(name);
    std::string filePath = FileUtil::filePathForConfig(path.c_str());
    
    saveEntityLayout(filePath, layout);
}

void EntityLayoutMapper::saveEntityLayout(std::string filePath, EntityLayoutDef* layout)
{
    assert(layout);
    
    _layoutToSave = layout;
    
    JsonFile jsonFile(filePath.c_str());
    jsonFile.setSerializerFunc(sLayoutSerializerFunc);
    jsonFile.save();
}

std::string EntityLayoutMapper::getJsonConfigFilePath(uint32_t inFourCCName)
{
    auto q = _layouts.find(inFourCCName);
    
    assert(q != _layouts.end());
    
    return q->second;
}

EntityLayoutMapper::EntityLayoutMapper() : _layoutToSave(NULL)
{
    // Empty
}

EntityLayoutMapper::~EntityLayoutMapper()
{
    _maps.clear();
    _layouts.clear();
}
