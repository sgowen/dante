//
//  InstanceManager.cpp
//  dante
//
//  Created by Stephen Gowen on 6/14/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "InstanceManager.h"

#include "Entity.h"
#include "World.h"
#include "Robot.h"

#include <assert.h>

World* InstanceManager::s_clientWorldInstance = nullptr;
World* InstanceManager::s_serverWorldInstance = nullptr;

void InstanceManager::createClientWorld()
{
    assert(!s_clientWorldInstance);
    
    s_clientWorldInstance = new World();
}

void InstanceManager::createServerWorld()
{
    assert(!s_serverWorldInstance);
    
    s_serverWorldInstance = new World();
}

void InstanceManager::destroyClientWorld()
{
    assert(s_clientWorldInstance);
    
    delete s_clientWorldInstance;
    s_clientWorldInstance = nullptr;
}

void InstanceManager::destroyServerWorld()
{
    assert(s_serverWorldInstance);
    
    delete s_serverWorldInstance;
    s_serverWorldInstance = nullptr;
}

World* InstanceManager::getClientWorld()
{
    return s_clientWorldInstance;
}

World* InstanceManager::getServerWorld()
{
    return s_serverWorldInstance;
}

void InstanceManager::staticHandleEntityCreatedOnClient(Entity* inEntity)
{
    if (InstanceManager::getClientWorld())
    {
        InstanceManager::getClientWorld()->addEntity(inEntity);
    }
}

void InstanceManager::staticHandleEntityDeletedOnClient(Entity* inEntity)
{
    if (InstanceManager::getClientWorld())
    {
        InstanceManager::getClientWorld()->removeEntity(inEntity);
    }
}

uint64_t InstanceManager::staticGetPlayerAddressHashForIndexOnClient(int inPlayerIndex)
{
    uint64_t ret = 0;
 
    Robot* robot = InstanceManager::staticGetPlayerRobotForIDOnClient(inPlayerIndex + 1);
    if (robot)
    {
        ret = robot->getAddressHash();
    }
    
    return ret;
}

Robot* InstanceManager::staticGetPlayerRobotForIDOnClient(int inPlayerID)
{
    Robot* ret = nullptr;
    
    if (InstanceManager::getClientWorld())
    {
        ret = InstanceManager::getClientWorld()->getRobotWithPlayerId(inPlayerID);
    }
    
    return ret;
}

void InstanceManager::staticHandleEntityCreatedOnServer(Entity* inEntity)
{
    if (InstanceManager::getServerWorld())
    {
        InstanceManager::getServerWorld()->addEntity(inEntity);
    }
}

void InstanceManager::staticHandleEntityDeletedOnServer(Entity* inEntity)
{
    if (InstanceManager::getServerWorld())
    {
        InstanceManager::getServerWorld()->removeEntity(inEntity);
    }
}

InstanceManager::InstanceManager()
{
    // Empty
}

InstanceManager::~InstanceManager()
{
    // Empty
}
