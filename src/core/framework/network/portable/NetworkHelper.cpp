//
//  NetworkHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/portable/NetworkHelper.h>

#include <framework/network/portable/OutputMemoryBitStream.h>
#include <framework/network/portable/MachineAddress.h>
#include <framework/util/WeightedTimedMovingAverage.h>
#include <framework/network/portable/PacketHandler.h>

#include <framework/network/portable/InputMemoryBitStream.h>

NetworkHelper::NetworkHelper(PacketHandler* packetHandler) : _packetHandler(packetHandler)
{
    // Empty
}

NetworkHelper::~NetworkHelper()
{
    delete _packetHandler;
}

void NetworkHelper::processIncomingPackets()
{
    _packetHandler->processIncomingPackets();
}

void NetworkHelper::sendPacket(const OutputMemoryBitStream& inOutputStream, MachineAddress* inFromAddress)
{
    _packetHandler->sendPacket(inOutputStream, inFromAddress);
}

const WeightedTimedMovingAverage& NetworkHelper::getBytesReceivedPerSecond() const
{
    return _packetHandler->getBytesReceivedPerSecond();
}

const WeightedTimedMovingAverage& NetworkHelper::getBytesSentPerSecond() const
{
    return _packetHandler->getBytesSentPerSecond();
}
