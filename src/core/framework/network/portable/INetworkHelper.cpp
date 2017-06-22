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

INetworkHelper::INetworkHelper(IPacketHandler* packetHandler) : m_packetHandler(packetHandler)
{
    // Empty
}

INetworkHelper::~INetworkHelper()
{
    delete m_packetHandler;
}

void INetworkHelper::processIncomingPackets()
{
    m_packetHandler->processIncomingPackets();
}

void INetworkHelper::sendPacket(const OutputMemoryBitStream& inOutputStream, IMachineAddress* inFromAddress)
{
    m_packetHandler->sendPacket(inOutputStream, inFromAddress);
}

const WeightedTimedMovingAverage& INetworkHelper::getBytesReceivedPerSecond() const
{
    return m_packetHandler->getBytesReceivedPerSecond();
}

const WeightedTimedMovingAverage& INetworkHelper::getBytesSentPerSecond() const
{
    return m_packetHandler->getBytesSentPerSecond();
}
