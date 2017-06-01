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
    ReplicationManagerTransmissionData(ReplicationManagerServer* inReplicationManagerServer) :
    mReplicationManagerServer(inReplicationManagerServer)
    {}
    
    class ReplicationTransmission
    {
    public:
        ReplicationTransmission(int inNetworkId, ReplicationAction inAction, uint32_t inState) :
        m_iNetworkId(inNetworkId),
        mAction(inAction),
        mState(inState)
        {}
        
        int getID() const { return m_iNetworkId; }
        ReplicationAction GetAction() const	{ return mAction; }
        uint32_t GetState() const { return mState; }
        
    private:
        int m_iNetworkId;
        ReplicationAction mAction;
        uint32_t mState;
    };
    
    void AddTransmission(int inNetworkId, ReplicationAction inAction, uint32_t inState);
    
    virtual void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const override;
    virtual void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const override;
    
private:
    void HandleCreateDeliveryFailure(int inNetworkId) const;
    void HandleUpdateStateDeliveryFailure(int inNetworkId, uint32_t inState, DeliveryNotificationManager* inDeliveryNotificationManager) const;
    void HandleDestroyDeliveryFailure(int inNetworkId) const;
    void HandleCreateDeliverySuccess(int inNetworkId) const;
    void HandleDestroyDeliverySuccess(int inNetworkId) const;
    
    ReplicationManagerServer* mReplicationManagerServer;
    
    std::vector<ReplicationTransmission> mTransmissions;
};

#endif /* defined(__noctisgames__ReplicationManagerTransmissionData__) */
