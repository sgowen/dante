//
//  ReplicationManagerTransmissionData.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/server/ReplicationManagerTransmissionData.h>

#include <framework/network/server/ReplicationManagerServer.h>

#include <framework/entity/EntityManager.h>
#include <framework/entity/Entity.h>
#include <framework/network/portable/ReplicationAction.h>
#include <framework/network/portable/DeliveryNotificationManager.h>

ReplicationManagerTransmissionData::ReplicationManagerTransmissionData() :
_replicationManagerServer(NULL)
{
    // Empty
}

void ReplicationManagerTransmissionData::free()
{
    if (_replicationManagerTransmissionDatas)
    {
        _replicationManagerTransmissionDatas->free(this);
    }
}

void ReplicationManagerTransmissionData::handleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    //run through the transmissions
    for (const ReplicationTransmission& rt: _transmissions)
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
    for (const ReplicationTransmission& rt: _transmissions)
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

void ReplicationManagerTransmissionData::reset(ReplicationManagerServer* inReplicationManagerServer, EntityManager* entityManager, NoctisGames::NGPool<ReplicationManagerTransmissionData>* replicationManagerTransmissionDatas)
{
    _replicationManagerServer = inReplicationManagerServer;
    _entityManager = entityManager;
    _replicationManagerTransmissionDatas = replicationManagerTransmissionDatas;
    _transmissions.clear();
}

void ReplicationManagerTransmissionData::addTransmission(uint32_t inNetworkId, ReplicationAction inAction, uint32_t inState)
{
    _transmissions.push_back(ReplicationTransmission(inNetworkId, inAction, inState));
}

void ReplicationManagerTransmissionData::handleCreateDeliveryFailure(uint32_t inNetworkId) const
{
    assert(_replicationManagerServer);
    assert(_entityManager);
    
    //does the object still exist? it might be dead, in which case we don't resend a create
    Entity* entity = _entityManager->getEntityByID(inNetworkId);
    if (entity)
    {
        _replicationManagerServer->replicateCreate(inNetworkId, NG_ALL_STATE);
    }
}

void ReplicationManagerTransmissionData::handleUpdateStateDeliveryFailure(uint32_t inNetworkId, uint32_t inState, DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    assert(_replicationManagerServer);
    assert(_entityManager);
    
    //does the object still exist? it might be dead, in which case we don't resend an update
    if (_entityManager->getEntityByID(inNetworkId))
    {
        //look in all future in flight packets, in all transmissions
        //remove written state from dirty state
        for (const auto& inFlightPacket: inDeliveryNotificationManager->getInFlightPackets())
        {
            ReplicationManagerTransmissionData* rmtdp = static_cast<ReplicationManagerTransmissionData*>(inFlightPacket.getTransmissionData('RPLM'));
            
            for (const ReplicationTransmission& otherRT: rmtdp->_transmissions)
            {
                inState &= ~otherRT.getState();
            }
        }
        
        //if there's still any dirty state, mark it
        if (inState)
        {
            _replicationManagerServer->setStateDirty(inNetworkId, inState);
        }
    }
}

void ReplicationManagerTransmissionData::handleDestroyDeliveryFailure(uint32_t inNetworkId) const
{
    _replicationManagerServer->replicateDestroy(inNetworkId);
}

void ReplicationManagerTransmissionData::handleCreateDeliverySuccess(uint32_t inNetworkId) const
{
    //we've received an ack for the create, so we can start sending as only an update
    _replicationManagerServer->handleCreateAckd(inNetworkId);
}

void ReplicationManagerTransmissionData::handleDestroyDeliverySuccess(uint32_t inNetworkId) const
{
    _replicationManagerServer->removeFromReplication(inNetworkId);
}

ReplicationManagerTransmissionData::ReplicationTransmission::ReplicationTransmission(uint32_t inNetworkId, ReplicationAction inAction, uint32_t inState) :
_networkId(inNetworkId),
_action(inAction),
_state(inState)
{
    // Empty
}

int ReplicationManagerTransmissionData::ReplicationTransmission::getID() const
{
    return _networkId;
}

ReplicationAction ReplicationManagerTransmissionData::ReplicationTransmission::getAction() const
{
    return _action;
}

uint32_t ReplicationManagerTransmissionData::ReplicationTransmission::getState() const
{
    return _state;
}
