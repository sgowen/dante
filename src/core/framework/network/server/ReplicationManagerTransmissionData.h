//
//  ReplicationManagerTransmissionData.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ReplicationManagerTransmissionData__
#define __noctisgames__ReplicationManagerTransmissionData__

#include "framework/network/portable/TransmissionData.h"

#include "framework/network/portable/ReplicationCommand.h"
#include "framework/network/portable/ReplicationAction.h"

#include <vector>

class ReplicationManagerServer;

class ReplicationManagerTransmissionData : public TransmissionData
{
public:
    ReplicationManagerTransmissionData();
    
    void reset(ReplicationManagerServer* inReplicationManagerServer);
    
    void addTransmission(uint32_t inNetworkId, ReplicationAction inAction, uint32_t inState);
    
    virtual void handleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const override;
    
    virtual void handleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const override;
    
private:
    class ReplicationTransmission;
    
    ReplicationManagerServer* _replicationManagerServer;
    
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
