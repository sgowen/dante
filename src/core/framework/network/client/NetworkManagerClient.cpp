//
//  NetworkManagerClient.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NetworkManagerClient.h"

#include "EntityManager.h"
#include "Entity.h"
#include "MoveList.h"
#include "ReplicationManagerClient.h"
#include "WeightedTimedMovingAverage.h"

#include "StringUtil.h"
#include "EntityRegistry.h"
#include "Timing.h"
#include "OutputMemoryBitStream.h"
#include "SocketAddressFactory.h"
#include "FWInstanceManager.h"

#include <assert.h>

NetworkManagerClient* NetworkManagerClient::s_instance = nullptr;

void NetworkManagerClient::create(const std::string& inServerIPAddress, const std::string& inName, float inFrameRate, RemoveProcessedMovesFunc removeProcessedMovesFunc, GetMoveListFunc getMoveListFunc)
{
    assert(!s_instance);
    
    s_instance = new NetworkManagerClient(inServerIPAddress, inName, inFrameRate, removeProcessedMovesFunc, getMoveListFunc);
}

void NetworkManagerClient::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = nullptr;
}

NetworkManagerClient * NetworkManagerClient::getInstance()
{
    return s_instance;
}

void NetworkManagerClient::processPacket(InputMemoryBitStream& inInputStream, SocketAddress* inFromAddress)
{
    uint32_t packetType;
    inInputStream.read(packetType);
    
    switch(packetType)
    {
        case kWelcomeCC:
            handleWelcomePacket(inInputStream);
            break;
        case kStateCC:
            if (m_deliveryNotificationManager.readAndProcessState(inInputStream))
            {
                handleStatePacket(inInputStream);
            }
            break;
    }
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
    return *m_avgRoundTripTime;
}

float NetworkManagerClient::getRoundTripTime() const
{
    return m_avgRoundTripTime->getValue();
}

int NetworkManagerClient::getPlayerId() const
{
    return m_iPlayerId;
}

void NetworkManagerClient::updateSayingHello()
{
    float time = Timing::getInstance()->getFrameStartTime();
    
    static const float kTimeBetweenHellos = 1.0f;
    
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
        m_replicationManagerClient->read(inInputStream);
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
        m_avgRoundTripTime->update(rtt);
        
        m_removeProcessedMovesFunc(m_fLastMoveProcessedByServerTimestamp);
    }
}

void NetworkManagerClient::handleEntityState(InputMemoryBitStream& inInputStream)
{
    EntityManager* entityManager = FWInstanceManager::getClientEntityManager();
    
    // copy the map so that anything that doesn't get an updated can be destroyed...
    std::unordered_map<int, Entity*> objectsToDestroy = entityManager->getMapCopy();
    
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
            auto itGO = entityManager->getMap().find(networkId);
            //didn't find it, better create it!
            if (itGO == entityManager->getMap().end())
            {
                go = FWInstanceManager::getClientEntityRegistry()->createEntity(fourCC);
                go->setID(networkId);
                entityManager->registerEntity(go);
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
    EntityManager* entityManager = FWInstanceManager::getClientEntityManager();
    
    for (auto& pair: inObjectsToDestroy)
    {
        pair.second->requestDeletion();
        
        entityManager->removeEntity(pair.second);
    }
}

void NetworkManagerClient::updateSendingInputPacket()
{
    float time = Timing::getInstance()->getFrameStartTime();
    
    if (time > m_fTimeOfLastInputPacket + m_fFrameRate)
    {
        sendInputPacket();
        m_fTimeOfLastInputPacket = time;
    }
}

void NetworkManagerClient::sendInputPacket()
{
    //only send if there's any input to sent!
    MoveList& moveList = m_getMoveListFunc();
    
    if (moveList.hasMoves())
    {
        OutputMemoryBitStream inputPacket;
        
        inputPacket.write(kInputCC);
        
        m_deliveryNotificationManager.writeState(inputPacket);
        
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

NetworkManagerClient::NetworkManagerClient(const std::string& inServerIPAddress, const std::string& inName, float inFrameRate, RemoveProcessedMovesFunc removeProcessedMovesFunc, GetMoveListFunc getMoveListFunc) :
// This allows us to run both a debug and a release client on the same machine
#if defined(DEBUG) || defined(_DEBUG)
INetworkManager(1339),
#else
INetworkManager(1337),
#endif
m_removeProcessedMovesFunc(removeProcessedMovesFunc),
m_getMoveListFunc(getMoveListFunc),
m_replicationManagerClient(new ReplicationManagerClient()),
m_avgRoundTripTime(new WeightedTimedMovingAverage(1.f)),
m_serverAddress(SocketAddressFactory::createIPv4FromString(inServerIPAddress)),
m_state(NCS_SayingHello),
m_deliveryNotificationManager(DeliveryNotificationManager(true, false)),
m_fTimeOfLastHello(0.0f),
m_fTimeOfLastInputPacket(0.0f),
m_name(inName),
m_fFrameRate(inFrameRate)
{
    // Empty
}

NetworkManagerClient::~NetworkManagerClient()
{
    delete m_replicationManagerClient;
    delete m_serverAddress;
    delete m_avgRoundTripTime;
}
