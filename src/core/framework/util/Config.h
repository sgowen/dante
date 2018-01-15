//
//  Config.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Config__
#define __noctisgames__Config__

#include <string>

class JsonFile;

#define NG_CFG (Config::getInstance())

class Config
{
public:
    static Config* getInstance();
    
    void initWithJsonFile(const char* jsonFile, bool isBundled = true, bool useEncryption = false);
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
