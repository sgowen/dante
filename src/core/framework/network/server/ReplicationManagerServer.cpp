//
//  ReplicationManagerServer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "ReplicationManagerServer.h"

#include "MemoryBitStream.h"
#include "ReplicationManagerTransmissionData.h"

#include "ReplicationManagerTransmissionData.h"
#include "NetworkManagerServer.h"
#include "ReplicationAction.h"

void ReplicationManagerServer::ReplicateCreate(int inNetworkId, uint32_t inInitialDirtyState)
{
    m_iNetworkIdToReplicationCommand[inNetworkId] = ReplicationCommand(inInitialDirtyState);
}

void ReplicationManagerServer::ReplicateDestroy(int inNetworkId)
{
    //it's broken if we don't find it...
    m_iNetworkIdToReplicationCommand[inNetworkId].SetDestroy();
}

void ReplicationManagerServer::RemoveFromReplication(int inNetworkId)
{
    m_iNetworkIdToReplicationCommand.erase(inNetworkId);
}

void ReplicationManagerServer::SetStateDirty(int inNetworkId, uint32_t inDirtyState)
{
    m_iNetworkIdToReplicationCommand[inNetworkId].AddDirtyState(inDirtyState);
}

void ReplicationManagerServer::HandleCreateAckd(int inNetworkId)
{
    m_iNetworkIdToReplicationCommand[inNetworkId].HandleCreateAckd();
}

void ReplicationManagerServer::Write(OutputMemoryBitStream& inOutputStream, ReplicationManagerTransmissionData* ioTransmissinData)
{
    //run through each replication command and do something...
    for (auto& pair: m_iNetworkIdToReplicationCommand)
    {
        ReplicationCommand& replicationCommand = pair.second;
        if (replicationCommand.HasDirtyState())
        {
            int networkId = pair.first;
            
            //well, first write the network id...
            inOutputStream.Write(networkId);
            
            //only need 2 bits for action...
            ReplicationAction action = replicationCommand.GetAction();
            inOutputStream.Write(action, 2);
            
            uint32_t writtenState = 0;
            uint32_t dirtyState = replicationCommand.GetDirtyState();
            
            //now do what?
            switch(action)
            {
                case RA_Create:
                    writtenState = WriteCreateAction(inOutputStream, networkId, dirtyState);
                    break;
                case RA_Update:
                    writtenState = WriteUpdateAction(inOutputStream, networkId, dirtyState);
                    break;
                case RA_Destroy:
                    //don't need anything other than state!
                    writtenState = WriteDestroyAction(inOutputStream, networkId, dirtyState);
                    break;
            }
            
            ioTransmissinData->AddTransmission(networkId, action, writtenState);
            
            //let's pretend everything was written- don't make this too hard
            replicationCommand.ClearDirtyState(writtenState);
        }
    }
}

uint32_t ReplicationManagerServer::WriteCreateAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint32_t inDirtyState)
{
    //need object
    GameObject* gameObject = NetworkManagerServer::getInstance()->GetGameObject(inNetworkId);
    //need 4 cc
    inOutputStream.Write(gameObject->getNetworkType());
    return gameObject->Write(inOutputStream, inDirtyState);
}

uint32_t ReplicationManagerServer::WriteUpdateAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint32_t inDirtyState)
{
    //need object
    GameObject* gameObject = NetworkManagerServer::getInstance()->GetGameObject(inNetworkId);
    
    //if we can't find the gameObject on the other side, we won't be able to read the written data (since we won't know which class wrote it)
    //so we need to know how many bytes to skip.
    
    //this means we need byte sand each new object needs to be byte aligned
    
    uint32_t writtenState = gameObject->Write(inOutputStream, inDirtyState);
    
    return writtenState;
}

uint32_t ReplicationManagerServer::WriteDestroyAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint32_t inDirtyState)
{
    (void)inOutputStream;
    (void) inNetworkId;
    (void) inDirtyState;
    //don't have to do anything- action already written
    
    return inDirtyState;
}
