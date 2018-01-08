//
//  NetworkManagerServer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/network/server/NetworkManagerServer.h"

#include "framework/network/server/ServerHelper.h"
#include "framework/network/portable/InputMemoryBitStream.h"
#include "framework/network/portable/OutputMemoryBitStream.h"
#include "framework/network/portable/DeliveryNotificationManager.h"
#include "framework/network/portable/MachineAddress.h"
#include "framework/network/server/ClientProxy.h"
#include "framework/network/portable/InputState.h"
#include "framework/entity/Entity.h"

#include "framework/util/StringUtil.h"
#include "framework/util/Timing.h"
#include "framework/entity/EntityManager.h"
#include "framework/util/StringUtil.h"
#include "framework/network/portable/FWInstanceManager.h"
#include "framework/util/NGSTDUtil.h"
#include "framework/util/FrameworkConstants.h"
#include "framework/network/client/NetworkManagerClient.h"

#include <assert.h>

NetworkManagerServer* NetworkManagerServer::s_instance = NULL;

void NetworkManagerServer::create(ServerHelper* inServerHelper, HandleNewClientFunc inHandleNewClientFunc, HandleLostClientFunc inHandleLostClientFunc, InputStateCreationFunc inInputStateCreationFunc)
{
    assert(!s_instance);
    
    s_instance = new NetworkManagerServer(inServerHelper, inHandleNewClientFunc, inHandleLostClientFunc, inInputStateCreationFunc);
}

void NetworkManagerServer::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = NULL;
}

NetworkManagerServer * NetworkManagerServer::getInstance()
{
    return s_instance;
}

void NetworkManagerServer::sProcessPacket(InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress)
{
    NG_SERVER->processPacket(inInputStream, inFromAddress);
}

void NetworkManagerServer::sHandleNoResponse()
{
    NG_SERVER->handleNoResponse();
}

void NetworkManagerServer::sHandleConnectionReset(MachineAddress* inFromAddress)
{
    NG_SERVER->handleConnectionReset(inFromAddress);
}

ClientProxy* NetworkManagerServer::sGetClientProxy(uint8_t inPlayerIndex)
{
    return NG_SERVER->getClientProxy(inPlayerIndex + 1);
}

void NetworkManagerServer::sHandleClientDisconnected(ClientProxy* inClientProxy)
{
    NG_SERVER->handleClientDisconnected(inClientProxy);
}

void NetworkManagerServer::processIncomingPackets()
{
    _serverHelper->processIncomingPackets();
}

void NetworkManagerServer::checkForDisconnects()
{
    std::vector<ClientProxy*> clientsToDC;
    
    float minAllowedLastPacketFromClientTime = Timing::getInstance()->getFrameStartTime() - NW_CLIENT_TIMEOUT;
    for (const auto& pair: _addressHashToClientMap)
    {
        if (pair.second->getLastPacketFromClientTime() < minAllowedLastPacketFromClientTime)
        {
            // can't remove from map while in iterator, so just remember for later...
            clientsToDC.push_back(pair.second);
        }
    }
    
    for (ClientProxy* client: clientsToDC)
    {
        handleClientDisconnected(client);
    }
}

void NetworkManagerServer::sendOutgoingPackets()
{
    //let's send a client a state packet whenever their move has come in...
    for (auto it = _addressHashToClientMap.begin(), end = _addressHashToClientMap.end(); it != end; ++it)
    {
        ClientProxy* clientProxy = it->second;
        //process any timed out packets while we're going through the list
        clientProxy->getDeliveryNotificationManager().processTimedOutPackets(Timing::getInstance()->getFrameStartTime());
        
        if (clientProxy->isLastMoveTimestampDirty())
        {
            sendStatePacketToClient(clientProxy);
        }
    }
}

void NetworkManagerServer::registerEntity(Entity* inEntity)
{
    //add mapping from network id to game object
    FWInstanceManager::getServerEntityManager()->registerEntity(inEntity);
    
    //tell all client proxies this is new...
    for (const auto& pair: _addressHashToClientMap)
    {
        pair.second->getReplicationManagerServer().replicateCreate(inEntity->getID(), NG_ALL_STATE);
    }
}

void NetworkManagerServer::deregisterEntity(Entity* inEntity)
{
    uint32_t networkId = inEntity->getID();
    
    FWInstanceManager::getServerEntityManager()->deregisterEntity(inEntity);
    
    //tell all client proxies to STOP replicating!
    //tell all client proxies this is new...
    for (const auto& pair: _addressHashToClientMap)
    {
        pair.second->getReplicationManagerServer().replicateDestroy(networkId);
    }
}

void NetworkManagerServer::setStateDirty(uint32_t inNetworkId, uint32_t inDirtyState)
{
    //tell everybody this is dirty
    for (const auto& pair: _addressHashToClientMap)
    {
        pair.second->getReplicationManagerServer().setStateDirty(inNetworkId, inDirtyState);
    }
}

ClientProxy* NetworkManagerServer::getClientProxy(uint8_t inPlayerId) const
{
    auto it = _playerIDToClientMap.find(inPlayerId);
    if (it != _playerIDToClientMap.end())
    {
        return it->second;
    }
    
    return NULL;
}

int NetworkManagerServer::getAverageMoveCount() const
{
    int ret = 0;
    
    if (_addressHashToClientMap.size() > 0)
    {
        for (auto const &entry : _addressHashToClientMap)
        {
            ret += entry.second->getUnprocessedMoveList().getMoveCount();
        }
        
        ret /= _addressHashToClientMap.size();
    }
    
    return ret;
}

int NetworkManagerServer::getLowestNonHostMoveCount() const
{
    int ret = -1;
    
    if (_addressHashToClientMap.size() > 0)
    {
        for (auto const &entry : _addressHashToClientMap)
        {
            ClientProxy* client = entry.second;
            if (client->getPlayerId() != 1)
            {
                uint32_t moveCount = client->getUnprocessedMoveList().getMoveCount();
                if (moveCount < ret || ret == -1)
                {
                    ret = moveCount;
                }
            }
        }
    }
    
    return ret;
}

int NetworkManagerServer::getHostMoveCount() const
{
    int ret = 0;
    
    ClientProxy* client = getClientProxy(1);
    if (client)
    {
        ret = client->getUnprocessedMoveList().getMoveCount();
    }
    
    return ret;
}

uint8_t NetworkManagerServer::getNumClientsConnected()
{
    return static_cast<uint8_t>(_addressHashToClientMap.size());
}

MachineAddress* NetworkManagerServer::getServerAddress()
{
    return _serverHelper->getServerAddress();
}

bool NetworkManagerServer::isConnected()
{
    return _serverHelper->isConnected();
}

ServerHelper* NetworkManagerServer::getServerHelper()
{
    return _serverHelper;
}

void NetworkManagerServer::processPacket(InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress)
{
    //try to get the client proxy for this address
    //pass this to the client proxy to process
    auto it = _addressHashToClientMap.find(inFromAddress->getHash());
    if (it == _addressHashToClientMap.end())
    {
        if (_playerIDToClientMap.size() < MAX_NUM_PLAYERS_PER_SERVER)
        {
            LOG("New Client with %s", inFromAddress->toString().c_str());
            
            //didn't find one? it's a new cilent..is the a NW_PACKET_TYPE_HELLO? if so, create a client proxy...
            handlePacketFromNewClient(inInputStream, inFromAddress);
        }
        else
        {
            LOG("Server is at max capacity, blocking new client...");
        }
    }
    else
    {
        processPacket((*it).second, inInputStream);
    }
}

void NetworkManagerServer::handleNoResponse()
{
    // Unused
}

void NetworkManagerServer::handleConnectionReset(MachineAddress* inFromAddress)
{
    //just dc the client right away...
    auto it = _addressHashToClientMap.find(inFromAddress->getHash());
    if (it != _addressHashToClientMap.end())
    {
        handleClientDisconnected(it->second);
    }
}

void NetworkManagerServer::sendPacket(const OutputMemoryBitStream& inOutputStream, MachineAddress* inFromAddress)
{
    _serverHelper->sendPacket(inOutputStream, inFromAddress);
}

void NetworkManagerServer::handlePacketFromNewClient(InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress)
{
    // read the beginning- is it a hello?
    uint8_t packetType;
    inInputStream.read(packetType);
    if (packetType == NW_PACKET_TYPE_HELLO)
    {
        // read the name
        std::string name;
        inInputStream.read(name);
        
        if (_addressHashToClientMap.size() == 0)
        {
            if (NG_CLIENT->getPlayerName().compare(name) != 0)
            {
                // The server host MUST be the first client to join
                return;
            }
        }
        
        ClientProxy* newClientProxy = new ClientProxy(inFromAddress, name, _nextPlayerId);
        _addressHashToClientMap[inFromAddress->getHash()] = newClientProxy;
        _playerIDToClientMap[newClientProxy->getPlayerId()] = newClientProxy;
        
        uint8_t playerId = newClientProxy->getPlayerId();
        std::string playerName = newClientProxy->getName();
        
        // tell the server about this client
        _handleNewClientFunc(playerId, playerName);
        
        //and welcome the client...
        sendWelcomePacket(newClientProxy);
        
        // and now init the replication manager with everything we know about!
        for (const auto& pair: FWInstanceManager::getServerEntityManager()->getMap())
        {
            newClientProxy->getReplicationManagerServer().replicateCreate(pair.first, NG_ALL_STATE);
        }
        
        updateNextPlayerId();
    }
    else
    {
        _serverHelper->processSpecialPacket(packetType, inInputStream, inFromAddress);
    }
}

void NetworkManagerServer::processPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream)
{
    //remember we got a packet so we know not to disconnect for a bit
    inClientProxy->updateLastPacketTime();
    
    uint8_t packetType;
    inInputStream.read(packetType);
    
    switch (packetType)
    {
        case NW_PACKET_TYPE_HELLO:
            //need to resend welcome. to be extra safe we should check the name is the one we expect from this address,
            //otherwise something weird is going on...
            sendWelcomePacket(inClientProxy);
            break;
        case NW_PACKET_TYPE_INPUT:
            if (inClientProxy->getDeliveryNotificationManager().readAndProcessState(inInputStream))
            {
                handleInputPacket(inClientProxy, inInputStream);
            }
            break;
        case NW_PACKET_TYPE_ADD_LOCAL_PLAYER:
            handleAddLocalPlayerPacket(inClientProxy, inInputStream);
            break;
        case NW_PACKET_TYPE_DROP_LOCAL_PLAYER:
            handleDropLocalPlayerPacket(inClientProxy, inInputStream);
            break;
        default:
            _serverHelper->processSpecialPacket(packetType, inInputStream, inClientProxy->getMachineAddress());
            break;
    }
}

void NetworkManagerServer::sendWelcomePacket(ClientProxy* inClientProxy)
{
    OutputMemoryBitStream packet;
    
    packet.write(static_cast<uint8_t>(NW_PACKET_TYPE_WELCOME));
    packet.write<uint8_t, 3>(inClientProxy->getPlayerId());
    
    LOG("Server welcoming new client '%s' as player %d", inClientProxy->getName().c_str(), inClientProxy->getPlayerId());
    
    sendPacket(packet, inClientProxy->getMachineAddress());
}

void NetworkManagerServer::sendStatePacketToClient(ClientProxy* inClientProxy)
{
    //build state packet
    OutputMemoryBitStream statePacket;
    
    //it's state!
    statePacket.write(static_cast<uint8_t>(NW_PACKET_TYPE_STATE));
#ifdef NG_LOG
    LOG("Outgoing statePacket Bit Length 1: %d \n", statePacket.getBitLength());
#endif
    
    InFlightPacket* ifp = inClientProxy->getDeliveryNotificationManager().writeState(statePacket);
#ifdef NG_LOG
    LOG("Outgoing statePacket Bit Length 2: %d \n", statePacket.getBitLength());
#endif
    
    writeLastMoveTimestampIfDirty(statePacket, inClientProxy);
#ifdef NG_LOG
    LOG("Outgoing statePacket Bit Length 3: %d \n", statePacket.getBitLength());
#endif
    
    ReplicationManagerTransmissionData* rmtd = _replicationManagerTransmissionDatas.obtain();
    rmtd->reset(&inClientProxy->getReplicationManagerServer(), &_replicationManagerTransmissionDatas);
    
    inClientProxy->getReplicationManagerServer().write(statePacket, rmtd);
    
    TransmissionData* currentTransmissionData = ifp->getTransmissionData('RPLM');
    if (currentTransmissionData)
    {
        _replicationManagerTransmissionDatas.free(static_cast<ReplicationManagerTransmissionData*>(currentTransmissionData));
    }
    
    ifp->setTransmissionData('RPLM', rmtd);
#ifdef NG_LOG
    LOG("Outgoing statePacket Bit Length F: %d \n", statePacket.getBitLength());
#endif
    
    sendPacket(statePacket, inClientProxy->getMachineAddress());
}

void NetworkManagerServer::writeLastMoveTimestampIfDirty(OutputMemoryBitStream& inOutputStream, ClientProxy* inClientProxy)
{
    //first, dirty?
    bool isTimestampDirty = inClientProxy->isLastMoveTimestampDirty();
    inOutputStream.write(isTimestampDirty);
    if (isTimestampDirty)
    {
        float lastProcessedMoveTimestamp = inClientProxy->getUnprocessedMoveList().getLastProcessedMoveTimestamp();
        
        inOutputStream.write(lastProcessedMoveTimestamp);
        
        inClientProxy->setIsLastMoveTimestampDirty(false);
    }
}

void NetworkManagerServer::handleInputPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream)
{
    uint8_t moveCount = 0;
    inInputStream.read<uint8_t, 2>(moveCount);
    
	InputState* referenceInputState = NULL;
	bool isRefInputStateOrphaned = false;
    
    for (; moveCount > 0; --moveCount)
    {
        Move move = Move(_inputStateCreationFunc());
        
        bool isCopy;
        inInputStream.read(isCopy);
        if (isCopy)
        {
            if (!referenceInputState)
            {
                LOG("Unexpected Network State!");
                
                return;
            }
            
            float timeStamp;
            inInputStream.read(timeStamp);
            move.setTimestamp(timeStamp);
            
            move.copyInputState(referenceInputState);
        }
        else
        {
            move.read(inInputStream);
        }

		if (isRefInputStateOrphaned && referenceInputState)
		{
			referenceInputState->setInUse(false);
		}

		referenceInputState = move.getInputState();
        
        if (inClientProxy->getUnprocessedMoveList().addMoveIfNew(move))
        {
			isRefInputStateOrphaned = false;
        }
		else
		{
			isRefInputStateOrphaned = true;
		}
    }
    
    if (isRefInputStateOrphaned && referenceInputState)
    {
        referenceInputState->setInUse(false);
    }
}

void NetworkManagerServer::handleAddLocalPlayerPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream)
{
    if (_playerIDToClientMap.size() < MAX_NUM_PLAYERS_PER_SERVER)
    {
        // read the current number of local players for this client at the time when the request was made
        uint8_t requestedIndex;
        inInputStream.read(requestedIndex);
        
        uint8_t playerId = inClientProxy->getPlayerId(requestedIndex);
        if (playerId == INPUT_UNASSIGNED)
        {
            std::string localPlayerName = StringUtil::format("%s(%d)", inClientProxy->getName().c_str(), requestedIndex);
            
            uint8_t playerId = _nextPlayerId;
            
            inClientProxy->onLocalPlayerAdded(playerId);
            
            _playerIDToClientMap[playerId] = inClientProxy;
            
            // tell the server about this client
            _handleNewClientFunc(playerId, localPlayerName);
            
            updateNextPlayerId();
        }
        
        // and welcome the new local player...
        sendLocalPlayerAddedPacket(inClientProxy, inClientProxy->getNumPlayers() - 1);
    }
    else
    {
        OutputMemoryBitStream packet;
        packet.write(static_cast<uint8_t>(NW_PACKET_TYPE_LOCAL_PLAYER_DENIED));
        
        sendPacket(packet, inClientProxy->getMachineAddress());
    }
}

void NetworkManagerServer::sendLocalPlayerAddedPacket(ClientProxy* inClientProxy, uint8_t index)
{
    uint8_t playerId = inClientProxy->getPlayerId(index);
    
    OutputMemoryBitStream packet;
    
    packet.write(static_cast<uint8_t>(NW_PACKET_TYPE_LOCAL_PLAYER_ADDED));
    packet.write<uint8_t, 3>(playerId);
    
    std::string localPlayerName = StringUtil::format("%s(%d)", inClientProxy->getName().c_str(), index);
    
    LOG("Server welcoming new client local player '%s' as player %d", localPlayerName.c_str(), playerId);
    
    sendPacket(packet, inClientProxy->getMachineAddress());
}

void NetworkManagerServer::handleDropLocalPlayerPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream)
{
    // read the index to drop
    uint8_t index;
    inInputStream.read(index);
    
    if (index < 1)
    {
        // If the primary player on this client wants to drop, a disconnect request should be fired off instead of a drop
        return;
    }
    
    uint8_t playerId = inClientProxy->getPlayerId(index);
    if (playerId != INPUT_UNASSIGNED)
    {
        _playerIDToClientMap.erase(playerId);
        
        _handleLostClientFunc(inClientProxy, index);
        
        inClientProxy->onLocalPlayerRemoved(playerId);
        
        updateNextPlayerId();
    }
}

void NetworkManagerServer::handleClientDisconnected(ClientProxy* inClientProxy)
{
    for (uint8_t i = 0; i < inClientProxy->getNumPlayers(); ++i)
    {
        _playerIDToClientMap.erase(inClientProxy->getPlayerId(i));
    }
    
    _addressHashToClientMap.erase(inClientProxy->getMachineAddress()->getHash());
    
    _handleLostClientFunc(inClientProxy, 0);
    
    _serverHelper->onClientDisconnected(inClientProxy);
    
    delete inClientProxy;
    
    updateNextPlayerId();
}

void NetworkManagerServer::updateNextPlayerId()
{
    LOG("updateNextPlayerId");
    
    // Find the next available Player ID
    _nextPlayerId = 1;
    for (uint8_t i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        for (auto const &entry : _playerIDToClientMap)
        {
            if (entry.first == _nextPlayerId)
            {
                ++_nextPlayerId;
            }
        }
    }
    
    LOG("_nextPlayerId: %d", _nextPlayerId);
}

NetworkManagerServer::NetworkManagerServer(ServerHelper* inServerHelper, HandleNewClientFunc inHandleNewClientFunc, HandleLostClientFunc inHandleLostClientFunc, InputStateCreationFunc inInputStateCreationFunc) :
_serverHelper(inServerHelper),
_handleNewClientFunc(inHandleNewClientFunc),
_handleLostClientFunc(inHandleLostClientFunc),
_inputStateCreationFunc(inInputStateCreationFunc),
_nextPlayerId(1)
{
    // Empty
}

NetworkManagerServer::~NetworkManagerServer()
{
    delete _serverHelper;
    
    NGSTDUtil::cleanUpMapOfPointers(_addressHashToClientMap);
    
    _playerIDToClientMap.clear();
}
