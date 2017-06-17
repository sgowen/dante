//
//  FWInstanceManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "FWInstanceManager.h"

#include "EntityRegistry.h"
#include "EntityManager.h"

EntityRegistry* FWInstanceManager::getClientEntityRegistry()
{
    static EntityRegistry instance = EntityRegistry();
    return &instance;
}

EntityRegistry* FWInstanceManager::getServerEntityRegistry()
{
    static EntityRegistry instance = EntityRegistry();
    return &instance;
}

EntityManager* FWInstanceManager::getClientEntityManager()
{
    static EntityManager instance = EntityManager();
    return &instance;
}

EntityManager* FWInstanceManager::getServerEntityManager()
{
    static EntityManager instance = EntityManager();
    return &instance;
}

FWInstanceManager::FWInstanceManager()
{
    // Empty
}

FWInstanceManager::~FWInstanceManager()
{
    // Empty
}
