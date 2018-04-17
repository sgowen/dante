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
    
    void initWithJsonFile(const char* fileName, bool isBundled = true, bool useEncryption = false);
    bool getBool(std::string key);
    int getInt(std::string key);
    float getFloat(std::string key);
    double getDouble(std::string key);
    std::string getString(std::string key);
    
private:
    JsonFile* _jsonFile;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Config();
    ~Config();
    Config(const Config&);
    Config& operator=(const Config&);
};

#endif /* defined(__noctisgames__Config__) */
