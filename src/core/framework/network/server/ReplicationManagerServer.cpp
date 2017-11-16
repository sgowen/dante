//
//  ReplicationManagerServer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "ReplicationManagerServer.h"

#include "OutputMemoryBitStream.h"
#include "ReplicationManagerTransmissionData.h"

#include "FWInstanceManager.h"
#include "EntityManager.h"
#include "ReplicationAction.h"
#include "macros.h"
#include "Entity.h"

void ReplicationManagerServer::replicateCreate(uint32_t inNetworkId, uint32_t inInitialDirtyState)
{
    m_networkIdToReplicationCommand[inNetworkId] = ReplicationCommand(inInitialDirtyState);
}

void ReplicationManagerServer::replicateDestroy(uint32_t inNetworkId)
{
    //it's broken if we don't find it...
    m_networkIdToReplicationCommand[inNetworkId].setDestroy();
}

void ReplicationManagerServer::removeFromReplication(uint32_t inNetworkId)
{
    m_networkIdToReplicationCommand.erase(inNetworkId);
}

void ReplicationManagerServer::setStateDirty(uint32_t inNetworkId, uint32_t inDirtyState)
{
    m_networkIdToReplicationCommand[inNetworkId].addDirtyState(inDirtyState);
}

void ReplicationManagerServer::handleCreateAckd(uint32_t inNetworkId)
{
    m_networkIdToReplicationCommand[inNetworkId].handleCreateAckd();
}

void ReplicationManagerServer::write(OutputMemoryBitStream& inOutputStream, ReplicationManagerTransmissionData* ioTransmissinData)
{
    //run through each replication command and do something...
    for (auto& pair: m_networkIdToReplicationCommand)
    {
        ReplicationCommand& replicationCommand = pair.second;
        if (replicationCommand.hasDirtyState())
        {
            int networkId = pair.first;
            
            //well, first write the network id...
            inOutputStream.write(networkId);
            
            //only need 2 bits for action...
            ReplicationAction action = replicationCommand.getAction();
            inOutputStream.write<u_int8_t, 2>(static_cast<u_int8_t>(action));
            
            uint32_t writtenState = 0;
            uint32_t dirtyState = replicationCommand.getDirtyState();
            
            //now do what?
            switch(action)
            {
                case RA_Create:
                    writtenState = writeCreateAction(inOutputStream, networkId, dirtyState);
                    break;
                case RA_Update:
                    writtenState = writeUpdateAction(inOutputStream, networkId, dirtyState);
                    break;
                case RA_Destroy:
                    //don't need anything other than state!
                    writtenState = writeDestroyAction(inOutputStream, networkId, dirtyState);
                    break;
            }
            
            ioTransmissinData->addTransmission(networkId, action, writtenState);
            
            //let's pretend everything was written- don't make this too hard
            replicationCommand.clearDirtyState(writtenState);
        }
    }
}

uint32_t ReplicationManagerServer::writeCreateAction(OutputMemoryBitStream& inOutputStream, uint32_t inNetworkId, uint32_t inDirtyState)
{
    //need object
    Entity* entity = FWInstanceManager::getServerEntityManager()->getEntityByID(inNetworkId);
    //need 4 cc
    inOutputStream.write(entity->getNetworkType());
    
    return entity->write(inOutputStream, inDirtyState);
}

uint32_t ReplicationManagerServer::writeUpdateAction(OutputMemoryBitStream& inOutputStream, uint32_t inNetworkId, uint32_t inDirtyState)
{
    //need object
    Entity* entity = FWInstanceManager::getServerEntityManager()->getEntityByID(inNetworkId);
    
    //if we can't find the entity on the other side, we won't be able to read the written data (since we won't know which class wrote it)
    //so we need to know how many bytes to skip.
    
    //this means we need byte sand each new object needs to be byte aligned
    
    uint32_t writtenState = entity->write(inOutputStream, inDirtyState);
    
    return writtenState;
}

uint32_t ReplicationManagerServer::writeDestroyAction(OutputMemoryBitStream& inOutputStream, uint32_t inNetworkId, uint32_t inDirtyState)
{
    UNUSED(inOutputStream);
    UNUSED(inNetworkId);
    UNUSED(inDirtyState);
    
    //don't have to do anything- action already written
    
    return inDirtyState;
}
