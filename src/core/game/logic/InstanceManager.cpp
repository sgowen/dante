//
//  InstanceManager.cpp
//  dante
//
//  Created by Stephen Gowen on 6/14/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/InstanceManager.h"

#include "framework/entity/Entity.h"
#include "game/logic/World.h"
#include <game/logic/PlayerController.h>

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
 
    Entity* entity = InstanceManager::sGetPlayerEntityForIDOnClient(inPlayerIndex + 1);
    if (entity)
    {
        PlayerController* robot = static_cast<PlayerController*>(entity->getController());
        assert(robot);
        ret = robot->getAddressHash();
    }
    
    return ret;
}

Entity* InstanceManager::sGetPlayerEntityForIDOnClient(uint8_t inPlayerID)
{
    Entity* ret = NULL;
    
    if (InstanceManager::getClientWorld())
    {
        ret = InstanceManager::getClientWorld()->getPlayerWithId(inPlayerID);
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
