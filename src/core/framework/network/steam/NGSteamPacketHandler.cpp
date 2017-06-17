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

NGSteamPacketHandler::NGSteamPacketHandler(ProcessPacketFunc processPacketFunc, HandleConnectionResetFunc handleConnectionResetFunc) :
IPacketHandler(),
m_processPacketFunc(processPacketFunc),
m_handleConnectionResetFunc(handleConnectionResetFunc)
{
    // TODO
}

NGSteamPacketHandler::~NGSteamPacketHandler()
{
    // TODO
}

void NGSteamPacketHandler::sendPacket(const OutputMemoryBitStream& inOutputStream, IMachineAddress* inFromAddress)
{
    // TODO
}

void NGSteamPacketHandler::readIncomingPacketsIntoQueue()
{
    // TODO
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
