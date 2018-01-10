//
//  EntityLayout.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/10/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EntityLayout__
#define __noctisgames__EntityLayout__

#include <vector>

struct EntityLayoutDef
{
    uint32_t type;
    int x;
    int y;
};

class EntityLayout
{
public:
    static EntityLayout* getInstance();
    
    static void sDeserializerFunc(const char* data);
    
    void initWithJsonFile(const char* jsonFile, bool isBundled, bool useEncryption = false);
    void initWithJson(const char* json);
    
    std::vector<EntityLayoutDef*>& getEntityLayoutDefs();
    
private:
    std::vector<EntityLayoutDef*> _entityLayoutDefs;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    EntityLayout();
    ~EntityLayout();
    EntityLayout(const EntityLayout&);
    EntityLayout& operator=(const EntityLayout&);
};

#endif /* defined(__noctisgames__EntityLayout__) */
