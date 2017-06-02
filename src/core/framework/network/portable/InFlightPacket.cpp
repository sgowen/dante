//
//  InFlightPacket.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "InFlightPacket.h"

#include "Timing.h"

InFlightPacket::InFlightPacket(uint16_t inSequenceNumber) :
mSequenceNumber(inSequenceNumber),
m_fTimeDispatched(Timing::getInstance()->getTime())
{
    //null out other transmision data params...
}

void InFlightPacket::SetTransmissionData(int inKey, ITransmissionData* inTransmissionData)
{
    mTransmissionDataMap[inKey] = inTransmissionData;
}

ITransmissionData* InFlightPacket::GetTransmissionData(int inKey) const
{
    auto it = mTransmissionDataMap.find(inKey);
    
    return (it != mTransmissionDataMap.end()) ? it->second : nullptr;
}

uint16_t InFlightPacket::GetSequenceNumber() const
{
    return mSequenceNumber;
}

float InFlightPacket::GetTimeDispatched() const
{
    return m_fTimeDispatched;
}

void InFlightPacket::HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    for (const auto& pair : mTransmissionDataMap)
    {
        pair.second->HandleDeliveryFailure(inDeliveryNotificationManager);
    }
}

void InFlightPacket::HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    for (const auto& pair : mTransmissionDataMap)
    {
        pair.second->HandleDeliverySuccess(inDeliveryNotificationManager);
    }
}
