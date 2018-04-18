//
//  EntityLayoutMapper.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 1/10/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/entity/EntityLayoutMapper.h>

#include <framework/entity/Entity.h>
#include <framework/entity/EntityIDManager.h>

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
        uint32_t key = StringUtil::fourCharFromString(keyStr);
        
        assert(_layouts.find(key) == _layouts.end());
        
        std::string fileName = iv.GetString();
        
        _maps.push_back(MapDef(key, keyStr, fileName));
        _layouts[key] = fileName;
    }
}

void EntityLayoutMapper::loadEntityLayout(uint32_t name, EntityIDManager* entityIDManager)
{
    std::string path = getJsonConfigFilePath(name);
    std::string filePath = FileUtil::filePathForConfig(path.c_str());
    
    loadEntityLayout(filePath, entityIDManager);
}

void EntityLayoutMapper::loadEntityLayout(std::string filePath, EntityIDManager* entityIDManager)
{
    assert(entityIDManager);
    
    _entityIDManager = entityIDManager;
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
            uint32_t key = StringUtil::fourCharFromString(keyStr);
            
            uint32_t ID = _entityIDManager->getNextStaticEntityID();
            uint32_t type = key;
            uint32_t x = iv["x"].GetUint();
            uint32_t y = iv["y"].GetUint();
            uint32_t w = iv.HasMember("w") ? iv["w"].GetUint() : 0;
            uint32_t h = iv.HasMember("h") ? iv["h"].GetUint() : 0;
            
            _entityLayoutDef.entities.push_back(EntityInstanceDef(ID, type, x, y, w, h));
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
        for (EntityInstanceDef eid : _layoutToSave->entities)
        {
            w.StartObject();
            {
                w.String("type");
                std::string type = StringUtil::stringFromFourChar(eid.type);
                w.String(type.c_str());
            }
            {
                w.String("x");
                w.Uint(eid.x);
            }
            {
                w.String("y");
                w.Uint(eid.y);
            }
            if (eid.w > 0)
            {
                w.String("w");
                w.Uint(eid.w);
            }
            if (eid.h > 0)
            {
                w.String("h");
                w.Uint(eid.h);
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
    if (inFourCCName == 'TEST')
    {
        return std::string("test.cfg");
    }
    
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
