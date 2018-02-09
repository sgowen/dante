//
//  GameConfig.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GameConfig__
#define __noctisgames__GameConfig__

#define GM_CFG (GameConfig::getInstance())

#include <string>

class GameConfig
{
public:
    static GameConfig* getInstance();
    
    int _camWidth;
    int _camHeight;
    float _playerLightZ;
    float _gravity;
    float _maxXVelocity;
    float _maxYVelocity;
    double _parallaxLayer0FactorX;
    double _parallaxLayer0FactorY;
    double _parallaxLayer1FactorX;
    double _parallaxLayer1FactorY;
    double _parallaxLayer2FactorX;
    double _parallaxLayer2FactorY;
    float _playerLightColor[4];
    float _ambientColor[4];
    float _fallOff[3];
    float _behindPlayerLightZFactor;
    float _frontPlayerLightZFactor;
    float _robotLightPositionFactorY;
    float _tempStaticLight1[6];
    float _tempStaticLight2[6];
    int _versionMajor;
    int _versionMinor;
    int _versionBuild;
    int _versionRevision;
    std::string _versionName;
    int _serverPort;
    int _clientPort;
    std::string _steamGameDir;
    std::string _steamProductName;
    std::string _steamProductDescription;
    
    void initWithJsonFile(const char* fileName, bool isBundled = true, bool useEncryption = false);
    
private:    
    // ctor, copy ctor, and assignment should be private in a Singleton
    GameConfig();
    ~GameConfig();
    GameConfig(const GameConfig&);
    GameConfig& operator=(const GameConfig&);
};

#endif /* defined(__noctisgames__GameConfig__) */
