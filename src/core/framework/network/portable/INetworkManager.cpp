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
#include "Entity.h"

#include "SocketUtil.h"
#include "StringUtil.h"
#include "Timing.h"
#include "SocketAddressFamily.h"
#include "EntityManager.h"
#include "macros.h"

void INetworkManager::handleConnectionReset(const SocketAddress& inFromAddress)
{
    UNUSED(inFromAddress);
}

bool INetworkManager::init(uint16_t inPort, HandleEntityDeletion handleEntityDeletion)
{
    m_handleEntityDeletion = handleEntityDeletion;
    
    m_socket = SOCKET_UTIL->createUDPSocket(INET);
    SocketAddress ownAddress(INADDR_ANY, inPort);
    m_socket->bindSocket(ownAddress);
    
    LOG("Initializing INetworkManager at port %d", inPort);
    
    m_bytesReceivedPerSecond = WeightedTimedMovingAverage(1.f);
    m_bytesSentPerSecond = WeightedTimedMovingAverage(1.f);
    
    //did we bind okay?
    if (m_socket == nullptr)
    {
        return false;
    }
    
    if (m_socket->setNonBlockingMode(true) != NO_ERROR)
    {
        return false;
    }
    
    return true;
}

void INetworkManager::processIncomingPackets()
{
    readIncomingPacketsIntoQueue();
    
    processQueuedPackets();
    
    updateBytesSentLastFrame();
}

void INetworkManager::sendPacket(const OutputMemoryBitStream& inOutputStream, const SocketAddress& inFromAddress)
{
    int sentByteCount = m_socket->sendToAddress(inOutputStream.getBufferPtr(), inOutputStream.getByteLength(), inFromAddress);
    if (sentByteCount > 0)
    {
        m_bytesSentThisFrame += sentByteCount;
    }
}

const WeightedTimedMovingAverage& INetworkManager::getBytesReceivedPerSecond() const
{
    return m_bytesReceivedPerSecond;
}

const WeightedTimedMovingAverage& INetworkManager::getBytesSentPerSecond() const
{
    return m_bytesSentPerSecond;
}

Entity* INetworkManager::getEntity(int inNetworkId) const
{
    return ENTITY_MGR->getEntityFromID(inNetworkId);
}

void INetworkManager::addToNetworkIdToEntityMap(Entity* inEntity)
{
    ENTITY_MGR->registerEntity(inEntity);
}

void INetworkManager::removeFromNetworkIdToEntityMap(Entity* inEntity)
{
    m_handleEntityDeletion(inEntity);
    
    ENTITY_MGR->removeEntity(inEntity);
    
    delete inEntity;
    inEntity = nullptr;
}

void INetworkManager::updateBytesSentLastFrame()
{
    if (m_bytesSentThisFrame > 0)
    {
        m_bytesSentPerSecond.updatePerSecond(static_cast<float>(m_bytesSentThisFrame));
        
        m_bytesSentThisFrame = 0;
    }
}

void INetworkManager::readIncomingPacketsIntoQueue()
{
    //should we just keep a static one?
    //should we just keep a static one?
    char packetMem[1500];
    int packetSize = sizeof(packetMem);
    InputMemoryBitStream inputStream(packetMem, packetSize * 8);
    SocketAddress fromAddress;
    
    //keep reading until we don't have anything to read (or we hit a max number that we'll process per frame)
    int receivedPackedCount = 0;
    int totalReadByteCount = 0;
    
    while (receivedPackedCount < kMaxPacketsPerFrameCount)
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
            handleConnectionReset(fromAddress);
        }
        else if (readByteCount > 0)
        {
            inputStream.resetToCapacity(readByteCount);
            ++receivedPackedCount;
            totalReadByteCount += readByteCount;
            
            //we made it
            //shove the packet into the queue and we'll handle it as soon as we should...
            //we'll pretend it wasn't received until simulated latency from now
            //this doesn't sim jitter, for that we would need to.....
            
            float simulatedReceivedTime = Timing::getInstance()->getTime();
            m_packetQueue.emplace(simulatedReceivedTime, inputStream, fromAddress);
        }
        else
        {
            //uhoh, error? exit or just keep going?
        }
    }
    
    if (totalReadByteCount > 0)
    {
        m_bytesReceivedPerSecond.updatePerSecond(static_cast<float>(totalReadByteCount));
    }
}

void INetworkManager::processQueuedPackets()
{
    //look at the front packet...
    while (!m_packetQueue.empty())
    {
        ReceivedPacket& nextPacket = m_packetQueue.front();
        if (Timing::getInstance()->getTime() > nextPacket.getReceivedTime())
        {
            processPacket(nextPacket.getPacketBuffer(), nextPacket.getFromAddress());
            m_packetQueue.pop();
        }
        else
        {
            break;
        }
    }
}

INetworkManager::INetworkManager() : m_bytesSentThisFrame(0)
{
    // Empty
}

INetworkManager::~INetworkManager()
{
    delete m_socket;
}

INetworkManager::ReceivedPacket::ReceivedPacket(float inReceivedTime, InputMemoryBitStream& ioInputMemoryBitStream, const SocketAddress& inFromAddress) :
m_fReceivedTime(inReceivedTime),
m_fromAddress(inFromAddress),
m_packetBuffer(ioInputMemoryBitStream)
{
}

const SocketAddress& INetworkManager::ReceivedPacket::getFromAddress()
{
    return m_fromAddress;
}

float INetworkManager::ReceivedPacket::getReceivedTime() const
{
    return m_fReceivedTime;
}

InputMemoryBitStream& INetworkManager::ReceivedPacket::getPacketBuffer()
{
    return m_packetBuffer;
}
