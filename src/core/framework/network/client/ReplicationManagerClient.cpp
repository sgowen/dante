//
//  ReplicationManagerClient.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "ReplicationManagerClient.h"

#include "EntityRegistry.h"
#include "InputMemoryBitStream.h"

#include "FWInstanceManager.h"
#include "EntityManager.h"
#include "Entity.h"
#include "ReplicationAction.h"

#include <cassert>

ReplicationManagerClient::ReplicationManagerClient()
{
    // Empty
}

void ReplicationManagerClient::read(InputMemoryBitStream& inInputStream)
{
    while (inInputStream.getRemainingBitCount() >= 32)
    {
        //read the network id...
        int networkId; inInputStream.read(networkId);
        
        //only need 2 bits for action...
        uint8_t action; inInputStream.read(action, 2);
        
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

void ReplicationManagerClient::readAndDoCreateAction(InputMemoryBitStream& inInputStream, int inNetworkId)
{
    //need 4 cc
    uint32_t fourCCName;
    inInputStream.read(fourCCName);
    
    //we might already have this object- could happen if our ack of the create got dropped so server resends create request
    //(even though we might have created)
    Entity* gameObject = FWInstanceManager::getClientEntityManager()->getEntityByID(inNetworkId);
    if (!gameObject)
    {
        //create the object and map it...
        gameObject = FWInstanceManager::getClientEntityRegistry()->createEntity(fourCCName);
        gameObject->setID(inNetworkId);
        
        FWInstanceManager::getClientEntityManager()->registerEntity(gameObject);
        
        //it had really be the rigth type...
        assert(gameObject->getNetworkType() == fourCCName);
    }
    
    //and read state
    gameObject->read(inInputStream);
}

void ReplicationManagerClient::readAndDoUpdateAction(InputMemoryBitStream& inInputStream, int inNetworkId)
{
    //need object
    Entity* gameObject = FWInstanceManager::getClientEntityManager()->getEntityByID(inNetworkId);
    
    //gameObject MUST be found, because create was ack'd if we're getting an update...
    //and read state
    gameObject->read(inInputStream);
}

void ReplicationManagerClient::readAndDoDestroyAction(InputMemoryBitStream& inInputStream, int inNetworkId)
{
    //if something was destroyed before the create went through, we'll never get it
    //but we might get the destroy request, so be tolerant of being asked to destroy something that wasn't created
    Entity* gameObject = FWInstanceManager::getClientEntityManager()->getEntityByID(inNetworkId);
    if (gameObject)
    {
        gameObject->requestDeletion();
        gameObject->onDeletion();
        FWInstanceManager::getClientEntityManager()->removeEntity(gameObject);
    }
}
