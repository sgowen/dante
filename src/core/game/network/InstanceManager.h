//
//  InstanceManager.h
//  dante
//
//  Created by Stephen Gowen on 6/14/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InstanceManager__
#define __noctisgames__InstanceManager__

class Entity;
class World;
class Robot;

class InstanceManager
{
public:
    static void createClientWorld();
    static void createServerWorld();
    static void destroyClientWorld();
    static void destroyServerWorld();
    static World* getClientWorld();
    static World* getServerWorld();
    
    static void staticHandleEntityCreatedOnClient(Entity* inEntity);
    static void staticHandleEntityDeletedOnClient(Entity* inEntity);
    static uint64_t staticGetPlayerAddressHashForIndexOnClient(int inPlayerIndex);
    static Robot* staticGetPlayerRobotForIDOnClient(int inPlayerID);
    
    static void staticHandleEntityCreatedOnServer(Entity* inEntity);
    static void staticHandleEntityDeletedOnServer(Entity* inEntity);
    
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
