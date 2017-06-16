//
//  FWInstanceManager.cpp
//  dante
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "FWInstanceManager.h"

#include "EntityManager.h"

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
