//
//  NetworkManagerClient.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NetworkManagerClient.h"

#include "SocketClientHelper.h"
#include "InputMemoryBitStream.h"
#include "OutputMemoryBitStream.h"
#include "DeliveryNotificationManager.h"
#include "IMachineAddress.h"
#include "EntityRegistry.h"
#include "Entity.h"
#include "MoveList.h"
#include "ReplicationManagerClient.h"
#include "WeightedTimedMovingAverage.h"
#include "SocketAddress.h"

#include "EntityManager.h"
#include "StringUtil.h"
#include "Timing.h"
#include "SocketAddressFactory.h"
#include "FWInstanceManager.h"
#include "macros.h"

#include <assert.h>

NetworkManagerClient* NetworkManagerClient::s_instance = nullptr;

void NetworkManagerClient::create(IClientHelper* inClientHelper, float inFrameRate, RemoveProcessedMovesFunc inRemoveProcessedMovesFunc, GetMoveListFunc inGetMoveListFunc, OnPlayerWelcomedFunc inOnPlayerWelcomedFunc)
{
    assert(!s_instance);
    
    s_instance = new NetworkManagerClient(inClientHelper, inFrameRate, inRemoveProcessedMovesFunc, inGetMoveListFunc, inOnPlayerWelcomedFunc);
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

void NetworkManagerClient::sProcessPacket(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress)
{
    NG_CLIENT->processPacket(inInputStream, static_cast<SocketAddress*>(inFromAddress));
}

void NetworkManagerClient::sHandleNoResponse()
{
    NG_CLIENT->handleNoResponse();
}

void NetworkManagerClient::sHandleConnectionReset(IMachineAddress* inFromAddress)
{
    NG_CLIENT->handleConnectionReset(static_cast<SocketAddress*>(inFromAddress));
}

void NetworkManagerClient::processIncomingPackets()
{
    m_clientHelper->processIncomingPackets();
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
            updateAddLocalPlayerRequest();
            updateDropLocalPlayerRequest();
            break;
        case NCS_Uninitialized:
            m_clientHelper->handleUninitialized();
            break;
        case NCS_Disconnected:
            break;
    }
    
    if (m_state != NCS_Disconnected)
    {
        int clientHelperState = m_clientHelper->getState();
        if (clientHelperState == CLIENT_READY_TO_SAY_HELLO
            && m_state != NCS_Welcomed)
        {
            m_state = NCS_SayingHello;
        }
        else if (clientHelperState == CLIENT_AUTH_FAILED)
        {
            m_state = NCS_Disconnected;
        }
    }
}

void NetworkManagerClient::requestToAddLocalPlayer()
{
    m_isRequestingToAddLocalPlayer = true;
    
    m_isRequestingToDropLocalPlayer = 0;
}

void NetworkManagerClient::requestToDropLocalPlayer(int index)
{
    if (index < 1)
    {
        return;
    }
    
    m_isRequestingToDropLocalPlayer = index;
    
    m_isRequestingToAddLocalPlayer = false;
    
    m_indexToPlayerIdMap.erase(index);
    
    updateNextIndex();
}

const WeightedTimedMovingAverage& NetworkManagerClient::getBytesReceivedPerSecond() const
{
    return m_clientHelper->getBytesReceivedPerSecond();
}

const WeightedTimedMovingAverage& NetworkManagerClient::getBytesSentPerSecond() const
{
    return m_clientHelper->getBytesSentPerSecond();
}

const WeightedTimedMovingAverage& NetworkManagerClient::getAvgRoundTripTime() const
{
    return *m_avgRoundTripTime;
}

float NetworkManagerClient::getRoundTripTime() const
{
    return m_avgRoundTripTime->getValue();
}

bool NetworkManagerClient::isPlayerIdLocal(uint8_t inPlayerId) const
{
    for (auto const &entry : m_indexToPlayerIdMap)
    {
        if (entry.second == inPlayerId)
        {
            return true;
        }
    }
    
    return false;
}

std::unordered_map<uint8_t, uint8_t>& NetworkManagerClient::getPlayerIds()
{
    return m_indexToPlayerIdMap;
}

std::string& NetworkManagerClient::getPlayerName()
{
    return m_clientHelper->getName();
}

NetworkClientState NetworkManagerClient::getState() const
{
    return m_state;
}

void NetworkManagerClient::processPacket(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress)
{
    m_fLastServerCommunicationTimestamp = Timing::getInstance()->getFrameStartTime();
    
    uint32_t packetType;
    inInputStream.read(packetType);
    
    switch (packetType)
    {
        case NW_PACKET_TYPE_WELCOME:
            handleWelcomePacket(inInputStream);
            break;
        case NW_PACKET_TYPE_LOCAL_PLAYER_ADDED:
            handleLocalPlayerAddedPacket(inInputStream);
            break;
        case NW_PACKET_TYPE_LOCAL_PLAYER_DENIED:
            handleLocalPlayerDeniedPacket();
            break;
        case NW_PACKET_TYPE_STATE:
            if (m_deliveryNotificationManager->readAndProcessState(inInputStream))
            {
                handleStatePacket(inInputStream);
            }
            break;
        default:
            m_clientHelper->processSpecialPacket(packetType, inInputStream, inFromAddress);
            break;
    }
}

void NetworkManagerClient::handleNoResponse()
{
    float time = Timing::getInstance()->getFrameStartTime();
    
    float timeout = m_state == NCS_Uninitialized ? NW_CONNECT_TO_SERVER_TIMEOUT : NW_SERVER_TIMEOUT;
    if (time > m_fLastServerCommunicationTimestamp + timeout)
    {
        m_state = NCS_Disconnected;
    }
}

void NetworkManagerClient::handleConnectionReset(IMachineAddress* inFromAddress)
{
    UNUSED(inFromAddress);
}

void NetworkManagerClient::sendPacket(const OutputMemoryBitStream& inOutputStream)
{
    m_clientHelper->sendPacket(inOutputStream);
}

void NetworkManagerClient::updateSayingHello()
{
    float time = Timing::getInstance()->getFrameStartTime();
    
    if (time > m_fTimeOfLastHello + NW_CLIENT_TIME_BETWEEN_HELLOS)
    {
        OutputMemoryBitStream helloPacket;
        
        helloPacket.write(NW_PACKET_TYPE_HELLO);
        helloPacket.write(getPlayerName());
        
        sendPacket(helloPacket);
        
        m_fTimeOfLastHello = time;
    }
}

void NetworkManagerClient::handleWelcomePacket(InputMemoryBitStream& inInputStream)
{
    if (m_state == NCS_SayingHello)
    {
        // if we got a player id, we've been welcomed!
        uint8_t playerId;
        inInputStream.read(playerId);
        
        m_state = NCS_Welcomed;
        
        m_indexToPlayerIdMap.clear();
        m_indexToPlayerIdMap[m_iNextIndex] = playerId;
        
        LOG("'%s' was welcomed on client as player %d", getPlayerName().c_str(), playerId);
        
        m_onPlayerWelcomedFunc(playerId);
        
        updateNextIndex();
    }
}

void NetworkManagerClient::handleLocalPlayerAddedPacket(InputMemoryBitStream& inInputStream)
{
    if (m_state == NCS_Welcomed
        && m_isRequestingToAddLocalPlayer)
    {
        // if we got a player id, our local player has been added!
        uint8_t playerId;
        inInputStream.read(playerId);
        
        m_indexToPlayerIdMap[m_iNextIndex] = playerId;
        
        LOG("'%s(%d)' was welcomed on client as player %d", getPlayerName().c_str(), m_iNextIndex, playerId);
        
        m_isRequestingToAddLocalPlayer = false;
        
        m_onPlayerWelcomedFunc(playerId);
        
        updateNextIndex();
    }
}

void NetworkManagerClient::handleLocalPlayerDeniedPacket()
{
    LOG("'%s(%d)' was denied on client...", getPlayerName().c_str(), m_iNextIndex);
    
    m_isRequestingToAddLocalPlayer = false;
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
        inInputStream.read(m_fLastMoveReceivedByServerTimestamp);
        
        float rtt = Timing::getInstance()->getFrameStartTime() - m_fLastMoveReceivedByServerTimestamp;
        m_avgRoundTripTime->update(rtt);
        
        m_removeProcessedMovesFunc(m_fLastMoveProcessedByServerTimestamp);
    }
}

void NetworkManagerClient::updateSendingInputPacket()
{
    sendInputPacket();
}

void NetworkManagerClient::sendInputPacket()
{
    //only send if there's any input to sent!
    MoveList& moveList = m_getMoveListFunc();
    
    if (moveList.hasMoves())
    {
        OutputMemoryBitStream inputPacket;
        
        inputPacket.write(NW_PACKET_TYPE_INPUT);
        
        m_deliveryNotificationManager->writeState(inputPacket);
        
        // eventually write the 31 latest moves so they have 3 chances to get through...
        int moveCount = moveList.getNumMovesAfterTimestamp(m_fLastMoveReceivedByServerTimestamp);
        int firstMoveIndex = moveCount - 31;
        if (firstMoveIndex < 0)
        {
            firstMoveIndex = 0;
        }
        else if (firstMoveIndex > 0)
        {
            LOG("Skipping some moves");
        }
        
        auto move = moveList.begin() + firstMoveIndex;
        
        // only need 2 bits to write the move count, because it's 0-31
        inputPacket.write(moveCount - firstMoveIndex, 5);
        
        Move* referenceMove = nullptr;
        for (; firstMoveIndex < moveCount; ++firstMoveIndex, ++move)
        {
            bool needsToWriteMove = true;
            
            if (referenceMove)
            {
                if (move->isEqual(referenceMove))
                {
                    inputPacket.write(true);
                    
                    inputPacket.write(move->getTimestamp());
                    
                    needsToWriteMove = false;
                }
            }
            
            if (needsToWriteMove)
            {
                inputPacket.write(false);
                
                move->write(inputPacket);
                
                referenceMove = moveList.getMoveAtIndex(firstMoveIndex);
            }
        }
        
        sendPacket(inputPacket);
    }
}

void NetworkManagerClient::updateAddLocalPlayerRequest()
{
    if (m_isRequestingToAddLocalPlayer)
    {
        m_isRequestingToDropLocalPlayer = 0;
        
        float time = Timing::getInstance()->getFrameStartTime();
        
        if (time > m_fTimeOfLastHello + NW_CLIENT_TIME_BETWEEN_HELLOS)
        {
            OutputMemoryBitStream packet;
            
            packet.write(NW_PACKET_TYPE_ADD_LOCAL_PLAYER);
            packet.write(m_iNextIndex);
            
            sendPacket(packet);
            
            m_fTimeOfLastHello = time;
        }
    }
}

void NetworkManagerClient::updateDropLocalPlayerRequest()
{
    if (m_isRequestingToDropLocalPlayer > 0)
    {
        m_isRequestingToAddLocalPlayer = false;
        
        OutputMemoryBitStream packet;
        
        packet.write(NW_PACKET_TYPE_DROP_LOCAL_PLAYER);
        packet.write(m_isRequestingToDropLocalPlayer);
        
        sendPacket(packet);
        
        m_isRequestingToDropLocalPlayer = 0;
    }
}

void NetworkManagerClient::updateNextIndex()
{
    // Find the next available Player ID
    m_iNextIndex = 0;
    for (auto const &entry : m_indexToPlayerIdMap)
    {
        if (entry.first == m_iNextIndex)
        {
            ++m_iNextIndex;
        }
    }
}

NetworkManagerClient::NetworkManagerClient(IClientHelper* inClientHelper, float inFrameRate, RemoveProcessedMovesFunc inRemoveProcessedMovesFunc, GetMoveListFunc inGetMoveListFunc, OnPlayerWelcomedFunc inOnPlayerWelcomedFunc) :
m_clientHelper(inClientHelper),
m_removeProcessedMovesFunc(inRemoveProcessedMovesFunc),
m_getMoveListFunc(inGetMoveListFunc),
m_onPlayerWelcomedFunc(inOnPlayerWelcomedFunc),
m_replicationManagerClient(new ReplicationManagerClient()),
m_avgRoundTripTime(new WeightedTimedMovingAverage(1.f)),
m_state(NCS_Uninitialized),
m_deliveryNotificationManager(new DeliveryNotificationManager(true, false)),
m_fTimeOfLastHello(0.0f),
m_fLastMoveProcessedByServerTimestamp(0.0f),
m_fLastMoveReceivedByServerTimestamp(0.0f),
m_fLastServerCommunicationTimestamp(Timing::getInstance()->getFrameStartTime()),
m_fFrameRate(inFrameRate),
m_isRequestingToAddLocalPlayer(false),
m_isRequestingToDropLocalPlayer(0),
m_iNextIndex(0)
{
    // Empty
}

NetworkManagerClient::~NetworkManagerClient()
{
    delete m_clientHelper;
    delete m_deliveryNotificationManager;
    delete m_replicationManagerClient;
    delete m_avgRoundTripTime;
}
