//
//  NetworkHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NetworkHelper.h"

#include "OutputMemoryBitStream.h"
#include "MachineAddress.h"
#include "WeightedTimedMovingAverage.h"
#include "PacketHandler.h"

#include "InputMemoryBitStream.h"

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
