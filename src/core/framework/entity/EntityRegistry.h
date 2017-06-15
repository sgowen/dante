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

#include "Network.h"

#include <unordered_map>

typedef void (*HandleEntityCreatedFunc)(Entity* inEntity);
typedef Entity* (*EntityCreationFunc)();

class EntityRegistry
{
public:
    EntityRegistry();
    
    void init(HandleEntityCreatedFunc handleEntityCreatedFunc);
    
    void registerCreationFunction(uint32_t inFourCCName, EntityCreationFunc inCreationFunction);
    
    Entity* createEntity(uint32_t inFourCCName);
    
private:
    std::unordered_map<uint32_t, EntityCreationFunc> m_nameToEntityCreationFunctionMap;
    HandleEntityCreatedFunc m_handleEntityCreatedFunc;
};

#endif /* defined(__noctisgames__EntityRegistry__) */
