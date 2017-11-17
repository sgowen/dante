//
//  IPacketHandler.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "IPacketHandler.h"

#include "WeightedTimedMovingAverage.h"

IPacketHandler::IPacketHandler(ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc) :
_processPacketFunc(processPacketFunc),
_handleNoResponseFunc(handleNoResponseFunc),
_handleConnectionResetFunc(handleConnectionResetFunc),
_bytesReceivedPerSecond(new WeightedTimedMovingAverage(1.f)),
_bytesSentPerSecond(new WeightedTimedMovingAverage(1.f)),
_bytesSentThisFrame(0)
{
    // Empty
}

IPacketHandler::~IPacketHandler()
{
    delete _bytesReceivedPerSecond;
    delete _bytesSentPerSecond;
}

void IPacketHandler::processIncomingPackets()
{
    readIncomingPacketsIntoQueue();
    
    processQueuedPackets();
    
    updateBytesSentLastFrame();
}

const WeightedTimedMovingAverage& IPacketHandler::getBytesReceivedPerSecond() const
{
    return *_bytesReceivedPerSecond;
}

const WeightedTimedMovingAverage& IPacketHandler::getBytesSentPerSecond() const
{
    return *_bytesSentPerSecond;
}

void IPacketHandler::updateBytesSentLastFrame()
{
    if (_bytesSentThisFrame > 0)
    {
        _bytesSentPerSecond->updatePerSecond(static_cast<float>(_bytesSentThisFrame));
        
        _bytesSentThisFrame = 0;
    }
}

void IPacketHandler::updateBytesReceivedLastFrame(int totalReadByteCount)
{
    _bytesReceivedPerSecond->updatePerSecond(static_cast<float>(totalReadByteCount));
}
