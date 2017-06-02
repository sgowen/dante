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

#include <unordered_map>
#include <stdint.h>

class DeliveryNotificationManager;

class InFlightPacket
{
public:
    InFlightPacket(uint16_t inSequenceNumber);
    
    void SetTransmissionData(int inKey, ITransmissionData* inTransmissionData);
    
    ITransmissionData* GetTransmissionData(int inKey) const;
    
    uint16_t GetSequenceNumber() const;
    
    float GetTimeDispatched() const;
    
    void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const;
    void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const;
    
private:
    uint16_t mSequenceNumber;
    float m_fTimeDispatched;
    
    std::unordered_map<int, ITransmissionData*> mTransmissionDataMap;
};

#endif /* defined(__noctisgames__InFlightPacket__) */
