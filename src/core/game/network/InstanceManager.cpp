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
    
    s_clientWorldInstance = new World(false);
}

void InstanceManager::createServerWorld()
{
    assert(!s_serverWorldInstance);
    
    s_serverWorldInstance = new World(true);
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

void InstanceManager::sHandleEntityCreatedOnClient(Entity* inEntity)
{
    if (InstanceManager::getClientWorld())
    {
        InstanceManager::getClientWorld()->addEntity(inEntity);
    }
}

void InstanceManager::sHandleEntityDeletedOnClient(Entity* inEntity)
{
    if (InstanceManager::getClientWorld())
    {
        InstanceManager::getClientWorld()->removeEntity(inEntity);
    }
}

uint64_t InstanceManager::sGetPlayerAddressHashForIndexOnClient(uint8_t inPlayerIndex)
{
    uint64_t ret = 0;
 
    Robot* robot = InstanceManager::sGetPlayerRobotForIDOnClient(inPlayerIndex + 1);
    if (robot)
    {
        ret = robot->getAddressHash();
    }
    
    return ret;
}

Robot* InstanceManager::sGetPlayerRobotForIDOnClient(uint8_t inPlayerID)
{
    Robot* ret = nullptr;
    
    if (InstanceManager::getClientWorld())
    {
        ret = InstanceManager::getClientWorld()->getRobotWithPlayerId(inPlayerID);
    }
    
    return ret;
}

void InstanceManager::sHandleEntityCreatedOnServer(Entity* inEntity)
{
    if (InstanceManager::getServerWorld())
    {
        InstanceManager::getServerWorld()->addEntity(inEntity);
    }
}

void InstanceManager::sHandleEntityDeletedOnServer(Entity* inEntity)
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
