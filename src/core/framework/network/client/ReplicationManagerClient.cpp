//
//  ReplicationManagerClient.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "ReplicationManagerClient.h"

#include "InputMemoryBitStream.h"

#include "NetworkManagerClient.h"
#include "Entity.h"
#include "ReplicationAction.h"
#include "EntityRegistry.h"

#include <cassert>

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
                ReadAndDoCreateAction(inInputStream, networkId);
                break;
            case RA_Update:
                ReadAndDoUpdateAction(inInputStream, networkId);
                break;
            case RA_Destroy:
                ReadAndDoDestroyAction(inInputStream, networkId);
                break;
        }
    }
}

void ReplicationManagerClient::ReadAndDoCreateAction(InputMemoryBitStream& inInputStream, int inNetworkId)
{
    //need 4 cc
    uint32_t fourCCName;
    inInputStream.read(fourCCName);
    
    //we might already have this object- could happen if our ack of the create got dropped so server resends create request
    //(even though we might have created)
    Entity* gameObject = NetworkManagerClient::getInstance()->getEntity(inNetworkId);
    if (!gameObject)
    {
        //create the object and map it...
        gameObject = EntityRegistry::getInstance()->createEntity(fourCCName);
        gameObject->setID(inNetworkId);
        
        NetworkManagerClient::getInstance()->addToNetworkIdToEntityMap(gameObject);
        
        //it had really be the rigth type...
        assert(gameObject->getNetworkType() == fourCCName);
    }
    
    //and read state
    gameObject->read(inInputStream);
}

void ReplicationManagerClient::ReadAndDoUpdateAction(InputMemoryBitStream& inInputStream, int inNetworkId)
{
    //need object
    Entity* gameObject = NetworkManagerClient::getInstance()->getEntity(inNetworkId);
    
    //gameObject MUST be found, because create was ack'd if we're getting an update...
    //and read state
    gameObject->read(inInputStream);
}

void ReplicationManagerClient::ReadAndDoDestroyAction(InputMemoryBitStream& inInputStream, int inNetworkId)
{
    //if something was destroyed before the create went through, we'll never get it
    //but we might get the destroy request, so be tolerant of being asked to destroy something that wasn't created
    Entity* gameObject = NetworkManagerClient::getInstance()->getEntity(inNetworkId);
    if (gameObject)
    {
        gameObject->requestDeletion();
        NetworkManagerClient::getInstance()->removeFromNetworkIdToEntityMap(gameObject);
    }
}
