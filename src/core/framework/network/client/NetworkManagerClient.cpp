//
//  NetworkManagerClient.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/network/client/NetworkManagerClient.h"

#include "framework/network/client/SocketClientHelper.h"
#include "framework/network/portable/InputMemoryBitStream.h"
#include "framework/network/portable/OutputMemoryBitStream.h"
#include "framework/network/portable/DeliveryNotificationManager.h"
#include "framework/network/portable/MachineAddress.h"
#include "framework/entity/EntityRegistry.h"
#include "framework/entity/Entity.h"
#include "framework/network/portable/MoveList.h"
#include "framework/network/client/ReplicationManagerClient.h"
#include "framework/util/WeightedTimedMovingAverage.h"
#include "framework/network/portable/SocketAddress.h"

#include "framework/entity/EntityManager.h"
#include "framework/util/StringUtil.h"
#include "framework/util/Timing.h"
#include "framework/network/portable/SocketAddressFactory.h"
#include "framework/network/portable/FWInstanceManager.h"
#include "framework/util/macros.h"

#include <assert.h>

NetworkManagerClient* NetworkManagerClient::s_instance = NULL;

void NetworkManagerClient::create(ClientHelper* inClientHelper, RemoveProcessedMovesFunc inRemoveProcessedMovesFunc, GetMoveListFunc inGetMoveListFunc, OnPlayerWelcomedFunc inOnPlayerWelcomedFunc)
{
    assert(!s_instance);
    
    s_instance = new NetworkManagerClient(inClientHelper, inRemoveProcessedMovesFunc, inGetMoveListFunc, inOnPlayerWelcomedFunc);
}

NetworkManagerClient * NetworkManagerClient::getInstance()
{
    return s_instance;
}

void NetworkManagerClient::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = NULL;
}

void NetworkManagerClient::sProcessPacket(InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress)
{
    NG_CLIENT->processPacket(inInputStream, static_cast<SocketAddress*>(inFromAddress));
}

void NetworkManagerClient::sHandleNoResponse()
{
    NG_CLIENT->handleNoResponse();
}

void NetworkManagerClient::sHandleConnectionReset(MachineAddress* inFromAddress)
{
    NG_CLIENT->handleConnectionReset(static_cast<SocketAddress*>(inFromAddress));
}

void NetworkManagerClient::processIncomingPackets()
{
    _clientHelper->processIncomingPackets();
}

void NetworkManagerClient::sendOutgoingPackets()
{
    _hasReceivedNewState = false;
    
    switch (_state)
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
            _clientHelper->handleUninitialized();
            break;
        case NCS_Disconnected:
            break;
    }
    
    if (_state != NCS_Disconnected)
    {
        int clientHelperState = _clientHelper->getState();
        if (clientHelperState == CLIENT_READY_TO_SAY_HELLO
            && _state != NCS_Welcomed)
        {
            _state = NCS_SayingHello;
        }
        else if (clientHelperState == CLIENT_AUTH_FAILED)
        {
            _state = NCS_Disconnected;
        }
    }
}

void NetworkManagerClient::requestToAddLocalPlayer()
{
    _isRequestingToAddLocalPlayer = true;
    
    _isRequestingToDropLocalPlayer = 0;
}

void NetworkManagerClient::requestToDropLocalPlayer(uint8_t index)
{
    if (index < 1)
    {
        return;
    }
    
    _isRequestingToDropLocalPlayer = index;
    
    _isRequestingToAddLocalPlayer = false;
    
    _indexToPlayerIdMap.erase(index);
    
    updateNextIndex();
}

const WeightedTimedMovingAverage& NetworkManagerClient::getBytesReceivedPerSecond() const
{
    return _clientHelper->getBytesReceivedPerSecond();
}

const WeightedTimedMovingAverage& NetworkManagerClient::getBytesSentPerSecond() const
{
    return _clientHelper->getBytesSentPerSecond();
}

const WeightedTimedMovingAverage& NetworkManagerClient::getAvgRoundTripTime() const
{
    return *_avgRoundTripTime;
}

float NetworkManagerClient::getRoundTripTime() const
{
    return _avgRoundTripTime->getValue();
}

bool NetworkManagerClient::isPlayerIdLocal(uint8_t inPlayerId) const
{
    for (auto const &entry : _indexToPlayerIdMap)
    {
        if (entry.second == inPlayerId)
        {
            return true;
        }
    }
    
    return false;
}

bool NetworkManagerClient::hasReceivedNewState()
{
    return _hasReceivedNewState;
}

std::map<uint8_t, uint8_t>& NetworkManagerClient::getPlayerIds()
{
    return _indexToPlayerIdMap;
}

std::string& NetworkManagerClient::getPlayerName()
{
    return _clientHelper->getName();
}

NetworkClientState NetworkManagerClient::getState() const
{
    return _state;
}

void NetworkManagerClient::processPacket(InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress)
{
    _lastServerCommunicationTimestamp = Timing::getInstance()->getFrameStartTime();
    
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
            if (_deliveryNotificationManager->readAndProcessState(inInputStream))
            {
                handleStatePacket(inInputStream);
            }
            break;
        default:
            _clientHelper->processSpecialPacket(packetType, inInputStream, inFromAddress);
            break;
    }
}

void NetworkManagerClient::handleNoResponse()
{
    float time = Timing::getInstance()->getFrameStartTime();
    
    float timeout = _state == NCS_Uninitialized ? NW_CONNECT_TO_SERVER_TIMEOUT : NW_SERVER_TIMEOUT;
    if (time > _lastServerCommunicationTimestamp + timeout)
    {
        _state = NCS_Disconnected;
    }
}

void NetworkManagerClient::handleConnectionReset(MachineAddress* inFromAddress)
{
    UNUSED(inFromAddress);
}

void NetworkManagerClient::sendPacket(const OutputMemoryBitStream& inOutputStream)
{
    _clientHelper->sendPacket(inOutputStream);
}

void NetworkManagerClient::updateSayingHello()
{
    float time = Timing::getInstance()->getFrameStartTime();
    
    if (time > _timeOfLastHello + NW_CLIENT_TIME_BETWEEN_HELLOS)
    {
        OutputMemoryBitStream helloPacket;
        
        helloPacket.write(NW_PACKET_TYPE_HELLO);
        helloPacket.write(getPlayerName());
        
        sendPacket(helloPacket);
        
        _timeOfLastHello = time;
    }
}

void NetworkManagerClient::handleWelcomePacket(InputMemoryBitStream& inInputStream)
{
    if (_state == NCS_SayingHello)
    {
        // if we got a player id, we've been welcomed!
        uint8_t playerId;
        inInputStream.read(playerId);
        
        _state = NCS_Welcomed;
        
        _indexToPlayerIdMap.clear();
        _indexToPlayerIdMap[_nextIndex] = playerId;
        
        LOG("'%s' was welcomed on client as player %d", getPlayerName().c_str(), playerId);
        
        _onPlayerWelcomedFunc(playerId);
        
        updateNextIndex();
    }
}

void NetworkManagerClient::handleLocalPlayerAddedPacket(InputMemoryBitStream& inInputStream)
{
    if (_state == NCS_Welcomed
        && _isRequestingToAddLocalPlayer)
    {
        // if we got a player id, our local player has been added!
        uint8_t playerId;
        inInputStream.read(playerId);
        
        _indexToPlayerIdMap[_nextIndex] = playerId;
        
        LOG("'%s(%d)' was welcomed on client as player %d", getPlayerName().c_str(), _nextIndex, playerId);
        
        _isRequestingToAddLocalPlayer = false;
        
        _onPlayerWelcomedFunc(playerId);
        
        updateNextIndex();
    }
}

void NetworkManagerClient::handleLocalPlayerDeniedPacket()
{
    LOG("'%s(%d)' was denied on client...", getPlayerName().c_str(), _nextIndex);
    
    _isRequestingToAddLocalPlayer = false;
}

void NetworkManagerClient::handleStatePacket(InputMemoryBitStream& inInputStream)
{
    if (_state == NCS_Welcomed)
    {
        readLastMoveProcessedOnServerTimestamp(inInputStream);
        
        //tell the replication manager to handle the rest...
        _replicationManagerClient->read(inInputStream);
        
        _hasReceivedNewState = true;
    }
}

void NetworkManagerClient::readLastMoveProcessedOnServerTimestamp(InputMemoryBitStream& inInputStream)
{
    bool isTimestampDirty;
    inInputStream.read(isTimestampDirty);
    if (isTimestampDirty)
    {
        inInputStream.read(_lastMoveProcessedByServerTimestamp);
        
        float rtt = Timing::getInstance()->getFrameStartTime() - _lastMoveProcessedByServerTimestamp;
        _avgRoundTripTime->update(rtt);
        
        _removeProcessedMovesFunc(_lastMoveProcessedByServerTimestamp);
    }
}

void NetworkManagerClient::updateSendingInputPacket()
{
    sendInputPacket();
}

void NetworkManagerClient::sendInputPacket()
{
    //only send if there's any input to sent!
    MoveList& moveList = _getMoveListFunc();
    
    if (moveList.hasMoves())
    {
        OutputMemoryBitStream inputPacket;
        
        inputPacket.write(NW_PACKET_TYPE_INPUT);
        
        _deliveryNotificationManager->writeState(inputPacket);
        
        // eventually write the 3 latest moves so they have 3 chances to get through...
        int moveCount = moveList.getNumMovesAfterTimestamp(_lastMoveProcessedByServerTimestamp);
        int firstMoveIndex = moveCount - 3;
        if (firstMoveIndex < 0)
        {
            firstMoveIndex = 0;
        }
        
        std::deque<Move>::const_iterator move = moveList.begin() + firstMoveIndex;
        
        // only need 2 bits to write the move count, because it's 0-3
        inputPacket.write<uint8_t, 2>(moveCount - firstMoveIndex);
        
        const Move* referenceMove = NULL;
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
                
                referenceMove = &(*move);
            }
        }
        
        sendPacket(inputPacket);
    }
}

void NetworkManagerClient::updateAddLocalPlayerRequest()
{
    if (_isRequestingToAddLocalPlayer)
    {
        _isRequestingToDropLocalPlayer = 0;
        
        float time = Timing::getInstance()->getFrameStartTime();
        
        if (time > _timeOfLastHello + NW_CLIENT_TIME_BETWEEN_HELLOS)
        {
            OutputMemoryBitStream packet;
            
            packet.write(NW_PACKET_TYPE_ADD_LOCAL_PLAYER);
            packet.write(_nextIndex);
            
            sendPacket(packet);
            
            _timeOfLastHello = time;
        }
    }
}

void NetworkManagerClient::updateDropLocalPlayerRequest()
{
    if (_isRequestingToDropLocalPlayer > 0)
    {
        _isRequestingToAddLocalPlayer = false;
        
        OutputMemoryBitStream packet;
        
        packet.write(NW_PACKET_TYPE_DROP_LOCAL_PLAYER);
        packet.write(_isRequestingToDropLocalPlayer);
        
        sendPacket(packet);
        
        _isRequestingToDropLocalPlayer = 0;
    }
}

void NetworkManagerClient::updateNextIndex()
{
    // Find the next available Player ID
    _nextIndex = 0;
    for (auto const &entry : _indexToPlayerIdMap)
    {
        if (entry.first == _nextIndex)
        {
            ++_nextIndex;
        }
    }
}

NetworkManagerClient::NetworkManagerClient(ClientHelper* inClientHelper, RemoveProcessedMovesFunc inRemoveProcessedMovesFunc, GetMoveListFunc inGetMoveListFunc, OnPlayerWelcomedFunc inOnPlayerWelcomedFunc) :
_clientHelper(inClientHelper),
_removeProcessedMovesFunc(inRemoveProcessedMovesFunc),
_getMoveListFunc(inGetMoveListFunc),
_onPlayerWelcomedFunc(inOnPlayerWelcomedFunc),
_replicationManagerClient(new ReplicationManagerClient()),
_avgRoundTripTime(new WeightedTimedMovingAverage(1.f)),
_state(NCS_Uninitialized),
_deliveryNotificationManager(new DeliveryNotificationManager(true, false)),
_timeOfLastHello(0.0f),
_lastMoveProcessedByServerTimestamp(0.0f),
_lastServerCommunicationTimestamp(Timing::getInstance()->getFrameStartTime()),
_isRequestingToAddLocalPlayer(false),
_isRequestingToDropLocalPlayer(0),
_nextIndex(0),
_hasReceivedNewState(false)
{
    // Empty
}

NetworkManagerClient::~NetworkManagerClient()
{
    delete _clientHelper;
    delete _deliveryNotificationManager;
    delete _replicationManagerClient;
    delete _avgRoundTripTime;
}
