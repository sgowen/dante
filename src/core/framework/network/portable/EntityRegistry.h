//
//  EntityRegistry.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EntityRegistry__
#define __noctisgames__EntityRegistry__

#include "Entity.h"

#include "NetworkConstants.h"

#include <unordered_map>

typedef void (*HandleEntityCreatedFunc)(Entity* inEntity);
typedef Entity* (*EntityCreationFunc)();

class EntityRegistry
{
public:
    static EntityRegistry* getInstance();
    
    void init(HandleEntityCreatedFunc handleEntityCreatedFunc);
    
    void registerCreationFunction(uint32_t inFourCCName, EntityCreationFunc inCreationFunction);
    
    Entity* createEntity(uint32_t inFourCCName);
    
private:
    std::unordered_map<uint32_t, EntityCreationFunc> m_nameToEntityCreationFunctionMap;
    HandleEntityCreatedFunc m_handleEntityCreatedFunc;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    EntityRegistry();
    EntityRegistry(const EntityRegistry&);
    EntityRegistry& operator=(const EntityRegistry&);
};

#endif /* defined(__noctisgames__EntityRegistry__) */
