//
//  TransmissionData.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__TransmissionData__
#define __noctisgames__TransmissionData__

#include "RoboCatShared.h"

class DeliveryNotificationManager;

class TransmissionData
{
public:
	virtual void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const = 0;
	virtual void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const = 0;
};
typedef shared_ptr< TransmissionData > TransmissionDataPtr;

#endif /* defined(__noctisgames__TransmissionData__) */
