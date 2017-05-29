//
//  ITransmissionData.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ITransmissionData__
#define __noctisgames__ITransmissionData__

#include <memory>

class DeliveryNotificationManager;

class ITransmissionData
{
public:
    virtual void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const = 0;
    virtual void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const = 0;
};

typedef std::shared_ptr<ITransmissionData> TransmissionDataPtr;

#endif /* defined(__noctisgames__ITransmissionData__) */
