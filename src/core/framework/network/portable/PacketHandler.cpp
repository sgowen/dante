//
//  PacketHandler.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/portable/PacketHandler.h>

#include <framework/util/Timing.h>
#include <framework/util/WeightedTimedMovingAverage.h>
#include <framework/network/portable/InputMemoryBitStream.h>
#include <framework/network/portable/OutputMemoryBitStream.h>
#include <framework/network/portable/MachineAddress.h>

PacketHandler::PacketHandler(Timing* timing, bool isServer, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc) :
_timing(timing),
_processPacketFunc(processPacketFunc),
_handleNoResponseFunc(handleNoResponseFunc),
_handleConnectionResetFunc(handleConnectionResetFunc),
_bytesReceivedPerSecond(new WeightedTimedMovingAverage(timing, 1.f)),
_bytesSentPerSecond(new WeightedTimedMovingAverage(timing, 1.f)),
_bytesSentThisFrame(0),
_isServer(isServer)
{
    // Empty
}

PacketHandler::~PacketHandler()
{
    delete _bytesReceivedPerSecond;
    delete _bytesSentPerSecond;
}

void PacketHandler::processIncomingPackets()
{
    readIncomingPacketsIntoQueue();
    
    processQueuedPackets();
    
    updateBytesSentLastFrame();
}

const WeightedTimedMovingAverage& PacketHandler::getBytesReceivedPerSecond() const
{
    return *_bytesReceivedPerSecond;
}

const WeightedTimedMovingAverage& PacketHandler::getBytesSentPerSecond() const
{
    return *_bytesSentPerSecond;
}

void PacketHandler::updateBytesSentLastFrame()
{
    if (_bytesSentThisFrame > 0)
    {
        _bytesSentPerSecond->updatePerSecond(static_cast<float>(_bytesSentThisFrame));
        
        _bytesSentThisFrame = 0;
    }
}

void PacketHandler::updateBytesReceivedLastFrame(int totalReadByteCount)
{
    _bytesReceivedPerSecond->updatePerSecond(static_cast<float>(totalReadByteCount));
}
