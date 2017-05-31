//
//  InFlightPacket.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InFlightPacket__
#define __noctisgames__InFlightPacket__

#include "NetworkConstants.h"
#include "ITransmissionData.h"

#include <unordered_map>

class DeliveryNotificationManager;

class InFlightPacket
{
public:
    InFlightPacket(PacketSequenceNumber inSequenceNumber);
    
    PacketSequenceNumber GetSequenceNumber() const { return mSequenceNumber; }
    float GetTimeDispatched() const	{ return m_fTimeDispatched; }
    
    void SetTransmissionData(int inKey, ITransmissionData* inTransmissionData)
    {
        mTransmissionDataMap[inKey] = inTransmissionData;
    }
    
    ITransmissionData* GetTransmissionData(int inKey) const
    {
        auto it = mTransmissionDataMap.find(inKey);
        return (it != mTransmissionDataMap.end()) ? it->second : nullptr;
    }
    
    void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const;
    void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const;
    
private:
    PacketSequenceNumber mSequenceNumber;
    float m_fTimeDispatched;
    
    std::unordered_map<int, ITransmissionData*> mTransmissionDataMap;
};

#endif /* defined(__noctisgames__InFlightPacket__) */
