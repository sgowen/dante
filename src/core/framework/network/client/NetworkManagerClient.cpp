//
//  NetworkManagerClient.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NetworkManagerClient.h"

#include "InputManager.h"
#include "StringUtil.h"
#include "GameObjectRegistry.h"
#include "Timing.h"
#include "World.h"

namespace
{
    const float kTimeBetweenHellos = 1.f;
    const float kTimeBetweenInputPackets = 0.033f;
}

NetworkManagerClient* NetworkManagerClient::getInstance()
{
    return static_cast<NetworkManagerClient*>(NetworkManager::sInstance);
}

NetworkManagerClient::NetworkManagerClient() :
mState(NCS_Uninitialized),
mDeliveryNotificationManager(true, false),
m_fLastRoundTripTime(0.f)
{
}

void NetworkManagerClient::StaticInit(const SocketAddress& inServerAddress, const std::string& inName)
{
    NetworkManagerClient* ret = new NetworkManagerClient();
    
    ret->Init(inServerAddress, inName);
    
    sInstance = ret;
}

void NetworkManagerClient::Init(const SocketAddress& inServerAddress, const std::string& inName)
{
    NetworkManager::Init(0);
    
    mServerAddress = inServerAddress;
    mState = NCS_SayingHello;
    m_fTimeOfLastHello = 0.f;
    mName = inName;
    
    mAvgRoundTripTime = WeightedTimedMovingAverage(1.f);
}

void NetworkManagerClient::ProcessPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
    uint32_t packetType;
    inInputStream.read(packetType);
    
    switch(packetType)
    {
        case kWelcomeCC:
            HandleWelcomePacket(inInputStream);
            break;
        case kStateCC:
            if (mDeliveryNotificationManager.ReadAndProcessState(inInputStream))
            {
                HandleStatePacket(inInputStream);
            }
            break;
    }
}

void NetworkManagerClient::SendOutgoingPackets()
{
    switch (mState)
    {
        case NCS_SayingHello:
            UpdateSayingHello();
            break;
        case NCS_Welcomed:
            UpdateSendingInputPacket();
            break;
        case NCS_Uninitialized:
            break;
    }
}

void NetworkManagerClient::UpdateSayingHello()
{
    float time = Timing::getInstance()->getTime();
    
    if (time > m_fTimeOfLastHello + kTimeBetweenHellos)
    {
        SendHelloPacket();
        m_fTimeOfLastHello = time;
    }
}

void NetworkManagerClient::SendHelloPacket()
{
    OutputMemoryBitStream helloPacket;
    
    helloPacket.write(kHelloCC);
    helloPacket.write(mName);
    
    SendPacket(helloPacket, mServerAddress);
}

void NetworkManagerClient::HandleWelcomePacket(InputMemoryBitStream& inInputStream)
{
    if (mState == NCS_SayingHello)
    {
        //if we got a player id, we've been welcomed!
        int playerId;
        inInputStream.read(playerId);
        m_iPlayerId = playerId;
        mState = NCS_Welcomed;
        LOG("'%s' was welcomed on client as player %d", mName.c_str(), m_iPlayerId);
    }
}

void NetworkManagerClient::HandleStatePacket(InputMemoryBitStream& inInputStream)
{
    if (mState == NCS_Welcomed)
    {
        ReadLastMoveProcessedOnServerTimestamp(inInputStream);
        
        //tell the replication manager to handle the rest...
        mReplicationManagerClient.read(inInputStream);
    }
}

void NetworkManagerClient::ReadLastMoveProcessedOnServerTimestamp(InputMemoryBitStream& inInputStream)
{
    bool isTimestampDirty;
    inInputStream.read(isTimestampDirty);
    if (isTimestampDirty)
    {
        inInputStream.read(m_fLastMoveProcessedByServerTimestamp);
        
        float rtt = Timing::getInstance()->getFrameStartTime() - m_fLastMoveProcessedByServerTimestamp;
        m_fLastRoundTripTime = rtt;
        mAvgRoundTripTime.update(rtt);
        
        InputManager::getInstance()->getMoveList().RemovedProcessedMoves(m_fLastMoveProcessedByServerTimestamp);
    }
}

void NetworkManagerClient::HandleGameObjectState(InputMemoryBitStream& inInputStream)
{
    //copy the m_networkIdToGameObjectMap so that anything that doesn't get an updated can be destroyed...
    std::unordered_map<int, GameObject*> objectsToDestroy = m_networkIdToGameObjectMap;
    
    int stateCount;
    inInputStream.read(stateCount);
    if (stateCount > 0)
    {
        for (int stateIndex = 0; stateIndex < stateCount; ++stateIndex)
        {
            int networkId;
            uint32_t fourCC;
            
            inInputStream.read(networkId);
            inInputStream.read(fourCC);
            GameObject* go;
            auto itGO = m_networkIdToGameObjectMap.find(networkId);
            //didn't find it, better create it!
            if (itGO == m_networkIdToGameObjectMap.end())
            {
                go = GameObjectRegistry::sInstance->CreateGameObject(fourCC);
                go->SetNetworkId(networkId);
                AddToNetworkIdToGameObjectMap(go);
            }
            else
            {
                //found it
                go = itGO->second;
            }
            
            //now we can update into it
            go->read(inInputStream);
            objectsToDestroy.erase(networkId);
        }
    }
    
    //anything left gets the axe
    DestroyGameObjectsInMap(objectsToDestroy);
}

void NetworkManagerClient::DestroyGameObjectsInMap(const std::unordered_map<int, GameObject*>& inObjectsToDestroy)
{
    for (auto& pair: inObjectsToDestroy)
    {
        pair.second->SetDoesWantToDie(true);
        
        RemoveFromNetworkIdToGameObjectMap(pair.second);    
    }
}

void NetworkManagerClient::UpdateSendingInputPacket()
{
    float time = Timing::getInstance()->getTime();
    
    if (time > m_fTimeOfLastInputPacket + kTimeBetweenInputPackets)
    {
        SendInputPacket();
        m_fTimeOfLastInputPacket = time;
    }
}

void NetworkManagerClient::SendInputPacket()
{
    //only send if there's any input to sent!
    const MoveList& moveList = InputManager::getInstance()->getMoveList();
    
    if (moveList.HasMoves())
    {
        OutputMemoryBitStream inputPacket;
        
        inputPacket.write(kInputCC);
        
        mDeliveryNotificationManager.WriteState(inputPacket);
        
        //eventually write the 3 latest moves so they have three chances to get through...
        int moveCount = moveList.GetMoveCount();
        int firstMoveIndex = moveCount - 3;
        if (firstMoveIndex < 3)
        {
            firstMoveIndex = 0;
        }
        auto move = moveList.begin() + firstMoveIndex;
        
        //only need two bits to write the move count, because it's 0, 1, 2 or 3
        inputPacket.write(moveCount - firstMoveIndex, 2);
        
        for (; firstMoveIndex < moveCount; ++firstMoveIndex, ++move)
        {
            ///would be nice to optimize the time stamp...
            move->write(inputPacket);
        }
        
        SendPacket(inputPacket, mServerAddress);
    }
}
