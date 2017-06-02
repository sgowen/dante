//
//  NetworkManagerClient.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NetworkManagerClient.h"

#include "Entity.h"

#include "InputManager.h"
#include "StringUtil.h"
#include "EntityRegistry.h"
#include "Timing.h"
#include "EntityManager.h"
#include "OutputMemoryBitStream.h"

NetworkManagerClient* NetworkManagerClient::getInstance()
{
    static NetworkManagerClient instance = NetworkManagerClient();
    return &instance;
}

void NetworkManagerClient::processPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
    uint32_t packetType;
    inInputStream.read(packetType);
    
    switch(packetType)
    {
        case kWelcomeCC:
            handleWelcomePacket(inInputStream);
            break;
        case kStateCC:
            if (m_deliveryNotificationManager.ReadAndProcessState(inInputStream))
            {
                handleStatePacket(inInputStream);
            }
            break;
    }
}

bool NetworkManagerClient::init(const SocketAddress& inServerAddress, const std::string& inName, HandleEntityDeletion handleEntityDeletion)
{
    m_serverAddress = inServerAddress;
    m_state = NCS_SayingHello;
    m_fTimeOfLastHello = 0.f;
    m_name = inName;
    
    m_avgRoundTripTime = WeightedTimedMovingAverage(1.f);
    
    return INetworkManager::init(9999, handleEntityDeletion);
}

void NetworkManagerClient::sendOutgoingPackets()
{
    switch (m_state)
    {
        case NCS_SayingHello:
            updateSayingHello();
            break;
        case NCS_Welcomed:
            updateSendingInputPacket();
            break;
        case NCS_Uninitialized:
            break;
    }
}

const WeightedTimedMovingAverage& NetworkManagerClient::getAvgRoundTripTime() const
{
    return m_avgRoundTripTime;
}

float NetworkManagerClient::getRoundTripTime() const
{
    return m_avgRoundTripTime.getValue();
}

int NetworkManagerClient::getPlayerId() const
{
    return m_iPlayerId;
}

float NetworkManagerClient::getLastMoveProcessedByServerTimestamp() const
{
    return m_fLastMoveProcessedByServerTimestamp;
}

void NetworkManagerClient::updateSayingHello()
{
    float time = Timing::getInstance()->getTime();
    
    static const float kTimeBetweenHellos = 1.f;
    
    if (time > m_fTimeOfLastHello + kTimeBetweenHellos)
    {
        sendHelloPacket();
        m_fTimeOfLastHello = time;
    }
}

void NetworkManagerClient::sendHelloPacket()
{
    OutputMemoryBitStream helloPacket;
    
    helloPacket.write(kHelloCC);
    helloPacket.write(m_name);
    
    sendPacket(helloPacket, m_serverAddress);
}

void NetworkManagerClient::handleWelcomePacket(InputMemoryBitStream& inInputStream)
{
    if (m_state == NCS_SayingHello)
    {
        //if we got a player id, we've been welcomed!
        int playerId;
        inInputStream.read(playerId);
        m_iPlayerId = playerId;
        m_state = NCS_Welcomed;
        LOG("'%s' was welcomed on client as player %d", m_name.c_str(), m_iPlayerId);
    }
}

void NetworkManagerClient::handleStatePacket(InputMemoryBitStream& inInputStream)
{
    if (m_state == NCS_Welcomed)
    {
        readLastMoveProcessedOnServerTimestamp(inInputStream);
        
        //tell the replication manager to handle the rest...
        m_replicationManagerClient.read(inInputStream);
    }
}

void NetworkManagerClient::readLastMoveProcessedOnServerTimestamp(InputMemoryBitStream& inInputStream)
{
    bool isTimestampDirty;
    inInputStream.read(isTimestampDirty);
    if (isTimestampDirty)
    {
        inInputStream.read(m_fLastMoveProcessedByServerTimestamp);
        
        float rtt = Timing::getInstance()->getFrameStartTime() - m_fLastMoveProcessedByServerTimestamp;
        m_fLastRoundTripTime = rtt;
        m_avgRoundTripTime.update(rtt);
        
        InputManager::getInstance()->getMoveList().removedProcessedMoves(m_fLastMoveProcessedByServerTimestamp);
    }
}

void NetworkManagerClient::handleEntityState(InputMemoryBitStream& inInputStream)
{
    // copy the map so that anything that doesn't get an updated can be destroyed...
    std::unordered_map<int, Entity*> objectsToDestroy = ENTITY_MGR->getMapCopy();
    
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
            Entity* go;
            auto itGO = ENTITY_MGR->getMap().find(networkId);
            //didn't find it, better create it!
            if (itGO == ENTITY_MGR->getMap().end())
            {
                go = EntityRegistry::getInstance()->createEntity(fourCC);
                go->setID(networkId);
                addToNetworkIdToEntityMap(go);
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
    destroyAllInMap(objectsToDestroy);
}

void NetworkManagerClient::destroyAllInMap(const std::unordered_map<int, Entity*>& inObjectsToDestroy)
{
    for (auto& pair: inObjectsToDestroy)
    {
        pair.second->requestDeletion();
        
        removeFromNetworkIdToEntityMap(pair.second);
    }
}

void NetworkManagerClient::updateSendingInputPacket()
{
    float time = Timing::getInstance()->getTime();
    
    static const float kTimeBetweenInputPackets = 0.033f;
    
    if (time > m_fTimeOfLastInputPacket + kTimeBetweenInputPackets)
    {
        sendInputPacket();
        m_fTimeOfLastInputPacket = time;
    }
}

void NetworkManagerClient::sendInputPacket()
{
    //only send if there's any input to sent!
    const MoveList& moveList = InputManager::getInstance()->getMoveList();
    
    if (moveList.hasMoves())
    {
        OutputMemoryBitStream inputPacket;
        
        inputPacket.write(kInputCC);
        
        m_deliveryNotificationManager.WriteState(inputPacket);
        
        //eventually write the 3 latest moves so they have three chances to get through...
        int moveCount = moveList.getMoveCount();
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
        
        sendPacket(inputPacket, m_serverAddress);
    }
}

NetworkManagerClient::NetworkManagerClient() : INetworkManager(), m_state(NCS_Uninitialized), m_deliveryNotificationManager(true, false), m_fLastRoundTripTime(0.f)
{
    // Empty
}

NetworkManagerClient::~NetworkManagerClient()
{
    // Empty
}
