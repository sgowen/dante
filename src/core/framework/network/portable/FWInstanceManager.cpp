//
//  FWInstanceManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/network/portable/FWInstanceManager.h"

#include "framework/entity/EntityRegistry.h"

#include <assert.h>

EntityManager* FWInstanceManager::s_clientEntityManagerInstance = NULL;
EntityManager* FWInstanceManager::s_serverEntityManagerInstance = NULL;

void FWInstanceManager::createClientEntityManager(HandleEntityCreatedFunc handleEntityCreatedFunc, HandleEntityDeletionFunc handleEntityDeletionFunc)
{
    assert(!s_clientEntityManagerInstance);
    
    s_clientEntityManagerInstance = new EntityManager(handleEntityCreatedFunc, handleEntityDeletionFunc);
}

void FWInstanceManager::createServerEntityManager(HandleEntityCreatedFunc handleEntityCreatedFunc, HandleEntityDeletionFunc handleEntityDeletionFunc)
{
    assert(!s_serverEntityManagerInstance);
    
    s_serverEntityManagerInstance = new EntityManager(handleEntityCreatedFunc, handleEntityDeletionFunc);
}

void FWInstanceManager::destroyClientEntityManager()
{
    assert(s_clientEntityManagerInstance);
    
    delete s_clientEntityManagerInstance;
    s_clientEntityManagerInstance = NULL;
}

void FWInstanceManager::destroyServerEntityManager()
{
    assert(s_serverEntityManagerInstance);
    
    delete s_serverEntityManagerInstance;
    s_serverEntityManagerInstance = NULL;
}

EntityManager* FWInstanceManager::getClientEntityManager()
{
    return s_clientEntityManagerInstance;
}

EntityManager* FWInstanceManager::getServerEntityManager()
{
    return s_serverEntityManagerInstance;
}

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

FWInstanceManager::FWInstanceManager()
{
    // Empty
}

FWInstanceManager::~FWInstanceManager()
{
    // Empty
}
