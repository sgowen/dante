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
    
    static void sHandleEntityCreatedOnClient(Entity* inEntity);
    static void sHandleEntityDeletedOnClient(Entity* inEntity);
    static uint64_t sGetPlayerAddressHashForIndexOnClient(int inPlayerIndex);
    static Robot* sGetPlayerRobotForIDOnClient(int inPlayerID);
    
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
