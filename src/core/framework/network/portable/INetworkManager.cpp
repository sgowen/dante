//
//  INetworkManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "INetworkManager.h"

#include "EntityManager.h"
#include "OutputMemoryBitStream.h"
#include "Entity.h"
#include "WeightedTimedMovingAverage.h"

#include "SocketUtil.h"
#include "StringUtil.h"
#include "Timing.h"
#include "SocketAddressFamily.h"
#include "macros.h"

void INetworkManager::handleConnectionReset(const SocketAddress& inFromAddress)
{
    UNUSED(inFromAddress);
}

bool INetworkManager::init(uint16_t inPort, HandleEntityDeletionFunc handleEntityDeletion)
{
    if (!SOCKET_UTIL->init())
    {
        return false;
    }
    
    m_entityManager = new EntityManager();
    m_handleEntityDeletion = handleEntityDeletion;
    
    m_socket = SOCKET_UTIL->createUDPSocket(INET);
    SocketAddress ownAddress(INADDR_ANY, inPort);
    m_socket->bindSocket(ownAddress);
    
    LOG("Initializing INetworkManager at port %hu", inPort);
    
    //did we bind okay?
    if (m_socket == nullptr)
    {
        return false;
    }
    
    if (m_socket->setNonBlockingMode(true) != NO_ERROR)
    {
        return false;
    }
    
    m_isInitialized = true;
    
    return true;
}

void INetworkManager::processIncomingPackets()
{
    if (!m_isInitialized)
    {
        return;
    }
    
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
    return *m_bytesReceivedPerSecond;
}

const WeightedTimedMovingAverage& INetworkManager::getBytesSentPerSecond() const
{
    return *m_bytesSentPerSecond;
}

Entity* INetworkManager::getEntity(int inNetworkId) const
{
    return m_entityManager->getEntityFromID(inNetworkId);
}

void INetworkManager::addToNetworkIdToEntityMap(Entity* inEntity)
{
    m_entityManager->registerEntity(inEntity);
}

void INetworkManager::removeFromNetworkIdToEntityMap(Entity* inEntity)
{
    m_handleEntityDeletion(inEntity);
    
    m_entityManager->removeEntity(inEntity);
    
    delete inEntity;
    inEntity = nullptr;
}

void INetworkManager::updateBytesSentLastFrame()
{
    if (m_bytesSentThisFrame > 0)
    {
        m_bytesSentPerSecond->updatePerSecond(static_cast<float>(m_bytesSentThisFrame));
        
        m_bytesSentThisFrame = 0;
    }
}

void INetworkManager::readIncomingPacketsIntoQueue()
{
    //should we just keep a static one?
    static char packetMem[1500];
    bzero(packetMem, 1500);
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
            handleConnectionReset(fromAddress);
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
            m_packetQueue.emplace(simulatedReceivedTime, inputStream, fromAddress);
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

void INetworkManager::processQueuedPackets()
{
    //look at the front packet...
    while (!m_packetQueue.empty())
    {
        ReceivedPacket& nextPacket = m_packetQueue.front();
        if (Timing::getInstance()->getFrameStartTime() > nextPacket.getReceivedTime())
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

INetworkManager::INetworkManager() :
m_socket(nullptr),
m_bytesReceivedPerSecond(new WeightedTimedMovingAverage(1.f)),
m_bytesSentPerSecond(new WeightedTimedMovingAverage(1.f)),
m_bytesSentThisFrame(0),
m_isInitialized(false)
{
    // Empty
}

INetworkManager::~INetworkManager()
{
    if (m_socket)
    {
        delete m_socket;
    }
    
    delete m_bytesReceivedPerSecond;
    delete m_bytesSentPerSecond;
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
