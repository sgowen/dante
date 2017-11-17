//
//  INetworkHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "INetworkHelper.h"

#include "OutputMemoryBitStream.h"
#include "IMachineAddress.h"
#include "WeightedTimedMovingAverage.h"
#include "IPacketHandler.h"

#include "InputMemoryBitStream.h"

INetworkHelper::INetworkHelper(IPacketHandler* packetHandler) : _packetHandler(packetHandler)
{
    // Empty
}

INetworkHelper::~INetworkHelper()
{
    delete _packetHandler;
}

void INetworkHelper::processIncomingPackets()
{
    _packetHandler->processIncomingPackets();
}

void INetworkHelper::sendPacket(const OutputMemoryBitStream& inOutputStream, IMachineAddress* inFromAddress)
{
    _packetHandler->sendPacket(inOutputStream, inFromAddress);
}

const WeightedTimedMovingAverage& INetworkHelper::getBytesReceivedPerSecond() const
{
    return _packetHandler->getBytesReceivedPerSecond();
}

const WeightedTimedMovingAverage& INetworkHelper::getBytesSentPerSecond() const
{
    return _packetHandler->getBytesSentPerSecond();
}
