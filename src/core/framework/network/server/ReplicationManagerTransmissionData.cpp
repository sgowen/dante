//
//  ReplicationManagerTransmissionData.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "ReplicationManagerTransmissionData.h"

#include "ReplicationManagerServer.h"

#include "FWInstanceManager.h"
#include "EntityManager.h"
#include "Entity.h"
#include "ReplicationAction.h"
#include "DeliveryNotificationManager.h"

ReplicationManagerTransmissionData::ReplicationManagerTransmissionData(ReplicationManagerServer* inReplicationManagerServer) :
m_replicationManagerServer(inReplicationManagerServer)
{
    // Empty
}

void ReplicationManagerTransmissionData::addTransmission(uint32_t inNetworkId, ReplicationAction inAction, uint32_t inState)
{
    /*
     //it would be silly if we already had a transmission for this network id in here...
     for (const auto& transmission: m_transmissions)
     {
     assert(inNetworkId != transmission.getID());
     }
     */
    m_transmissions.push_back(ReplicationTransmission(inNetworkId, inAction, inState));
}

void ReplicationManagerTransmissionData::handleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    //run through the transmissions
    for (const ReplicationTransmission& rt: m_transmissions)
    {
        //is it a create? then we have to redo the create.
        uint32_t networkId = rt.getID();
        
        switch(rt.getAction())
        {
            case RA_Create:
                handleCreateDeliveryFailure(networkId);
                break;
            case RA_Update:
                handleUpdateStateDeliveryFailure(networkId, rt.getState(), inDeliveryNotificationManager);
                break;
            case RA_Destroy:
                handleDestroyDeliveryFailure(networkId);
                break;
        }
    }
}

void ReplicationManagerTransmissionData::handleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    //run through the transmissions, if any are Destroyed then we can remove this network id from the map
    for (const ReplicationTransmission& rt: m_transmissions)
    {
        switch(rt.getAction())
        {
            case RA_Create:
                handleCreateDeliverySuccess(rt.getID());
                break;
            case RA_Destroy:
                handleDestroyDeliverySuccess(rt.getID());
                break;
            case RA_Update:
                break;
        }
    }
}

void ReplicationManagerTransmissionData::handleCreateDeliveryFailure(uint32_t inNetworkId) const
{
    //does the object still exist? it might be dead, in which case we don't resend a create
    Entity* entity = FWInstanceManager::getServerEntityManager()->getEntityByID(inNetworkId);
    if (entity)
    {
        m_replicationManagerServer->replicateCreate(inNetworkId, entity->getAllStateMask());
    }
}

void ReplicationManagerTransmissionData::handleUpdateStateDeliveryFailure(uint32_t inNetworkId, uint32_t inState, DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    //does the object still exist? it might be dead, in which case we don't resend an update
    if (FWInstanceManager::getServerEntityManager()->getEntityByID(inNetworkId))
    {
        //look in all future in flight packets, in all transmissions
        //remove written state from dirty state
        for (const auto& inFlightPacket: inDeliveryNotificationManager->getInFlightPackets())
        {
            ReplicationManagerTransmissionData* rmtdp = static_cast<ReplicationManagerTransmissionData*>(inFlightPacket.getTransmissionData('RPLM'));
            
            for (const ReplicationTransmission& otherRT: rmtdp->m_transmissions)
            {
                inState &= ~otherRT.getState();
            }
        }
        
        //if there's still any dirty state, mark it
        if (inState)
        {
            m_replicationManagerServer->setStateDirty(inNetworkId, inState);
        }
    }
}

void ReplicationManagerTransmissionData::handleDestroyDeliveryFailure(uint32_t inNetworkId) const
{
    m_replicationManagerServer->replicateDestroy(inNetworkId);
}

void ReplicationManagerTransmissionData::handleCreateDeliverySuccess(uint32_t inNetworkId) const
{
    //we've received an ack for the create, so we can start sending as only an update
    m_replicationManagerServer->handleCreateAckd(inNetworkId);
}

void ReplicationManagerTransmissionData::handleDestroyDeliverySuccess(uint32_t inNetworkId) const
{
    m_replicationManagerServer->removeFromReplication(inNetworkId);
}

ReplicationManagerTransmissionData::ReplicationTransmission::ReplicationTransmission(uint32_t inNetworkId, ReplicationAction inAction, uint32_t inState) :
m_iNetworkId(inNetworkId),
m_action(inAction),
m_iState(inState)
{
    // Empty
}

int ReplicationManagerTransmissionData::ReplicationTransmission::getID() const
{
    return m_iNetworkId;
}

ReplicationAction ReplicationManagerTransmissionData::ReplicationTransmission::getAction() const
{
    return m_action;
}

uint32_t ReplicationManagerTransmissionData::ReplicationTransmission::getState() const
{
    return m_iState;
}
