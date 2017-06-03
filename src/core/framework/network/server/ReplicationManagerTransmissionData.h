//
//  ReplicationManagerTransmissionData.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ReplicationManagerTransmissionData__
#define __noctisgames__ReplicationManagerTransmissionData__

#include "ITransmissionData.h"

#include "ReplicationCommand.h"
#include "ReplicationAction.h"

#include <vector>

class ReplicationManagerServer;

class ReplicationManagerTransmissionData : public ITransmissionData
{
public:
    ReplicationManagerTransmissionData(ReplicationManagerServer* inReplicationManagerServer);
    
    void addTransmission(int inNetworkId, ReplicationAction inAction, uint32_t inState);
    
    virtual void handleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const override;
    
    virtual void handleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const override;
    
private:
    class ReplicationTransmission;
    
    ReplicationManagerServer* m_replicationManagerServer;
    
    std::vector<ReplicationTransmission> m_transmissions;
    
    void handleCreateDeliveryFailure(int inNetworkId) const;
    
    void handleUpdateStateDeliveryFailure(int inNetworkId, uint32_t inState, DeliveryNotificationManager* inDeliveryNotificationManager) const;
    
    void handleDestroyDeliveryFailure(int inNetworkId) const;
    
    void handleCreateDeliverySuccess(int inNetworkId) const;
    
    void handleDestroyDeliverySuccess(int inNetworkId) const;
    
    class ReplicationTransmission
    {
    public:
        ReplicationTransmission(int inNetworkId, ReplicationAction inAction, uint32_t inState);
        
        int getID() const;
        
        ReplicationAction getAction() const;
        
        uint32_t getState() const;
        
    private:
        int m_iNetworkId;
        ReplicationAction m_action;
        uint32_t m_iState;
    };
};

#endif /* defined(__noctisgames__ReplicationManagerTransmissionData__) */
