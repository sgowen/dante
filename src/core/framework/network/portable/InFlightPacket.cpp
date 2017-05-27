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

InFlightPacket::InFlightPacket(PacketSequenceNumber inSequenceNumber) :
mSequenceNumber(inSequenceNumber),
m_fTimeDispatched(Timing::sInstance.GetTime())
{
    //null out other transmision data params...
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
