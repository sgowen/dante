//
//  InstanceManager.cpp
//  dante
//
//  Created by Stephen Gowen on 6/14/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/logic/InstanceManager.h>

#include <framework/entity/Entity.h>
#include <game/logic/World.h>
#include <game/entity/PlayerController.h>

#include <assert.h>

World* InstanceManager::s_clientWorldInstance = NULL;
World* InstanceManager::s_serverWorldInstance = NULL;

void InstanceManager::setClientWorld(World* inValue)
{
    s_clientWorldInstance = inValue;
}

void InstanceManager::setServerWorld(World* inValue)
{
    s_serverWorldInstance = inValue;
}

World* InstanceManager::getClientWorld()
{
    return s_clientWorldInstance;
}

World* InstanceManager::getServerWorld()
{
    return s_serverWorldInstance;
}

void InstanceManager::sHandleDynamicEntityCreatedOnClient(Entity* inEntity)
{
    if (s_clientWorldInstance)
    {
        s_clientWorldInstance->addDynamicEntity(inEntity);
    }
}

void InstanceManager::sHandleDynamicEntityDeletedOnClient(Entity* inEntity)
{
    if (s_clientWorldInstance)
    {
        s_clientWorldInstance->removeDynamicEntity(inEntity);
    }
}

uint64_t InstanceManager::sGetPlayerAddressHashForIndexOnClient(uint8_t inPlayerIndex)
{
    uint64_t ret = 0;
 
    if (s_clientWorldInstance)
    {
        Entity* entity = s_clientWorldInstance->getPlayerWithId(inPlayerIndex + 1);
        assert(entity);
        
        PlayerController* robot = static_cast<PlayerController*>(entity->getController());
        assert(robot);
        ret = robot->getAddressHash();
    }
    
    return ret;
}

void InstanceManager::sHandleDynamicEntityCreatedOnServer(Entity* inEntity)
{
    if (s_serverWorldInstance)
    {
        s_serverWorldInstance->addDynamicEntity(inEntity);
    }
}

void InstanceManager::sHandleDynamicEntityDeletedOnServer(Entity* inEntity)
{
    if (s_serverWorldInstance)
    {
        s_serverWorldInstance->removeDynamicEntity(inEntity);
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
