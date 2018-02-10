//
//  InFlightPacket.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/portable/InFlightPacket.h>

#include <framework/util/Timing.h>

InFlightPacket::InFlightPacket(uint16_t inSequenceNumber) :
_sequenceNumber(inSequenceNumber),
_timeDispatched(Timing::getInstance()->getFrameStartTime()),
_inKey(0)
{
    //null out other transmision data params...
}

InFlightPacket::~InFlightPacket()
{
    if (_inKey > 0)
    {
        TransmissionData* transmissionData = getTransmissionData(_inKey);
        if (transmissionData)
        {
            transmissionData->free();
        }
    }
}

void InFlightPacket::setTransmissionData(int inKey, TransmissionData* inTransmissionData)
{
    _inKey = inKey;
    
    _transmissionDataMap[inKey] = inTransmissionData;
}

TransmissionData* InFlightPacket::getTransmissionData(int inKey) const
{
    auto it = _transmissionDataMap.find(inKey);
    
    return (it != _transmissionDataMap.end()) ? it->second : NULL;
}

uint16_t InFlightPacket::getSequenceNumber() const
{
    return _sequenceNumber;
}

float InFlightPacket::getTimeDispatched() const
{
    return _timeDispatched;
}

void InFlightPacket::handleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    for (const auto& pair : _transmissionDataMap)
    {
        pair.second->handleDeliveryFailure(inDeliveryNotificationManager);
    }
}

void InFlightPacket::handleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
    for (const auto& pair : _transmissionDataMap)
    {
        pair.second->handleDeliverySuccess(inDeliveryNotificationManager);
    }
}
