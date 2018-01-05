//
//  EntityRegistry.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__EntityRegistry__
#define __noctisgames__EntityRegistry__

#include <map>

class Entity;
class EntityController;

typedef Entity* (*EntityCreationFunc)();
typedef EntityController* (*EntityControllerCreationFunc)();

class EntityRegistry
{
public:
    EntityRegistry();
    
    void registerFunction(uint32_t inFourCCName, EntityCreationFunc inCreationFunction);
    Entity* createEntity(uint32_t inFourCCName);
    
    void registerFunction(uint32_t inFourCCName, EntityControllerCreationFunc inCreationFunction);
    EntityController* createEntityController(uint32_t inFourCCName);
    
private:
    std::map<uint32_t, EntityCreationFunc> _nameToEntityCreationFunctionMap;
    std::map<uint32_t, EntityControllerCreationFunc> _nameToEntityControllerCreationFunctionMap;
};

#endif /* defined(__noctisgames__EntityRegistry__) */
