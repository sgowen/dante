//
//  ReplicationManagerTransmissionData.h
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ReplicationManagerTransmissionData__
#define __noctisgames__ReplicationManagerTransmissionData__

#include <framework/network/portable/TransmissionData.h>

#include <framework/network/portable/ReplicationCommand.h>
#include <framework/network/portable/ReplicationAction.h>
#include <framework/util/NGPool.h>

#include <vector>

class ReplicationManagerServer;
class EntityManager;

class ReplicationManagerTransmissionData : public TransmissionData
{
public:
    ReplicationManagerTransmissionData();
    
    virtual void free() override;
    virtual void handleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const override;
    virtual void handleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const override;
    
    void reset(ReplicationManagerServer* inReplicationManagerServer, EntityManager* entityManager, NoctisGames::NGPool<ReplicationManagerTransmissionData>* replicationManagerTransmissionDatas);
    void addTransmission(uint32_t inNetworkId, ReplicationAction inAction, uint32_t inState);
    
private:
    class ReplicationTransmission;
    
    ReplicationManagerServer* _replicationManagerServer;
    EntityManager* _entityManager;
    NoctisGames::NGPool<ReplicationManagerTransmissionData>* _replicationManagerTransmissionDatas;
    std::vector<ReplicationTransmission> _transmissions;
    
    void handleCreateDeliveryFailure(uint32_t inNetworkId) const;
    void handleUpdateStateDeliveryFailure(uint32_t inNetworkId, uint32_t inState, DeliveryNotificationManager* inDeliveryNotificationManager) const;
    void handleDestroyDeliveryFailure(uint32_t inNetworkId) const;
    void handleCreateDeliverySuccess(uint32_t inNetworkId) const;
    void handleDestroyDeliverySuccess(uint32_t inNetworkId) const;
    
    class ReplicationTransmission
    {
    public:
        ReplicationTransmission(uint32_t inNetworkId, ReplicationAction inAction, uint32_t inState);
        
        int getID() const;
        ReplicationAction getAction() const;
        uint32_t getState() const;
        
    private:
        int _networkId;
        ReplicationAction _action;
        uint32_t _state;
    };
};

#endif /* defined(__noctisgames__ReplicationManagerTransmissionData__) */
