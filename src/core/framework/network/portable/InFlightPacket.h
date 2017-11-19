//
//  InFlightPacket.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InFlightPacket__
#define __noctisgames__InFlightPacket__

#include "framework/network/portable/TransmissionData.h"

#include <map>
#include <stdint.h>

class DeliveryNotificationManager;

class InFlightPacket
{
public:
    InFlightPacket(uint16_t inSequenceNumber);
    
    void setTransmissionData(int inKey, TransmissionData* inTransmissionData);
    
    TransmissionData* getTransmissionData(int inKey) const;
    
    uint16_t getSequenceNumber() const;
    
    float getTimeDispatched() const;
    
    void handleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const;
    
    void handleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const;
    
private:
    uint16_t _sequenceNumber;
    float _timeDispatched;
    
    std::map<int, TransmissionData*> _transmissionDataMap;
};

#endif /* defined(__noctisgames__InFlightPacket__) */
