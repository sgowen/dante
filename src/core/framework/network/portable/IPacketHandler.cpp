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
m_processPacketFunc(processPacketFunc),
m_handleNoResponseFunc(handleNoResponseFunc),
m_handleConnectionResetFunc(handleConnectionResetFunc),
m_bytesReceivedPerSecond(new WeightedTimedMovingAverage(1.f)),
m_bytesSentPerSecond(new WeightedTimedMovingAverage(1.f)),
m_bytesSentThisFrame(0)
{
    // Empty
}

IPacketHandler::~IPacketHandler()
{
    delete m_bytesReceivedPerSecond;
    delete m_bytesSentPerSecond;
}

void IPacketHandler::processIncomingPackets()
{
    readIncomingPacketsIntoQueue();
    
    processQueuedPackets();
    
    updateBytesSentLastFrame();
}

const WeightedTimedMovingAverage& IPacketHandler::getBytesReceivedPerSecond() const
{
    return *m_bytesReceivedPerSecond;
}

const WeightedTimedMovingAverage& IPacketHandler::getBytesSentPerSecond() const
{
    return *m_bytesSentPerSecond;
}

void IPacketHandler::updateBytesSentLastFrame()
{
    if (m_bytesSentThisFrame > 0)
    {
        m_bytesSentPerSecond->updatePerSecond(static_cast<float>(m_bytesSentThisFrame));
        
        m_bytesSentThisFrame = 0;
    }
}

void IPacketHandler::updateBytesReceivedLastFrame(int totalReadByteCount)
{
    m_bytesReceivedPerSecond->updatePerSecond(static_cast<float>(totalReadByteCount));
}
