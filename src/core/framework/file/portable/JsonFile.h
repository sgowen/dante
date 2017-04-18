//
//  JsonFile.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__JsonFile__
#define __noctisgames__JsonFile__

#include <map>
#include <string>

class JsonFile
{
public:
    JsonFile(const char* filePath, bool useEncryption = false);
    
    void save();
    
    void load();
    
    void clear();
    
    std::string findValue(std::string key);
    
    void setValue(std::string key, std::string value);

private:
    const char* m_filePath;
    bool m_useEncryption;
    std::map<std::string, std::string> m_keyValues;
};

#endif /* defined(__noctisgames__JsonFile__) */
