//
//  ITransmissionData.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ITransmissionData__
#define __noctisgames__ITransmissionData__

class DeliveryNotificationManager;

class ITransmissionData
{
public:
    ITransmissionData();
    virtual ~ITransmissionData();
    
    virtual void handleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const = 0;
    virtual void handleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const = 0;
};

#endif /* defined(__noctisgames__ITransmissionData__) */
