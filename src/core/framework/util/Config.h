//
//  Config.h
//  noctisgames
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Config__
#define __noctisgames__Config__

#define FW_CFG (Config::getInstance())

#include <string>

class JsonFile;

class Config
{
public:
    static Config* getInstance();
    
    int _versionMajor;
    int _versionMinor;
    int _versionBuild;
    int _versionRevision;
    std::string _versionName;
    std::string _steamGameDir;
    std::string _steamProductName;
    std::string _steamProductDescription;
    int _serverPort;
    int _clientPort;
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
    
    void initWithJsonFile(const char* fileName, bool isBundled = true, bool useEncryption = false);
    bool getBool(std::string key);
    int getInt(std::string key);
    float getFloat(std::string key);
    double getDouble(std::string key);
    std::string getString(std::string key);
    
private:
    JsonFile* _jsonFile;
    
    void onLoaded();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Config();
    ~Config();
    Config(const Config&);
    Config& operator=(const Config&);
};

#endif /* defined(__noctisgames__Config__) */
