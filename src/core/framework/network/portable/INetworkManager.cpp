//
//  INetworkManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "INetworkManager.h"

#include "OutputMemoryBitStream.h"
#include "WeightedTimedMovingAverage.h"
#include "SocketAddress.h"
#include "IMachineAddress.h"

#include "SocketPacketHandler.h"
#include "macros.h"

void INetworkManager::processIncomingPackets()
{
    m_packetHandler->readIncomingPacketsIntoQueue();
    
    m_packetHandler->processQueuedPackets();
    
    m_packetHandler->updateBytesSentLastFrame();
}

const WeightedTimedMovingAverage& INetworkManager::getBytesReceivedPerSecond() const
{
    return m_packetHandler->getBytesReceivedPerSecond();
}

const WeightedTimedMovingAverage& INetworkManager::getBytesSentPerSecond() const
{
    return m_packetHandler->getBytesSentPerSecond();
}

void INetworkManager::handleConnectionReset(SocketAddress* inFromAddress)
{
    UNUSED(inFromAddress);
}

void INetworkManager::sendPacket(const OutputMemoryBitStream& inOutputStream, IMachineAddress* inFromAddress)
{
    m_packetHandler->sendPacket(inOutputStream, inFromAddress);
}

INetworkManager::INetworkManager(uint16_t inPort, ProcessPacketFunc processPacketFunc, HandleConnectionResetFunc handleConnectionResetFunc) :
m_packetHandler(new SocketPacketHandler(inPort, processPacketFunc, handleConnectionResetFunc))
{
    // Empty
}

INetworkManager::~INetworkManager()
{
    delete m_packetHandler;
}
