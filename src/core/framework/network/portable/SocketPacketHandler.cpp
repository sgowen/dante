//
//  SocketPacketHandler.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/portable/SocketPacketHandler.h>

#include <framework/network/portable/OutputMemoryBitStream.h>
#include <framework/network/portable/UDPSocket.h>
#include <framework/network/portable/MachineAddress.h>

#include <framework/network/portable/SocketAddress.h>
#include <framework/network/portable/InputMemoryBitStream.h>
#include <framework/network/portable/SocketUtil.h>
#include <framework/util/StringUtil.h>
#include <framework/util/Timing.h>
#include <framework/network/portable/SocketAddressFamily.h>
#include <framework/util/macros.h>
#include <framework/util/Constants.h>
#include <framework/network/portable/Network.h>

#include <string.h>
#include <assert.h>

SocketPacketHandler::SocketPacketHandler(bool isServer, uint16_t inPort, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc) : PacketHandler(isServer, processPacketFunc, handleNoResponseFunc, handleConnectionResetFunc), _socketAddress(new SocketAddress(INADDR_ANY, inPort)), _socket(NULL), _isInitialized(false)
{
    if (!SOCKET_UTIL->init())
    {
        return;
    }

    _socket = SOCKET_UTIL->createUDPSocket(INET);
    _socket->bindSocket(*_socketAddress);

    LOG("Initializing SocketPacketHandler at port %hu", inPort);

    if (_socket && _socket->setNonBlockingMode(true) == NO_ERROR)
    {
        _isInitialized = true;
    }
}

SocketPacketHandler::~SocketPacketHandler()
{
    if (_socketAddress)
    {
        delete _socketAddress;
        _socketAddress = NULL;
    }

    if (_socket)
    {
        delete _socket;
        _socket = NULL;
    }
}

void SocketPacketHandler::sendPacket(const OutputMemoryBitStream& inOutputStream, MachineAddress* inFromAddress)
{
    if (!_isInitialized)
    {
        return;
    }
    
#ifdef NG_LOG
    LOG("%s Outgoing packet Bit Length: %d \n", _isServer ? "Server" : "Client", inOutputStream.getBitLength());
#endif

    SocketAddress* inFromSocketAddress = static_cast<SocketAddress*>(inFromAddress);
    int sentByteCount = _socket->sendToAddress(inOutputStream.getBufferPtr(), inOutputStream.getByteLength(), *inFromSocketAddress);
    if (sentByteCount > 0)
    {
        _bytesSentThisFrame += sentByteCount;
    }
}

SocketAddress* SocketPacketHandler::getSocketAddress()
{
    return _socketAddress;
}

bool SocketPacketHandler::isInitialized()
{
    return _isInitialized;
}

void SocketPacketHandler::readIncomingPacketsIntoQueue()
{
    if (!_isInitialized)
    {
        return;
    }

    static char packetMem[NW_MAX_PACKET_SIZE];
    static uint32_t packetSize = sizeof(packetMem);

    bzero(packetMem, NW_MAX_PACKET_SIZE);

    InputMemoryBitStream inputStream(packetMem, packetSize * 8);
    SocketAddress fromAddress;

    //keep reading until we don't have anything to read (or we hit a max number that we'll process per frame)
    int receivedPacketCount = 0;
    int totalReadByteCount = 0;

    while (receivedPacketCount < NW_MAX_NUM_PACKETS_PER_FRAME)
    {
        int readByteCount = _socket->receiveFromAddress(packetMem, packetSize, fromAddress);
        assert(readByteCount <= packetSize);
        
        if (readByteCount == 0)
        {
            // nothing to read
            _handleNoResponseFunc();
            break;
        }
        else if (readByteCount == -WSAECONNRESET)
        {
            //port closed on other end, so DC this person immediately
            _handleConnectionResetFunc(&fromAddress);
        }
        else if (readByteCount > 0)
        {
            inputStream.resetToCapacity(readByteCount);
            ++receivedPacketCount;
            totalReadByteCount += readByteCount;
            
#ifdef NG_LOG
            if (!_isServer)
            {
                LOG("readByteCount: %d", readByteCount);
            }
#endif

            //we made it
            //shove the packet into the queue and we'll handle it as soon as we should...
            //we'll pretend it wasn't received until simulated latency from now
            //this doesn't sim jitter, for that we would need to.....

            float simulatedReceivedTime = NG_TIME->getTime();
            _packetQueue.push(ReceivedPacket(simulatedReceivedTime, inputStream, fromAddress));
        }
        else
        {
            // uhoh, error? exit or just keep going?
        }
    }

    if (totalReadByteCount > 0)
    {
        updateBytesReceivedLastFrame(totalReadByteCount);
    }
}

void SocketPacketHandler::processQueuedPackets()
{
    //look at the front packet...
    while (!_packetQueue.empty())
    {
        ReceivedPacket& nextPacket = _packetQueue.front();
        if (NG_TIME->getTime() > nextPacket.getReceivedTime())
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

SocketPacketHandler::ReceivedPacket::ReceivedPacket(float inReceivedTime, InputMemoryBitStream& ioInputMemoryBitStream, SocketAddress inFromAddress) :
_receivedTime(inReceivedTime),
_fromAddress(inFromAddress),
_packetBuffer(ioInputMemoryBitStream)
{
    // Empty
}

SocketAddress& SocketPacketHandler::ReceivedPacket::getFromAddress()
{
    return _fromAddress;
}

float SocketPacketHandler::ReceivedPacket::getReceivedTime() const
{
    return _receivedTime;
}

InputMemoryBitStream& SocketPacketHandler::ReceivedPacket::getPacketBuffer()
{
    return _packetBuffer;
}
