//
//  SocketPacketHandler.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SocketPacketHandler.h"

#include "OutputMemoryBitStream.h"
#include "WeightedTimedMovingAverage.h"
#include "UDPSocket.h"
#include "IMachineAddress.h"

#include "SocketAddress.h"
#include "InputMemoryBitStream.h"
#include "SocketUtil.h"
#include "StringUtil.h"
#include "Timing.h"
#include "SocketAddressFamily.h"
#include "macros.h"

SocketPacketHandler::SocketPacketHandler(uint16_t inPort, ProcessPacketFunc processPacketFunc, HandleConnectionResetFunc handleConnectionResetFunc) :
IPacketHandler(),
m_processPacketFunc(processPacketFunc),
m_handleConnectionResetFunc(handleConnectionResetFunc)
{
    if (!SOCKET_UTIL->init())
    {
        return;
    }
    
    m_socket = SOCKET_UTIL->createUDPSocket(INET);
    SocketAddress ownAddress(INADDR_ANY, inPort);
    m_socket->bindSocket(ownAddress);
    
    LOG("Initializing SocketPacketHandler at port %hu", inPort);
    
    if (m_socket == nullptr)
    {
        return;
    }
    
    if (m_socket->setNonBlockingMode(true) != NO_ERROR)
    {
        return;
    }
}

SocketPacketHandler::~SocketPacketHandler()
{
    if (m_socket)
    {
        delete m_socket;
        m_socket = nullptr;
    }
}

void SocketPacketHandler::sendPacket(const OutputMemoryBitStream& inOutputStream, IMachineAddress* inFromAddress)
{
    SocketAddress* inFromSocketAddress = static_cast<SocketAddress*>(inFromAddress);
    int sentByteCount = m_socket->sendToAddress(inOutputStream.getBufferPtr(), inOutputStream.getByteLength(), *inFromSocketAddress);
    if (sentByteCount > 0)
    {
        m_bytesSentThisFrame += sentByteCount;
    }
}

void SocketPacketHandler::readIncomingPacketsIntoQueue()
{
    //should we just keep a static one?
    char packetMem[1500];
    int packetSize = sizeof(packetMem);
    InputMemoryBitStream inputStream(packetMem, packetSize * 8);
    SocketAddress fromAddress;
    
    //keep reading until we don't have anything to read (or we hit a max number that we'll process per frame)
    int receivedPacketCount = 0;
    int totalReadByteCount = 0;
    
    while (receivedPacketCount < kMaxPacketsPerFrameCount)
    {
        int readByteCount = m_socket->receiveFromAddress(packetMem, packetSize, fromAddress);
        if (readByteCount == 0)
        {
            //nothing to read
            break;
        }
        else if (readByteCount == -WSAECONNRESET)
        {
            //port closed on other end, so DC this person immediately
            m_handleConnectionResetFunc(&fromAddress);
        }
        else if (readByteCount > 0)
        {
            inputStream.resetToCapacity(readByteCount);
            ++receivedPacketCount;
            totalReadByteCount += readByteCount;
            
            //we made it
            //shove the packet into the queue and we'll handle it as soon as we should...
            //we'll pretend it wasn't received until simulated latency from now
            //this doesn't sim jitter, for that we would need to.....
            
            float simulatedReceivedTime = Timing::getInstance()->getFrameStartTime();
            m_packetQueue.emplace(ReceivedPacket(simulatedReceivedTime, inputStream, fromAddress));
        }
        else
        {
            //uhoh, error? exit or just keep going?
        }
    }
    
    if (totalReadByteCount > 0)
    {
        m_bytesReceivedPerSecond->updatePerSecond(static_cast<float>(totalReadByteCount));
    }
}

void SocketPacketHandler::processQueuedPackets()
{
    //look at the front packet...
    while (!m_packetQueue.empty())
    {
        ReceivedPacket& nextPacket = m_packetQueue.front();
        if (Timing::getInstance()->getFrameStartTime() > nextPacket.getReceivedTime())
        {
            m_processPacketFunc(nextPacket.getPacketBuffer(), &nextPacket.getFromAddress());
            m_packetQueue.pop();
        }
        else
        {
            break;
        }
    }
}

SocketPacketHandler::ReceivedPacket::ReceivedPacket(float inReceivedTime, InputMemoryBitStream& ioInputMemoryBitStream, SocketAddress inFromAddress) :
m_fReceivedTime(inReceivedTime),
m_fromAddress(inFromAddress),
m_packetBuffer(ioInputMemoryBitStream)
{
    // Empty
}

SocketAddress& SocketPacketHandler::ReceivedPacket::getFromAddress()
{
    return m_fromAddress;
}

float SocketPacketHandler::ReceivedPacket::getReceivedTime() const
{
    return m_fReceivedTime;
}

InputMemoryBitStream& SocketPacketHandler::ReceivedPacket::getPacketBuffer()
{
    return m_packetBuffer;
}
