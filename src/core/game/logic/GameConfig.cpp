//
//  GameConfig.cpp
//  noctisgames
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
    
    _maxCrawlerVelocityX = FW_CFG->getFloat("MaxCrawlerVelocityX");
    _robotLightPositionFactorY = FW_CFG->getFloat("RobotLightPositionFactorY");
    _maxRobotVelocityX = FW_CFG->getFloat("MaxRobotVelocityX");
    _maxRobotVelocityY = FW_CFG->getFloat("MaxRobotVelocityY");
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

void GameConfig::setWorld(World* world)
{
    _world = world;
}

World* GameConfig::getWorld()
{
    return _world;
}

GameConfig::GameConfig() : _world(NULL)
{
    // Empty
}

GameConfig::~GameConfig()
{
    // Empty
}
