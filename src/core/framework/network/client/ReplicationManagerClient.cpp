//
//  ReplicationManagerClient.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/network/client/ReplicationManagerClient.h"

#include "framework/network/portable/InputMemoryBitStream.h"

#include "framework/network/portable/FWInstanceManager.h"
#include "framework/entity/EntityManager.h"
#include "framework/entity/Entity.h"
#include "framework/network/portable/ReplicationAction.h"

#include <cassert>

ReplicationManagerClient::ReplicationManagerClient()
{
    // Empty
}

void ReplicationManagerClient::read(InputMemoryBitStream& inInputStream)
{
    while (inInputStream.getRemainingBitCount() > 32)
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
    //need 4 cc
    uint32_t fourCCName;
    inInputStream.read(fourCCName);
    
    //we might already have this object- could happen if our ack of the create got dropped so server resends create request
    //(even though we might have created)
    Entity* entity = CLIENT_ENTITY_MGR->getEntityByID(inNetworkId);
    if (!entity)
    {
        //create the object and map it...
        entity = CLIENT_ENTITY_MGR->createEntity(fourCCName);
        entity->setID(inNetworkId);
        
        CLIENT_ENTITY_MGR->registerEntity(entity);
        
        //it had really be the rigth type...
        assert(entity->getEntityDef().type == fourCCName);
    }
    
    //and read state
    entity->read(inInputStream);
}

void ReplicationManagerClient::readAndDoUpdateAction(InputMemoryBitStream& inInputStream, uint32_t inNetworkId)
{
    //need object
    Entity* entity = CLIENT_ENTITY_MGR->getEntityByID(inNetworkId);
    
    //entity MUST be found, because create was ack'd if we're getting an update...
    //and read state
    
    if (entity)
    {
        entity->read(inInputStream);
    }
}

void ReplicationManagerClient::readAndDoDestroyAction(InputMemoryBitStream& inInputStream, uint32_t inNetworkId)
{
    //if something was destroyed before the create went through, we'll never get it
    //but we might get the destroy request, so be tolerant of being asked to destroy something that wasn't created
    Entity* entity = CLIENT_ENTITY_MGR->getEntityByID(inNetworkId);
    if (entity)
    {
        CLIENT_ENTITY_MGR->deregisterEntity(entity);
    }
}
