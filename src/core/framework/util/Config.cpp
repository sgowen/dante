//
//  Config.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/util/Config.h>

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
    
    onLoaded();
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

void Config::onLoaded()
{
    _versionMajor = getInt("VersionMajor");
    _versionMinor = getInt("VersionMinor");
    _versionBuild = getInt("VersionBuild");
    _versionRevision = getInt("VersionRevision");
    _versionName = getString("VersionName");
    _steamGameDir = getString("SteamGameDir");
    _steamProductName = getString("SteamProductName");
    _steamProductDescription = getString("SteamProductDescription");
    _serverPort = getInt("ServerPort");
    _clientPort = getInt("ClientPort");
    _framebufferWidth = getInt("FramebufferWidth");
    _framebufferHeight = getInt("FramebufferHeight");
    _camWidth = getInt("CamWidth");
    _camHeight = getInt("CamHeight");
    _framebufferFilterMin = getString("FramebufferFilterMin");
    _framebufferFilterMag = getString("FramebufferFilterMag");
    _defaultTextureFilterMin = getString("DefaultTextureFilterMin");
    _defaultTextureFilterMag = getString("DefaultTextureFilterMag");
    _defaultTextureFilterMipMap = getBool("DefaultTextureFilterMipMap");
    _parallaxLayer0FactorX = getDouble("ParallaxLayer0FactorX");
    _parallaxLayer0FactorY = getDouble("ParallaxLayer0FactorY");
    _parallaxLayer1FactorX = getDouble("ParallaxLayer1FactorX");
    _parallaxLayer1FactorY = getDouble("ParallaxLayer1FactorY");
    _parallaxLayer2FactorX = getDouble("ParallaxLayer2FactorX");
    _parallaxLayer2FactorY = getDouble("ParallaxLayer2FactorY");
    _gravity = getFloat("Gravity");
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
