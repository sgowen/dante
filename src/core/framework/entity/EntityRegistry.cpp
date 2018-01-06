//
//  EntityRegistry.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/entity/EntityRegistry.h"

#include "framework/entity/Entity.h"

EntityRegistry::EntityRegistry()
{
    // Empty
}

void EntityRegistry::registerFunction(uint32_t inFourCCName, EntityCreationFunc inCreationFunction)
{
    _nameToEntityCreationFunctionMap[inFourCCName] = inCreationFunction;
}

Entity* EntityRegistry::createEntity(uint32_t inFourCCName)
{
    EntityCreationFunc creationFunc = _nameToEntityCreationFunctionMap[inFourCCName];
    
    return creationFunc();
}
