//
//  InFlightPacket.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "RoboCatShared.h"

InFlightPacket::InFlightPacket(PacketSequenceNumber inSequenceNumber) :
mSequenceNumber(inSequenceNumber),
mTimeDispatched(Timing::sInstance.GetTimef())
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
