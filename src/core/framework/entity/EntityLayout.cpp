//
//  EntityLayout.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/10/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "EntityLayout.h"

#include "framework/entity/Entity.h"

#include "framework/util/NGSTDUtil.h"
#include "framework/util/StringUtil.h"
#include <framework/file/portable/JsonFile.h>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"

#include <assert.h>

EntityLayout* EntityLayout::getInstance()
{
    static EntityLayout instance = EntityLayout();
    return &instance;
}

void EntityLayout::sDeserializerFunc(const char* data)
{
    EntityLayout::getInstance()->initWithJson(data);
}

void EntityLayout::initWithJsonFile(const char* path, bool isBundled, bool useEncryption)
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

void EntityLayout::initWithJson(const char* json)
{
    _entityLayoutDefs.clear();
    
    using namespace rapidjson;
    
    Document d;
    d.Parse<0>(json);
}

std::vector<EntityLayoutDef*>& EntityLayout::getEntityLayoutDefs()
{
    return _entityLayoutDefs;
}

EntityLayout::EntityLayout()
{
    // Empty
}

EntityLayout::~EntityLayout()
{
    NGSTDUtil::cleanUpVectorOfPointers(_entityLayoutDefs);
}
