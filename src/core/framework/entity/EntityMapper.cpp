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
#include <framework/file/portable/FileUtil.h>

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

void EntityMapper::initWithJsonFile(const char* fileName, bool isBundled, bool useEncryption)
{
    std::string filePath = FileUtil::filePathForConfig(fileName);
    
    JsonFile jsonFile(filePath.c_str(), isBundled, useEncryption);
    jsonFile.setDeserializerFunc(sDeserializerFunc);
    jsonFile.load();
}

void EntityMapper::initWithJson(const char* json)
{
    NGSTDUtil::cleanUpMapOfPointers(_entityDescriptorsMap);
    _entityDescriptors.clear();
    
    using namespace rapidjson;
    
    Document d;
    d.Parse<kParseStopWhenDoneFlag>(json);
    
    assert(d.IsObject());
    for (Value::ConstMemberIterator i = d.MemberBegin(); i != d.MemberEnd(); ++i)
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
        
        assert(_entityDescriptorsMap.find(key) == _entityDescriptorsMap.end());
        
        EntityDef* entry = new EntityDef();
        
        entry->type = key;
        entry->name = iv["name"].GetString();
        entry->typeName = keyStr;
        entry->controller = iv.HasMember("controller") ? iv["controller"].GetString() : "DefaultController";
        
        {
            const Value& v = iv["textureMappings"];
            assert(v.IsObject());
            for (Value::ConstMemberIterator i = v.MemberBegin(); i != v.MemberEnd(); ++i)
            {
                std::string name = i->name.GetString();
                int state = StringUtil::stringToNumber<int>(name);
                std::string value = i->value.GetString();
                entry->textureMappings.insert(std::make_pair(state, value));
            }
        }
        
        if (iv.HasMember("soundMappings"))
        {
            const Value& v = iv["soundMappings"];
            assert(v.IsObject());
            for (Value::ConstMemberIterator i = v.MemberBegin(); i != v.MemberEnd(); ++i)
            {
                std::string name = i->name.GetString();
                int state = StringUtil::stringToNumber<int>(name);
                int soundId = i->value.GetInt();
                entry->soundMappings.insert(std::make_pair(state, soundId));
            }
        }
        
        if (iv.HasMember("fixtures"))
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
        entry->bodyFlags = iv.HasMember("bodyFlags") ? iv["bodyFlags"].GetInt() : 0;
        entry->width = static_cast<float>(iv["width"].GetInt());
        entry->height = static_cast<float>(iv["height"].GetInt());
        entry->layer = iv["layer"].GetInt();
        entry->stateSensitive = iv.HasMember("stateSensitive") ? iv["stateSensitive"].GetBool() : false;
        
        _entityDescriptorsMap[key] = entry;
        _entityDescriptors.push_back(entry);
    }
}

Entity* EntityMapper::createEntity(uint32_t inFourCCName, int x, int y, bool isServer)
{
    auto q = _entityDescriptorsMap.find(inFourCCName);
    
    assert(q != _entityDescriptorsMap.end());
    
    EntityDef* entityDef = q->second;
    
    return createEntityFromDef(entityDef, x, y, isServer);
}

Entity* EntityMapper::createEntityFromDef(EntityDef* entityDef, int x, int y, bool isServer)
{
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

const std::vector<EntityDef*>& EntityMapper::getEntityDescriptors()
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
    NGSTDUtil::cleanUpMapOfPointers(_entityDescriptorsMap);
    _entityDescriptors.clear();
}
