//
//  EntityRegistry.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "EntityRegistry.h"

#include "World.h"
#include "RoboCat.h"

std::unique_ptr<EntityRegistry>	EntityRegistry::sInstance;

void EntityRegistry::StaticInit()
{
    sInstance.reset(new EntityRegistry());
}

EntityRegistry::EntityRegistry()
{
}

void EntityRegistry::RegisterCreationFunction(uint32_t inFourCCName, NWPhysicalEntityCreationFunc inCreationFunction)
{
    mNameToNWPhysicalEntityCreationFunctionMap[inFourCCName] = inCreationFunction;
}

NWPhysicalEntity* EntityRegistry::CreateNWPhysicalEntity(uint32_t inFourCCName)
{
    //no error checking- if the name isn't there, exception!
    NWPhysicalEntityCreationFunc creationFunc = mNameToNWPhysicalEntityCreationFunctionMap[inFourCCName];
    
    NWPhysicalEntity* entity = creationFunc();
    
    //should the registry depend on the world? this might be a little weird
    //perhaps you should ask the world to spawn things? for now it will be like this
    if (entity->getRTTI().derivesFrom(RoboCat::rtti))
    {
        World::sInstance->AddRoboCat((RoboCat*)entity);
    }
    
    return entity;
}
