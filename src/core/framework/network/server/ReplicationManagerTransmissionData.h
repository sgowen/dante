//
//  ReplicationManagerTransmissionData.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ReplicationManagerTransmissionData__
#define __noctisgames__ReplicationManagerTransmissionData__

class ReplicationManagerServer;

class ReplicationManagerTransmissionData : public TransmissionData
{
public:

	ReplicationManagerTransmissionData(ReplicationManagerServer* inReplicationManagerServer) :
	mReplicationManagerServer(inReplicationManagerServer)
	{}

	class ReplicationTransmission
	{
	public:
		ReplicationTransmission(int inNetworkId, ReplicationAction inAction, uint32_t inState) :
		mNetworkId(inNetworkId),
		mAction(inAction),
		mState(inState)
		{}

		int							GetNetworkId()		const	{ return mNetworkId; }
		ReplicationAction		GetAction()			const	{ return mAction; }
		uint32_t					GetState()			const	{ return mState; }

	private:
		int							mNetworkId;
		ReplicationAction		mAction;
		uint32_t					mState;
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

	ReplicationManagerServer*	mReplicationManagerServer;
	
    std::vector< ReplicationTransmission >		mTransmissions;
};

typedef  std::shared_ptr< ReplicationManagerTransmissionData > ReplicationManagerTransmissionDataPtr;

#endif /* defined(__noctisgames__ReplicationManagerTransmissionData__) */
