//
//  EntityRegistry.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "EntityRegistry.h"

EntityRegistry::EntityRegistry()
{
    // Empty
}

void EntityRegistry::registerCreationFunction(uint32_t inFourCCName, EntityCreationFunc inCreationFunction)
{
    m_nameToEntityCreationFunctionMap[inFourCCName] = inCreationFunction;
}

Entity* EntityRegistry::createEntity(uint32_t inFourCCName)
{
    EntityCreationFunc creationFunc = m_nameToEntityCreationFunctionMap[inFourCCName];
    
    return creationFunc();
}
