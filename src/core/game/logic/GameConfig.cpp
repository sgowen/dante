//
//  GameConfig.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/logic/GameConfig.h>

#include <framework/util/Config.h>

GameConfig* GameConfig::getInstance()
{
    static GameConfig instance = GameConfig();
    return &instance;
}

void GameConfig::initWithJsonFile(const char* fileName, bool isBundled, bool useEncryption)
{
    FW_CFG->initWithJsonFile(fileName, isBundled, useEncryption);
    
    _versionMajor = FW_CFG->getInt("VersionMajor");
    _versionMinor = FW_CFG->getInt("VersionMinor");
    _versionBuild = FW_CFG->getInt("VersionBuild");
    _versionRevision = FW_CFG->getInt("VersionRevision");
    _versionName = FW_CFG->getString("VersionName");
    _serverPort = FW_CFG->getInt("ServerPort");
    _clientPort = FW_CFG->getInt("ClientPort");
    _steamGameDir = FW_CFG->getString("SteamGameDir");
    _steamProductName = FW_CFG->getString("SteamProductName");
    _steamProductDescription = FW_CFG->getString("SteamProductDescription");
    _framebufferWidth = FW_CFG->getInt("FramebufferWidth");
    _framebufferHeight = FW_CFG->getInt("FramebufferHeight");
    _camWidth = FW_CFG->getInt("CamWidth");
    _camHeight = FW_CFG->getInt("CamHeight");
    _framebufferFilterMin = FW_CFG->getString("FramebufferFilterMin");
    _framebufferFilterMag = FW_CFG->getString("FramebufferFilterMag");
    _defaultTextureFilterMin = FW_CFG->getString("DefaultTextureFilterMin");
    _defaultTextureFilterMag = FW_CFG->getString("DefaultTextureFilterMag");
    _defaultTextureFilterMipMap = FW_CFG->getBool("DefaultTextureFilterMipMap");
    _parallaxLayer0FactorX = FW_CFG->getDouble("ParallaxLayer0FactorX");
    _parallaxLayer0FactorY = FW_CFG->getDouble("ParallaxLayer0FactorY");
    _parallaxLayer1FactorX = FW_CFG->getDouble("ParallaxLayer1FactorX");
    _parallaxLayer1FactorY = FW_CFG->getDouble("ParallaxLayer1FactorY");
    _parallaxLayer2FactorX = FW_CFG->getDouble("ParallaxLayer2FactorX");
    _parallaxLayer2FactorY = FW_CFG->getDouble("ParallaxLayer2FactorY");
    _gravity = FW_CFG->getFloat("Gravity");
    _robotLightPositionFactorY = FW_CFG->getFloat("RobotLightPositionFactorY");
    _maxXVelocity = FW_CFG->getFloat("MaxRobotVelocityX");
    _maxYVelocity = FW_CFG->getFloat("MaxRobotVelocityY");
    _playerLightZ = FW_CFG->getFloat("PlayerLightZ");
    _behindPlayerLightZFactor = FW_CFG->getFloat("BehindPlayerLightZFactor");
    _frontPlayerLightZFactor = FW_CFG->getFloat("FrontPlayerLightZFactor");
    _playerLightColor[0] = FW_CFG->getFloat("PlayerLightColorR");
    _playerLightColor[1] = FW_CFG->getFloat("PlayerLightColorG");
    _playerLightColor[2] = FW_CFG->getFloat("PlayerLightColorB");
    _playerLightColor[3] = FW_CFG->getFloat("PlayerLightColorA");
    _ambientColor[0] = FW_CFG->getFloat("AmbientColorR");
    _ambientColor[1] = FW_CFG->getFloat("AmbientColorG");
    _ambientColor[2] = FW_CFG->getFloat("AmbientColorB");
    _ambientColor[3] = FW_CFG->getFloat("AmbientColorA");
    _fallOff[0] = FW_CFG->getFloat("LightFalloffX");
    _fallOff[1] = FW_CFG->getFloat("LightFalloffY");
    _fallOff[2] = FW_CFG->getFloat("LightFalloffZ");
    _tempStaticLight1[0] = FW_CFG->getFloat("TempStaticLight1X");
    _tempStaticLight1[1] = FW_CFG->getFloat("TempStaticLight1Y");
    _tempStaticLight1[2] = FW_CFG->getFloat("TempStaticLight1R");
    _tempStaticLight1[3] = FW_CFG->getFloat("TempStaticLight1G");
    _tempStaticLight1[4] = FW_CFG->getFloat("TempStaticLight1B");
    _tempStaticLight1[5] = FW_CFG->getFloat("TempStaticLight1A");
    _tempStaticLight2[0] = FW_CFG->getFloat("TempStaticLight2X");
    _tempStaticLight2[1] = FW_CFG->getFloat("TempStaticLight2Y");
    _tempStaticLight2[2] = FW_CFG->getFloat("TempStaticLight2R");
    _tempStaticLight2[3] = FW_CFG->getFloat("TempStaticLight2G");
    _tempStaticLight2[4] = FW_CFG->getFloat("TempStaticLight2B");
    _tempStaticLight2[5] = FW_CFG->getFloat("TempStaticLight2A");
}

GameConfig::GameConfig()
{
    // Empty
}

GameConfig::~GameConfig()
{
    // Empty
}
