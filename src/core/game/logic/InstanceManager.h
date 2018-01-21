//
//  InstanceManager.h
//  dante
//
//  Created by Stephen Gowen on 6/14/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InstanceManager__
#define __noctisgames__InstanceManager__

#include <cstdint>

class Entity;
class World;

class InstanceManager
{
public:
    static void setClientWorld(World* inValue);
    static void setServerWorld(World* inValue);
    static World* getClientWorld();
    static World* getServerWorld();
    
    static void sHandleEntityCreatedOnClient(Entity* inEntity);
    static void sHandleEntityDeletedOnClient(Entity* inEntity);
    static uint64_t sGetPlayerAddressHashForIndexOnClient(uint8_t inPlayerIndex);
    static Entity* sGetPlayerEntityForIDOnClient(uint8_t inPlayerID);
    
    static void sHandleEntityCreatedOnServer(Entity* inEntity);
    static void sHandleEntityDeletedOnServer(Entity* inEntity);
    
private:
    static World* s_clientWorldInstance;
    static World* s_serverWorldInstance;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    InstanceManager();
    ~InstanceManager();
    InstanceManager(const InstanceManager&);
    InstanceManager& operator=(const InstanceManager&);
};

#endif /* defined(__noctisgames__InstanceManager__) */
