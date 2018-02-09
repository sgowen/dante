//
//  Config.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Config.h"

#include <framework/file/portable/JsonFile.h>

#include <framework/file/portable/FileUtil.h>
#include <framework/util/StringUtil.h>

Config* Config::getInstance()
{
    static Config instance = Config();
    return &instance;
}

void Config::initWithJsonFile(const char* fileName, bool isBundled, bool useEncryption)
{
    if (_jsonFile)
    {
        delete _jsonFile;
    }
    
    std::string filePath = FileUtil::filePathForConfig(fileName);
    
    _jsonFile = new JsonFile(filePath.c_str(), isBundled, useEncryption);
    _jsonFile->load();
}

bool Config::getBool(std::string key)
{
    std::string val = _jsonFile->findValue(key);
    bool ret = StringUtil::stringToBool(val);
    return ret;
}

int Config::getInt(std::string key)
{
    std::string val = _jsonFile->findValue(key);
    int ret = StringUtil::stringToNumber<int>(val);
    return ret;
}

float Config::getFloat(std::string key)
{
    std::string val = _jsonFile->findValue(key);
    float ret = StringUtil::stringToNumber<float>(val);
    return ret;
}

double Config::getDouble(std::string key)
{
    std::string val = _jsonFile->findValue(key);
    double ret = StringUtil::stringToNumber<double>(val);
    return ret;
}

std::string Config::getString(std::string key)
{
    return _jsonFile->findValue(key);
}

Config::Config() : _jsonFile(NULL)
{
    // Empty
}

Config::~Config()
{
    if (_jsonFile)
    {
        delete _jsonFile;
    }
}
