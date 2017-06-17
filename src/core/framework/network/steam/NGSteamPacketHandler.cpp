//
//  NGSteamPacketHandler.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NGSteamPacketHandler.h"

#include "OutputMemoryBitStream.h"
#include "IMachineAddress.h"

#include "InputMemoryBitStream.h"
#include "Timing.h"
#include "NGSteam.h"
#include "FrameworkConstants.h"

NGSteamPacketHandler::NGSteamPacketHandler(ProcessPacketFunc processPacketFunc, HandleConnectionResetFunc handleConnectionResetFunc) : IPacketHandler(processPacketFunc, handleConnectionResetFunc)
{
    // Empty
}

NGSteamPacketHandler::~NGSteamPacketHandler()
{
    // Empty
}

void NGSteamPacketHandler::sendPacket(const OutputMemoryBitStream& inOutputStream, IMachineAddress* inFromAddress)
{
    NGSteamAddress* inFromSteamAddress = static_cast<NGSteamAddress*>(inFromAddress);
    
    if (SteamGameServerNetworking()->SendP2PPacket(inFromSteamAddress->getSteamID(), inOutputStream.getBufferPtr(), inOutputStream.getByteLength(), k_EP2PSendUnreliable))
    {
        int sentByteCount = inOutputStream.getByteLength();
        if (sentByteCount > 0)
        {
            m_bytesSentThisFrame += sentByteCount;
        }
    }
}

void NGSteamPacketHandler::readIncomingPacketsIntoQueue()
{
    char packetMem[1500];
    size_t packetSize = sizeof(packetMem);
    uint32_t incomingSize = 0;
    InputMemoryBitStream inputStream(packetMem, packetSize * 8);
    CSteamID fromId;
    
    //keep reading until we don't have anything to read (or we hit a max number that we'll process per frame)
    int receivedPackedCount = 0;
    int totalReadByteCount = 0;
    
    while (SteamNetworking()->IsP2PPacketAvailable(&incomingSize) &&
          receivedPackedCount < NETWORK_MAX_PACKETS_PER_FRAME)
    {
        if (incomingSize <= packetSize)
        {
            uint32_t readByteCount;
            
            if (SteamGameServerNetworking()->ReadP2PPacket(packetMem, packetSize, &readByteCount, &fromId)
                && readByteCount > 0)
            {
                inputStream.resetToCapacity(readByteCount);
                ++receivedPackedCount;
                totalReadByteCount += readByteCount;
                
                //shove the packet into the queue and we'll handle it as soon as we should...
                //we'll pretend it wasn't received until simulated latency from now
                //this doesn't sim jitter, for that we would need to.....
                float simulatedReceivedTime = Timing::getInstance()->getFrameStartTime();
                
                m_packetQueue.emplace(simulatedReceivedTime, inputStream, fromId);
            }
        }
    }
    
    if (totalReadByteCount > 0)
    {
        updateBytesReceivedLastFrame(totalReadByteCount);
    }
}

void NGSteamPacketHandler::processQueuedPackets()
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

NGSteamPacketHandler::ReceivedPacket::ReceivedPacket(float inReceivedTime, InputMemoryBitStream& ioInputMemoryBitStream, NGSteamAddress inFromAddress) :
m_fReceivedTime(inReceivedTime),
m_fromAddress(inFromAddress),
m_packetBuffer(ioInputMemoryBitStream)
{
    // Empty
}

NGSteamAddress& NGSteamPacketHandler::ReceivedPacket::getFromAddress()
{
    return m_fromAddress;
}

float NGSteamPacketHandler::ReceivedPacket::getReceivedTime() const
{
    return m_fReceivedTime;
}

InputMemoryBitStream& NGSteamPacketHandler::ReceivedPacket::getPacketBuffer()
{
    return m_packetBuffer;
}
