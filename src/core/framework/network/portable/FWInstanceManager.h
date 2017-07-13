//
//  FWInstanceManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__FWInstanceManager__
#define __noctisgames__FWInstanceManager__

#include "EntityManager.h"

class EntityRegistry;

#define SERVER_ENTITY_REGISTRY FWInstanceManager::getServerEntityRegistry()

class FWInstanceManager
{
public:
    static void createClientEntityManager(HandleEntityCreatedFunc handleEntityCreatedFunc, HandleEntityDeletionFunc handleEntityDeletionFunc);
    static void createServerEntityManager(HandleEntityCreatedFunc handleEntityCreatedFunc, HandleEntityDeletionFunc handleEntityDeletionFunc);
    static void destroyClientEntityManager();
    static void destroyServerEntityManager();
    static EntityManager* getClientEntityManager();
    static EntityManager* getServerEntityManager();
    
    static EntityRegistry* getClientEntityRegistry();
    static EntityRegistry* getServerEntityRegistry();
    
private:
    static EntityManager* s_clientEntityManagerInstance;
    static EntityManager* s_serverEntityManagerInstance;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    FWInstanceManager();
    ~FWInstanceManager();
    FWInstanceManager(const FWInstanceManager&);
    FWInstanceManager& operator=(const FWInstanceManager&);
};

#endif /* defined(__noctisgames__FWInstanceManager__) */
