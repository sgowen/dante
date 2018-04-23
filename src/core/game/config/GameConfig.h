//
//  GameConfig.h
//  dante
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GameConfig__
#define __noctisgames__GameConfig__

#define GM_CFG (GameConfig::getInstance())

class GameConfig
{
public:
    static GameConfig* getInstance();
    
    float _maxCrawlerVelocityX;
    float _robotLightPositionFactorY;
    float _maxRobotVelocityX;
    float _maxRobotVelocityY;
    float _playerLightZ;
    float _behindPlayerLightZFactor;
    float _frontPlayerLightZFactor;
    float _playerLightColor[4];
    float _ambientColor[4];
    float _fallOff[3];
    float _tempStaticLight1[6];
    float _tempStaticLight2[6];
    
    void initWithJsonFile(const char* fileName, bool isBundled = true, bool useEncryption = false);
    
private:
    // ctor, copy ctor, and assignment should be private in a Singleton
    GameConfig();
    ~GameConfig();
    GameConfig(const GameConfig&);
    GameConfig& operator=(const GameConfig&);
};

#endif /* defined(__noctisgames__GameConfig__) */
