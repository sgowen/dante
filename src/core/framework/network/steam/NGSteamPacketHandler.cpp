//
//  NGSteamPacketHandler.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/steam/NGSteamPacketHandler.h>

#include <framework/network/portable/OutputMemoryBitStream.h>
#include <framework/network/portable/MachineAddress.h>

#include <framework/network/portable/InputMemoryBitStream.h>
#include <framework/util/Timing.h>
#include <framework/network/steam/NGSteam.h>
#include <framework/util/Constants.h>
#include <framework/util/StringUtil.h>
#include <framework/network/portable/Network.h>

#include <assert.h>

NGSteamPacketHandler::NGSteamPacketHandler(Timing* timing, bool isServer, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc) : PacketHandler(timing, isServer, processPacketFunc, handleNoResponseFunc, handleConnectionResetFunc)
{
    // Empty
}

NGSteamPacketHandler::~NGSteamPacketHandler()
{
    // Empty
}

void NGSteamPacketHandler::sendPacket(const OutputMemoryBitStream& inOutputStream, MachineAddress* inFromAddress)
{
#ifdef NG_LOG
    LOG("%s Outgoing packet Bit Length: %d \n", _isServer ? "Server" : "Client", inOutputStream.getBitLength());
#endif
    
    NGSteamAddress* inFromSteamAddress = static_cast<NGSteamAddress*>(inFromAddress);
    
    ISteamNetworking* steamNetworking = _isServer ? SteamGameServerNetworking() : SteamNetworking();
    
    if (steamNetworking->SendP2PPacket(inFromSteamAddress->getSteamID(), inOutputStream.getBufferPtr(), inOutputStream.getByteLength(), inFromSteamAddress->isReliable() ? k_EP2PSendReliable : k_EP2PSendUnreliable))
    {
        int sentByteCount = inOutputStream.getByteLength();
        if (sentByteCount > 0)
        {
            _bytesSentThisFrame += sentByteCount;
        }
    }
    else
    {
        LOG("Failed sending data to server");
    }
}

void NGSteamPacketHandler::readIncomingPacketsIntoQueue()
{
    static char packetMem[NW_MAX_PACKET_SIZE];
    static uint32 packetSize = sizeof(packetMem);
    
    bzero(packetMem, NW_MAX_PACKET_SIZE);
    
    uint32_t incomingSize = 0;
    InputMemoryBitStream inputStream(packetMem, packetSize * 8);
    CSteamID fromId;
    
    //keep reading until we don't have anything to read (or we hit a max number that we'll process per frame)
    int receivedPackedCount = 0;
    int totalReadByteCount = 0;
    
    ISteamNetworking* steamNetworking = _isServer ? SteamGameServerNetworking() : SteamNetworking();
    
    while (steamNetworking->IsP2PPacketAvailable(&incomingSize) && receivedPackedCount < NW_MAX_NUM_PACKETS_PER_FRAME)
    {
        if (incomingSize <= packetSize)
        {
            uint32_t readByteCount;
            if (steamNetworking->ReadP2PPacket(packetMem, packetSize, &readByteCount, &fromId))
            {
                assert(readByteCount <= packetSize);
                
                if (readByteCount > 0)
                {
                    inputStream.resetToCapacity(readByteCount);
                    ++receivedPackedCount;
                    totalReadByteCount += readByteCount;
                    
                    //shove the packet into the queue and we'll handle it as soon as we should...
                    //we'll pretend it wasn't received until simulated latency from now
                    //this doesn't sim jitter, for that we would need to.....
                    float simulatedReceivedTime = _timing->getTime();
                    
                    _packetQueue.push(ReceivedPacket(simulatedReceivedTime, inputStream, fromId));
                }
                
#ifdef NG_LOG
                if (!_isServer)
                {
                    LOG("readByteCount: %d", readByteCount);
                }
#endif
            }
        }
    }
    
    if (totalReadByteCount > 0)
    {
        updateBytesReceivedLastFrame(totalReadByteCount);
    }
    else
    {
        _handleNoResponseFunc();
    }
}

void NGSteamPacketHandler::processQueuedPackets()
{
    //look at the front packet...
    while (!_packetQueue.empty())
    {
        ReceivedPacket& nextPacket = _packetQueue.front();
        if (_timing->getTime() > nextPacket.getReceivedTime())
        {
            _processPacketFunc(nextPacket.getPacketBuffer(), &nextPacket.getFromAddress());
            _packetQueue.pop();
        }
        else
        {
            break;
        }
    }
}

NGSteamPacketHandler::ReceivedPacket::ReceivedPacket(float inReceivedTime, InputMemoryBitStream& ioInputMemoryBitStream, NGSteamAddress inFromAddress) :
_receivedTime(inReceivedTime),
_fromAddress(inFromAddress),
_packetBuffer(ioInputMemoryBitStream)
{
    // Empty
}

NGSteamAddress& NGSteamPacketHandler::ReceivedPacket::getFromAddress()
{
    return _fromAddress;
}

float NGSteamPacketHandler::ReceivedPacket::getReceivedTime() const
{
    return _receivedTime;
}

InputMemoryBitStream& NGSteamPacketHandler::ReceivedPacket::getPacketBuffer()
{
    return _packetBuffer;
}
