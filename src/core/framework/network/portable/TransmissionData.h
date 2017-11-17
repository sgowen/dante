//
//  TransmissionData.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__TransmissionData__
#define __noctisgames__TransmissionData__

class DeliveryNotificationManager;

class TransmissionData
{
public:
    TransmissionData();
    
    virtual ~TransmissionData();
    
    virtual void handleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const = 0;
    virtual void handleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const = 0;
};

#endif /* defined(__noctisgames__TransmissionData__) */
