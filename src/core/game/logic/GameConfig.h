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

class World;

class GameConfig
{
public:
    static GameConfig* getInstance();
    
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
    int _framebufferWidth;
    int _framebufferHeight;
    int _camWidth;
    int _camHeight;
    std::string _framebufferFilterMin;
    std::string _framebufferFilterMag;
    std::string _defaultTextureFilterMin;
    std::string _defaultTextureFilterMag;
    bool _defaultTextureFilterMipMap;
    double _parallaxLayer0FactorX;
    double _parallaxLayer0FactorY;
    double _parallaxLayer1FactorX;
    double _parallaxLayer1FactorY;
    double _parallaxLayer2FactorX;
    double _parallaxLayer2FactorY;
    float _gravity;
    float _robotLightPositionFactorY;
    float _maxXVelocity;
    float _maxYVelocity;
    float _playerLightZ;
    float _behindPlayerLightZFactor;
    float _frontPlayerLightZFactor;
    float _playerLightColor[4];
    float _ambientColor[4];
    float _fallOff[3];
    float _tempStaticLight1[6];
    float _tempStaticLight2[6];
    
    void initWithJsonFile(const char* fileName, bool isBundled = true, bool useEncryption = false);
    void setWorld(World* world);
    World* getWorld();
    
private:
    World* _world;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    GameConfig();
    ~GameConfig();
    GameConfig(const GameConfig&);
    GameConfig& operator=(const GameConfig&);
};

#endif /* defined(__noctisgames__GameConfig__) */
