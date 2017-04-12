//
//  SaveData.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/9/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SaveData__
#define __noctisgames__SaveData__

#define NG_SAVE_DATA (SaveData::getInstance())

#include <map>
#include <string>

class SaveData
{
public:
    static SaveData* getInstance();
    
    void config(const char* filePath);
    
    void save();
    
    void load();
    
    void clear();
    
    std::string findValue(std::string key);
    
    std::map<std::string, std::string>& getKeyValues();

private:
    const char* m_filePath;
    std::map<std::string, std::string> m_keyValues;
    
	// ctor, copy ctor, and assignment should be private in a Singleton
	SaveData();
	SaveData(const SaveData&);
	SaveData& operator=(const SaveData&);
};

#endif /* defined(__noctisgames__SaveData__) */
