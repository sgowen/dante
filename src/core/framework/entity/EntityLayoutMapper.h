//
//  EntityLayoutMapper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/10/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EntityLayoutMapper__
#define __noctisgames__EntityLayoutMapper__

#include <map>

class EntityLayoutMapper
{
public:
    static EntityLayoutMapper* getInstance();
    
    static void sDeserializerFunc(const char* data);
    
    void initWithJsonFile(const char* jsonFile, bool isBundled = true, bool useEncryption = false);
    void initWithJson(const char* json);
    
private:
    std::map<uint32_t, std::string> _layouts;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    EntityLayoutMapper();
    ~EntityLayoutMapper();
    EntityLayoutMapper(const EntityLayoutMapper&);
    EntityLayoutMapper& operator=(const EntityLayoutMapper&);
};

#endif /* defined(__noctisgames__EntityLayoutMapper__) */
