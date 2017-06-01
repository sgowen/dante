//
//  ReplicationManagerTransmissionData.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "ReplicationManagerTransmissionData.h"

#include "NetworkManagerServer.h"
#include "ReplicationAction.h"

void ReplicationManagerTransmissionData::AddTransmission(int inNetworkId, ReplicationAction inAction, uint32_t inState)
{
    /*
     //it would be silly if we already had a transmission for this network id in here...
     for (const auto& transmission: mTransmissions)
     {
     assert(inNetworkId != transmission.GetNetworkId());
     }
     */
    mTransmissions.emplace_back(inNetworkId, inAction, inState);
}

void ReplicationManagerTransmissionData::HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    //run through the transmissions
    for (const ReplicationTransmission& rt: mTransmissions)
    {
        //is it a create? then we have to redo the create.
        int networkId = rt.GetNetworkId();
        
        switch(rt.GetAction())
        {
            case RA_Create:
                HandleCreateDeliveryFailure(networkId);
                break;
            case RA_Update:
                HandleUpdateStateDeliveryFailure(networkId, rt.GetState(), inDeliveryNotificationManager);
                break;
            case RA_Destroy:
                HandleDestroyDeliveryFailure(networkId);
                break;
        }
    }
}

void ReplicationManagerTransmissionData::HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    //run through the transmissions, if any are Destroyed then we can remove this network id from the map
    for (const ReplicationTransmission& rt: mTransmissions)
    {
        switch(rt.GetAction())
        {
            case RA_Create:
                HandleCreateDeliverySuccess(rt.GetNetworkId());
                break;
            case RA_Destroy:
                HandleDestroyDeliverySuccess(rt.GetNetworkId());
                break;
            case RA_Update:
                break;
        }
    }
}

void ReplicationManagerTransmissionData::HandleCreateDeliveryFailure(int inNetworkId) const
{
    //does the object still exist? it might be dead, in which case we don't resend a create
    GameObject* gameObject = NetworkManagerServer::getInstance()->GetGameObject(inNetworkId);
    if (gameObject)
    {
        mReplicationManagerServer->ReplicateCreate(inNetworkId, gameObject->getAllStateMask());
    }
}

void ReplicationManagerTransmissionData::HandleDestroyDeliveryFailure(int inNetworkId) const
{
    mReplicationManagerServer->ReplicateDestroy(inNetworkId);
}

void ReplicationManagerTransmissionData::HandleUpdateStateDeliveryFailure(int inNetworkId, uint32_t inState, DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    //does the object still exist? it might be dead, in which case we don't resend an update
    if (NetworkManagerServer::getInstance()->GetGameObject(inNetworkId))
    {
        //look in all future in flight packets, in all transmissions
        //remove written state from dirty state
        for (const auto& inFlightPacket: inDeliveryNotificationManager->GetInFlightPackets())
        {
            ReplicationManagerTransmissionData* rmtdp = static_cast<ReplicationManagerTransmissionData*>(inFlightPacket.GetTransmissionData('RPLM'));
            
            for (const ReplicationTransmission& otherRT: rmtdp->mTransmissions)
            {
                inState &= ~otherRT.GetState();
            }
        }
        
        //if there's still any dirty state, mark it
        if (inState)
        {
            mReplicationManagerServer->SetStateDirty(inNetworkId, inState);
        }
    }
}

void ReplicationManagerTransmissionData::HandleCreateDeliverySuccess(int inNetworkId) const
{
    //we've received an ack for the create, so we can start sending as only an update
    mReplicationManagerServer->HandleCreateAckd(inNetworkId);
}

void ReplicationManagerTransmissionData::HandleDestroyDeliverySuccess(int inNetworkId) const
{
    mReplicationManagerServer->RemoveFromReplication(inNetworkId);
}
