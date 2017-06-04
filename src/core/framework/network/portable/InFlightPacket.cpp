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
m_iSequenceNumber(inSequenceNumber),
m_fTimeDispatched(Timing::getInstance()->getTime())
{
    //null out other transmision data params...
}

void InFlightPacket::setTransmissionData(int inKey, ITransmissionData* inTransmissionData)
{
    ITransmissionData* currentTransmissionData;
    if ((currentTransmissionData = getTransmissionData(inKey)))
    {
        delete currentTransmissionData;
    }
    
    m_transmissionDataMap[inKey] = inTransmissionData;
}

ITransmissionData* InFlightPacket::getTransmissionData(int inKey) const
{
    auto it = m_transmissionDataMap.find(inKey);
    
    return (it != m_transmissionDataMap.end()) ? it->second : nullptr;
}

uint16_t InFlightPacket::getSequenceNumber() const
{
    return m_iSequenceNumber;
}

float InFlightPacket::getTimeDispatched() const
{
    return m_fTimeDispatched;
}

void InFlightPacket::handleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    for (const auto& pair : m_transmissionDataMap)
    {
        pair.second->handleDeliveryFailure(inDeliveryNotificationManager);
    }
}

void InFlightPacket::handleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    for (const auto& pair : m_transmissionDataMap)
    {
        pair.second->handleDeliverySuccess(inDeliveryNotificationManager);
    }
}
