//
//  ReplicationManagerClient.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/client/ReplicationManagerClient.h>

#include <framework/entity/EntityManager.h>
#include <framework/network/portable/InputMemoryBitStream.h>

#include <framework/entity/EntityMapper.h>
#include <framework/entity/Entity.h>
#include <framework/entity/EntityLayoutMapper.h>
#include <framework/network/portable/ReplicationAction.h>
#include <framework/network/portable/MemoryBitStreamUtil.h>

#include <cassert>

ReplicationManagerClient::ReplicationManagerClient(EntityManager* entityManager) : _entityManager(entityManager)
{
    // Empty
}

void ReplicationManagerClient::read(InputMemoryBitStream& inInputStream)
{
    while (inInputStream.getRemainingBitCount() >= 34)
    {
        //read the network id...
        uint32_t networkId;
        inInputStream.read(networkId);
        
        //only need 2 bits for action...
        uint8_t action;
        inInputStream.read<uint8_t, 2>(action);
        
        switch(action)
        {
            case RA_Create:
                readAndDoCreateAction(inInputStream, networkId);
                break;
            case RA_Update:
                readAndDoUpdateAction(inInputStream, networkId);
                break;
            case RA_Destroy:
                readAndDoDestroyAction(inInputStream, networkId);
                break;
        }
    }
}

void ReplicationManagerClient::readAndDoCreateAction(InputMemoryBitStream& inInputStream, uint32_t inNetworkId)
{
    uint32_t fourCCName;
    inInputStream.read(fourCCName);
    
    //we might already have this object- could happen if our ack of the create got dropped so server resends create request
    //(even though we might have created)
    Entity* entity = _entityManager->getEntityByID(inNetworkId);
    if (!entity)
    {
        //create the object and map it...
        EntityInstanceDef eid(inNetworkId, fourCCName);
        entity = EntityMapper::getInstance()->createEntity(&eid, false);
        
        _entityManager->registerEntity(entity);
        
        //it had really be the rigth type...
        assert(entity->getEntityDef().type == fourCCName);
    }
    
    MemoryBitStreamUtil::read(inInputStream, *entity);
}

void ReplicationManagerClient::readAndDoUpdateAction(InputMemoryBitStream& inInputStream, uint32_t inNetworkId)
{
    //need object
    Entity* entity = _entityManager->getEntityByID(inNetworkId);
    
    //entity MUST be found, because create was ack'd if we're getting an update...
    //and read state
    
    if (entity)
    {
        MemoryBitStreamUtil::read(inInputStream, *entity);
    }
}

void ReplicationManagerClient::readAndDoDestroyAction(InputMemoryBitStream& inInputStream, uint32_t inNetworkId)
{
    //if something was destroyed before the create went through, we'll never get it
    //but we might get the destroy request, so be tolerant of being asked to destroy something that wasn't created
    Entity* entity = _entityManager->getEntityByID(inNetworkId);
    if (entity)
    {
        _entityManager->deregisterEntity(entity);
    }
}
