//
//  InFlightPacket.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InFlightPacket__
#define __noctisgames__InFlightPacket__

#include "ITransmissionData.h"

#include <map>
#include <stdint.h>

class DeliveryNotificationManager;

class InFlightPacket
{
public:
    InFlightPacket(uint16_t inSequenceNumber);
    
    void setTransmissionData(int inKey, ITransmissionData* inTransmissionData);
    
    ITransmissionData* getTransmissionData(int inKey) const;
    
    uint16_t getSequenceNumber() const;
    
    float getTimeDispatched() const;
    
    void handleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const;
    
    void handleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const;
    
private:
    uint16_t m_iSequenceNumber;
    float m_fTimeDispatched;
    
    std::map<int, ITransmissionData*> m_transmissionDataMap;
};

#endif /* defined(__noctisgames__InFlightPacket__) */
