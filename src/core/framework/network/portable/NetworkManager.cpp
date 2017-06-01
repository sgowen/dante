//
//  NetworkManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NetworkManager.h"

#include "SocketUtil.h"
#include "StringUtil.h"
#include "Timing.h"
#include "SocketAddressFamily.h"
#include "World.h"

NetworkManager* NetworkManager::sInstance;

NetworkManager::NetworkManager() :
mBytesSentThisFrame(0)
{
}

NetworkManager::~NetworkManager()
{
}

bool NetworkManager::Init(uint16_t inPort)
{
    mSocket = SocketUtil::CreateUDPSocket(INET);
    SocketAddress ownAddress(INADDR_ANY, inPort);
    mSocket->Bind(ownAddress);
    
    LOG("Initializing NetworkManager at port %d", inPort);
    
    mBytesReceivedPerSecond = WeightedTimedMovingAverage(1.f);
    mBytesSentPerSecond = WeightedTimedMovingAverage(1.f);
    
    //did we bind okay?
    if (mSocket == nullptr)
    {
        return false;
    }
    
    if (mSocket->SetNonBlockingMode(true) != NO_ERROR)
    {
        return false;
    }
    
    return true;
}

void NetworkManager::ProcessIncomingPackets()
{
    ReadIncomingPacketsIntoQueue();
    
    ProcessQueuedPackets();
    
    UpdateBytesSentLastFrame();
}

void NetworkManager::ReadIncomingPacketsIntoQueue()
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
        int readByteCount = mSocket->ReceiveFrom(packetMem, packetSize, fromAddress);
        if (readByteCount == 0)
        {
            //nothing to read
            break;
        }
        else if (readByteCount == -WSAECONNRESET)
        {
            //port closed on other end, so DC this person immediately
            HandleConnectionReset(fromAddress);
        }
        else if (readByteCount > 0)
        {
            inputStream.ResetToCapacity(readByteCount);
            ++receivedPackedCount;
            totalReadByteCount += readByteCount;
            
            //we made it
            //shove the packet into the queue and we'll handle it as soon as we should...
            //we'll pretend it wasn't received until simulated latency from now
            //this doesn't sim jitter, for that we would need to.....
            
            float simulatedReceivedTime = Timing::getInstance()->getTime();
            mPacketQueue.emplace(simulatedReceivedTime, inputStream, fromAddress);
        }
        else
        {
            //uhoh, error? exit or just keep going?
        }
    }
    
    if (totalReadByteCount > 0)
    {
        mBytesReceivedPerSecond.updatePerSecond(static_cast<float>(totalReadByteCount));
    }
}

void NetworkManager::ProcessQueuedPackets()
{
    //look at the front packet...
    while (!mPacketQueue.empty())
    {
        ReceivedPacket& nextPacket = mPacketQueue.front();
        if (Timing::getInstance()->getTime() > nextPacket.GetReceivedTime())
        {
            ProcessPacket(nextPacket.GetPacketBuffer(), nextPacket.GetFromAddress());
            mPacketQueue.pop();
        }
        else
        {
            break;
        }
    }
}

void NetworkManager::SendPacket(const OutputMemoryBitStream& inOutputStream, const SocketAddress& inFromAddress)
{
    int sentByteCount = mSocket->SendTo(inOutputStream.GetBufferPtr(), inOutputStream.GetByteLength(), inFromAddress);
    if (sentByteCount > 0)
    {
        mBytesSentThisFrame += sentByteCount;
    }
}

void NetworkManager::UpdateBytesSentLastFrame()
{
    if (mBytesSentThisFrame > 0)
    {
        mBytesSentPerSecond.updatePerSecond(static_cast<float>(mBytesSentThisFrame));
        
        mBytesSentThisFrame = 0;
    }
}

NetworkManager::ReceivedPacket::ReceivedPacket(float inReceivedTime, InputMemoryBitStream& ioInputMemoryBitStream, const SocketAddress& inFromAddress) :
mReceivedTime(inReceivedTime),
mFromAddress(inFromAddress),
mPacketBuffer(ioInputMemoryBitStream)
{
}

void NetworkManager::AddToNetworkIdToGameObjectMap(GameObject* inGameObject)
{
    m_networkIdToGameObjectMap[inGameObject->GetNetworkId()] = inGameObject;
}

void NetworkManager::RemoveFromNetworkIdToGameObjectMap(GameObject* inGameObject)
{
    World::sInstance->RemoveGameObject(inGameObject);
    
    m_networkIdToGameObjectMap.erase(inGameObject->GetNetworkId());
    
    delete inGameObject;
    inGameObject = nullptr;
}
