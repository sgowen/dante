//
//  EntityMapper.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/entity/EntityMapper.h>

#include <framework/entity/Entity.h>
#include <framework/entity/EntityLayoutMapper.h>
#include <framework/entity/EntityController.h>
#include <framework/entity/EntityNetworkController.h>

#include <framework/entity/EntityController.h>
#include <framework/util/NGSTDUtil.h>
#include <framework/util/StringUtil.h>
#include <framework/file/portable/JsonFile.h>
#include <framework/file/portable/FileUtil.h>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

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
        uint32_t key = StringUtil::fourCharFromString(keyStr);
        
        assert(_entityDescriptorsMap.find(key) == _entityDescriptorsMap.end());
        
        EntityDef* entry = new EntityDef();
        
        entry->type = key;
        entry->name = iv["name"].GetString();
        entry->typeName = keyStr;
        entry->controller = iv.HasMember("controller") ? iv["controller"].GetString() : "Entity";
        
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
        entry->width = iv["width"].GetInt();
        entry->height = iv["height"].GetInt();
        entry->stateSensitive = iv.HasMember("stateSensitive") ? iv["stateSensitive"].GetBool() : false;
        
        _entityDescriptorsMap[key] = entry;
        _entityDescriptors.push_back(entry);
    }
}

Entity* EntityMapper::createEntity(EntityInstanceDef* eid, bool isServer)
{
    return createEntityFromDef(getEntityDef(eid->type), eid, isServer);
}

Entity* EntityMapper::createEntityFromDef(EntityDef* inEntityDef, EntityInstanceDef* eid, bool isServer)
{
    EntityDef ed = *inEntityDef;
    
    ed.ID = eid->ID;
    ed.x = eid->x;
    ed.y = eid->y;
    ed.width = eid->w > 0 ? eid->w : inEntityDef->width;
    ed.height = eid->h > 0 ? eid->h : inEntityDef->height;
    ed.server = isServer;
    
    return new Entity(ed);
}

EntityDef* EntityMapper::getEntityDef(uint32_t inFourCCName)
{
    auto q = _entityDescriptorsMap.find(inFourCCName);
    
    assert(q != _entityDescriptorsMap.end());
    
    EntityDef* ret = q->second;
    
    return ret;
}

void EntityMapper::registerFunction(std::string name, EntityControllerCreationFunc func)
{
    assert(func);
    
    _entityControllerCreationFunctionMap[name] = func;
}

void EntityMapper::registerFunction(std::string name, EntityNetworkControllerCreationFunc func)
{
    assert(func);
    
    _entityNetworkControllerCreationFunctionMap[name] = func;
}

EntityController* EntityMapper::createEntityController(EntityDef& ed, Entity* e)
{
    EntityControllerCreationFunc func = _entityControllerCreationFunctionMap[ed.controller];
    
    assert(func);
    
    return func(e);
}

EntityNetworkController* EntityMapper::createEntityNetworkController(EntityDef& ed, Entity* e)
{
    EntityNetworkControllerCreationFunc func = _entityNetworkControllerCreationFunctionMap[ed.controller];
    
    assert(func);
    
    return func(e, ed.server);
}

const std::vector<EntityDef*>& EntityMapper::getEntityDescriptors()
{
    return _entityDescriptors;
}

const std::map<std::string, EntityControllerCreationFunc>& EntityMapper::getEntityControllerMap()
{
    return _entityControllerCreationFunctionMap;
}

const std::map<std::string, EntityNetworkControllerCreationFunc>& EntityMapper::getEntityNetworkControllerMap()
{
    return _entityNetworkControllerCreationFunctionMap;
}

EntityMapper::EntityMapper()
{
    registerFunction("Entity", EntityController::create);
    registerFunction("Entity", EntityNetworkController::create);
}

EntityMapper::~EntityMapper()
{
    NGSTDUtil::cleanUpMapOfPointers(_entityDescriptorsMap);
    _entityDescriptors.clear();
}
