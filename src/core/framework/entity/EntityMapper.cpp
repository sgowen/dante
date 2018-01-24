//
//  EntityMapper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "EntityMapper.h"

#include "framework/entity/Entity.h"
#include "framework/entity/EntityController.h"

#include "framework/util/NGSTDUtil.h"
#include "framework/util/StringUtil.h"
#include <framework/file/portable/JsonFile.h>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

#include <assert.h>

EntityMapper* EntityMapper::getInstance()
{
    static EntityMapper instance = EntityMapper();
    return &instance;
}

void EntityMapper::sDeserializerFunc(const char* data)
{
    EntityMapper::getInstance()->initWithJson(data);
}

void EntityMapper::initWithJsonFile(const char* path, bool isBundled, bool useEncryption)
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

void EntityMapper::initWithJson(const char* json)
{
    NGSTDUtil::cleanUpMapOfPointers(_entityDescriptors);
    
    using namespace rapidjson;
    
    Document d;
    d.Parse<kParseStopWhenDoneFlag>(json);
    
    if (d.HasMember("layers"))
    {
        Value& v = d["layers"];
        
        assert(v.IsObject());
        for (Value::ConstMemberIterator i = v.MemberBegin(); i != v.MemberEnd(); ++i)
        {
            const Value& iv = i->value;
            assert(iv.IsObject());
            
            std::string keyStr = i->name.GetString();
            assert(keyStr.length() == 4);
            
            const char* chars = keyStr.c_str();
            
            uint32_t key = (uint32_t)chars[0] << 24 |
            (uint32_t)chars[1] << 16 |
            (uint32_t)chars[2] << 8  |
            (uint32_t)chars[3];
            
            assert(_entityDescriptors.find(key) == _entityDescriptors.end());
            
            EntityDef* entry = new EntityDef();
            
            entry->type = key;
            entry->controller = std::string("DefaultController");
            
            {
                const Value& v = iv["mappings"];
                assert(v.IsObject());
                for (Value::ConstMemberIterator i = v.MemberBegin(); i != v.MemberEnd(); ++i)
                {
                    std::string name = i->name.GetString();
                    int state = StringUtil::stringToNumber<int>(name);
                    std::string value = i->value.GetString();
                    entry->mappings.insert(std::make_pair(state, value));
                }
            }
            
            entry->bodyFlags = 0;
            entry->width = static_cast<float>(iv["width"].GetInt());
            entry->height = static_cast<float>(iv["height"].GetInt());
            entry->layer = iv["layer"].GetInt();
            entry->stateSensitive = false;
            
            _entityDescriptors[key] = entry;
        }
    }
        
    if (d.HasMember("entities"))
    {
        Value& v = d["entities"];
        
        assert(v.IsObject());
        for (Value::ConstMemberIterator i = v.MemberBegin(); i != v.MemberEnd(); ++i)
        {
            const Value& iv = i->value;
            assert(iv.IsObject());
            
            std::string keyStr = i->name.GetString();
            assert(keyStr.length() == 4);
            
            const char* chars = keyStr.c_str();
            
            uint32_t key = (uint32_t)chars[0] << 24 |
            (uint32_t)chars[1] << 16 |
            (uint32_t)chars[2] << 8  |
            (uint32_t)chars[3];
            
            assert(_entityDescriptors.find(key) == _entityDescriptors.end());
            
            EntityDef* entry = new EntityDef();
            
            entry->type = key;
            entry->controller = iv["controller"].GetString();
            
            {
                const Value& v = iv["mappings"];
                assert(v.IsObject());
                for (Value::ConstMemberIterator i = v.MemberBegin(); i != v.MemberEnd(); ++i)
                {
                    std::string name = i->name.GetString();
                    int state = StringUtil::stringToNumber<int>(name);
                    std::string value = i->value.GetString();
                    entry->mappings.insert(std::make_pair(state, value));
                }
            }
            {
                const Value& v = iv["fixtures"];
                assert(v.IsArray());
                for (SizeType i = 0; i < v.Size(); ++i)
                {
                    const Value& iv = v[i];
                    assert(iv.IsObject());
                    FixtureDef fixtureDef;
                    fixtureDef.restitution = iv["restitution"].GetFloat();
                    fixtureDef.density = iv["density"].GetFloat();
                    fixtureDef.friction = iv["friction"].GetFloat();
                    fixtureDef.flags = iv["flags"].GetInt();
                    
                    {
                        const Value& iiv = iv["vertices"];
                        assert(iiv.IsArray());
                        for (SizeType i = 0; i < iiv.Size(); ++i)
                        {
                            const Value& iiiv = iiv[i];
                            assert(iiiv.IsObject());
                            float x = iiiv["x"].GetFloat();
                            float y = iiiv["y"].GetFloat();
                            fixtureDef.vertices.push_back(b2Vec2(x, y));
                        }
                    }
                    
                    float x = 0;
                    float y = 0;
                    if (iv.HasMember("center"))
                    {
                        const Value& iiv = iv["center"];
                        assert(iiv.IsObject());
                        x = iiv["x"].GetFloat();
                        y = iiv["y"].GetFloat();
                    }
                    fixtureDef.center.Set(x, y);
                    
                    entry->fixtures.push_back(fixtureDef);
                }
            }
            entry->bodyFlags = iv["bodyFlags"].GetInt();
            entry->width = static_cast<float>(iv["width"].GetInt());
            entry->height = static_cast<float>(iv["height"].GetInt());
            entry->layer = iv["layer"].GetInt();
            entry->stateSensitive = iv["stateSensitive"].GetBool();
            
            _entityDescriptors[key] = entry;
        }
    }
}

Entity* EntityMapper::createEntity(uint32_t inFourCCName, int x, int y, bool isServer)
{
    auto q = _entityDescriptors.find(inFourCCName);
    
    assert(q != _entityDescriptors.end());
    
    EntityDef* entityDef = q->second;
    
    return new Entity(*entityDef, x, y, isServer);
}

void EntityMapper::registerFunction(std::string name, EntityControllerCreationFunc inCreationFunction)
{
    assert(inCreationFunction);
    
    _nameToEntityControllerCreationFunctionMap[name] = inCreationFunction;
}

EntityController* EntityMapper::createEntityController(std::string name, Entity* inEntity)
{
    EntityControllerCreationFunc creationFunc = _nameToEntityControllerCreationFunctionMap[name];
    
    assert(creationFunc);
    
    return creationFunc(inEntity);
}

const std::map<uint32_t, EntityDef*>& EntityMapper::getEntityDescriptors()
{
    return _entityDescriptors;
}

const std::map<std::string, EntityControllerCreationFunc>& EntityMapper::getEntityControllerMap()
{
    return _nameToEntityControllerCreationFunctionMap;
}

EntityMapper::EntityMapper()
{
    // Empty
}

EntityMapper::~EntityMapper()
{
    NGSTDUtil::cleanUpMapOfPointers(_entityDescriptors);
}
