//
//  NetworkManagerServer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/server/NetworkManagerServer.h>

#include <framework/network/server/ServerHelper.h>
#include <framework/network/portable/InputMemoryBitStream.h>
#include <framework/network/portable/OutputMemoryBitStream.h>
#include <framework/network/portable/DeliveryNotificationManager.h>
#include <framework/network/portable/MachineAddress.h>
#include <framework/network/server/ClientProxy.h>
#include <framework/network/portable/InputState.h>
#include <framework/entity/Entity.h>
#include <framework/util/Timing.h>

#include <framework/util/StringUtil.h>
#include <framework/entity/EntityManager.h>
#include <framework/util/StringUtil.h>
#include <framework/util/NGSTDUtil.h>
#include <framework/util/Constants.h>
#include <framework/network/client/NetworkManagerClient.h>
#include <framework/network/server/ReplicationManagerServer.h>
#include <framework/util/InstanceManager.h>

#include <assert.h>

NetworkManagerServer* NetworkManagerServer::s_instance = NULL;

void NetworkManagerServer::create(ServerHelper* inServerHelper, HandleEntityCreatedFunc handleEntityCreatedFunc, HandleEntityDeletionFunc handleEntityDeletionFunc, HandleNewClientFunc handleNewClientFunc, HandleLostClientFunc handleLostClientFunc, InputStateCreationFunc inputStateCreationFunc, InputStateReleaseFunc inputStateReleaseFunc)
{
    assert(!s_instance);
    
    s_instance = new NetworkManagerServer(inServerHelper, handleEntityCreatedFunc, handleEntityDeletionFunc, handleNewClientFunc, handleLostClientFunc, inputStateCreationFunc, inputStateReleaseFunc);
}

NetworkManagerServer * NetworkManagerServer::getInstance()
{
    return s_instance;
}

void NetworkManagerServer::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = NULL;
}

void NetworkManagerServer::sProcessPacket(InputMemoryBitStream& inputStream, MachineAddress* inFromAddress)
{
    NG_SERVER->processPacket(inputStream, inFromAddress);
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
    
    /// Check for disconnects
    std::vector<ClientProxy*> clientsToDC;
    
    float minAllowedLastPacketFromClientTime = _timing->getTime() - NW_CLIENT_TIMEOUT;
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
        clientProxy->getDeliveryNotificationManager().processTimedOutPackets(_timing->getTime());
        
        if (clientProxy->isLastMoveTimestampDirty())
        {
            sendStatePacketToClient(clientProxy);
        }
    }
}

void NetworkManagerServer::registerEntity(Entity* inEntity)
{
    //add mapping from network id to game object
    _entityManager->registerEntity(inEntity);
    
    //tell all client proxies this is new...
    for (const auto& pair: _addressHashToClientMap)
    {
        pair.second->getReplicationManagerServer()->replicateCreate(inEntity->getID(), NG_ALL_STATE);
    }
}

void NetworkManagerServer::deregisterEntity(Entity* inEntity)
{
    uint32_t networkId = inEntity->getID();
    
    _entityManager->deregisterEntity(inEntity);
    
    //tell all client proxies to STOP replicating!
    //tell all client proxies this is new...
    for (const auto& pair: _addressHashToClientMap)
    {
        pair.second->getReplicationManagerServer()->replicateDestroy(networkId);
    }
}

void NetworkManagerServer::setStateDirty(uint32_t inNetworkId, uint16_t inDirtyState)
{
    //tell everybody this is dirty
    for (const auto& pair: _addressHashToClientMap)
    {
        pair.second->getReplicationManagerServer()->setStateDirty(inNetworkId, inDirtyState);
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

int NetworkManagerServer::getMoveCount()
{
    int ret = 0;
    
    int lowestNonHostMoveCount = getLowestNonHostMoveCount();
    int hostMoveCount = getHostMoveCount();
    
    if (lowestNonHostMoveCount == -1
        || (hostMoveCount <= lowestNonHostMoveCount
            && (hostMoveCount * 2) >= lowestNonHostMoveCount))
    {
        ret = hostMoveCount;
    }
    else if (lowestNonHostMoveCount <= hostMoveCount
             && (lowestNonHostMoveCount * 2) >= hostMoveCount)
    {
        ret = lowestNonHostMoveCount;
    }
    else if (lowestNonHostMoveCount >= 8 || hostMoveCount >= 8)
    {
        ret = getAverageMoveCount();
        
#ifdef NG_LOG
        LOG("lowestNonHostMoveCount: %d, hostMoveCount: %d, finalMoveCount(avg): %d", lowestNonHostMoveCount, hostMoveCount, ret);
#endif
    }
    
    return ret;
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

void NetworkManagerServer::setMap(uint32_t inValue)
{
    _map = inValue;
}

EntityManager* NetworkManagerServer::getEntityManager()
{
    return _entityManager;
}

void NetworkManagerServer::processPacket(InputMemoryBitStream& inputStream, MachineAddress* inFromAddress)
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
            handlePacketFromNewClient(inputStream, inFromAddress);
        }
        else
        {
            LOG("Server is at max capacity, blocking new client...");
        }
    }
    else
    {
        processPacket((*it).second, inputStream);
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

void NetworkManagerServer::handlePacketFromNewClient(InputMemoryBitStream& inputStream, MachineAddress* inFromAddress)
{
    // read the beginning- is it a hello?
    uint8_t packetType;
    inputStream.read(packetType);
    if (packetType == NW_PACKET_TYPE_HELLO)
    {
        // read the name
        std::string name;
        inputStream.readSmall(name);
        
        if (_addressHashToClientMap.size() == 0)
        {
            if (NG_CLIENT->getPlayerName().compare(name) != 0)
            {
                // The server host MUST be the first client to join
                return;
            }
        }
        
        ClientProxy* newClientProxy = new ClientProxy(_entityManager, inFromAddress, name, _nextPlayerId);
        _addressHashToClientMap[inFromAddress->getHash()] = newClientProxy;
        _playerIDToClientMap[newClientProxy->getPlayerId()] = newClientProxy;
        
        uint8_t playerId = newClientProxy->getPlayerId();
        std::string playerName = newClientProxy->getName();
        
        // tell the server about this client
        _handleNewClientFunc(playerId, playerName);
        
        //and welcome the client...
        sendWelcomePacket(newClientProxy);
        
        // and now init the replication manager with everything we know about!
        for (const auto& pair: _entityManager->getMap())
        {
            newClientProxy->getReplicationManagerServer()->replicateCreate(pair.first, NG_ALL_STATE);
        }
        
        updateNextPlayerId();
    }
    else
    {
        _serverHelper->processSpecialPacket(packetType, inputStream, inFromAddress);
    }
}

void NetworkManagerServer::processPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inputStream)
{
    //remember we got a packet so we know not to disconnect for a bit
    inClientProxy->updateLastPacketTime();
    
    uint8_t packetType;
    inputStream.read(packetType);
    
    switch (packetType)
    {
        case NW_PACKET_TYPE_HELLO:
            //need to resend welcome. to be extra safe we should check the name is the one we expect from this address,
            //otherwise something weird is going on...
            sendWelcomePacket(inClientProxy);
            break;
        case NW_PACKET_TYPE_INPUT:
            if (inClientProxy->getDeliveryNotificationManager().readAndProcessState(inputStream))
            {
                handleInputPacket(inClientProxy, inputStream);
            }
            break;
        case NW_PACKET_TYPE_ADD_LOCAL_PLAYER:
            handleAddLocalPlayerPacket(inClientProxy, inputStream);
            break;
        case NW_PACKET_TYPE_DROP_LOCAL_PLAYER:
            handleDropLocalPlayerPacket(inClientProxy, inputStream);
            break;
        default:
            _serverHelper->processSpecialPacket(packetType, inputStream, inClientProxy->getMachineAddress());
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
    
    InFlightPacket* ifp = inClientProxy->getDeliveryNotificationManager().writeState(statePacket);
    
    writeLastMoveTimestampIfDirty(statePacket, inClientProxy);
    
    ReplicationManagerTransmissionData* rmtd = _replicationManagerTransmissionDatas.obtain();
    rmtd->reset(inClientProxy->getReplicationManagerServer(), _entityManager, &_replicationManagerTransmissionDatas);
    
#ifdef NG_LOG
    LOG("Pre-State Outgoing packet Bit Length: %d \n", statePacket.getBitLength());
#endif
    
    inClientProxy->getReplicationManagerServer()->write(statePacket, rmtd);
    
    TransmissionData* currentTransmissionData = ifp->getTransmissionData('RPLM');
    if (currentTransmissionData)
    {
        _replicationManagerTransmissionDatas.free(static_cast<ReplicationManagerTransmissionData*>(currentTransmissionData));
    }
    
    ifp->setTransmissionData('RPLM', rmtd);
    
#ifdef NG_LOG
    LOG("Post-State Outgoing packet Bit Length: %d \n", statePacket.getBitLength());
#endif
    
    sendPacket(statePacket, inClientProxy->getMachineAddress());
}

void NetworkManagerServer::writeLastMoveTimestampIfDirty(OutputMemoryBitStream& inOutputStream, ClientProxy* inClientProxy)
{
    bool isTimestampDirty = inClientProxy->isLastMoveTimestampDirty();
    inOutputStream.write(isTimestampDirty);
    if (isTimestampDirty)
    {
        float lastProcessedMoveTimestamp = inClientProxy->getUnprocessedMoveList().getLastProcessedMoveTimestamp();
        
        inOutputStream.write(lastProcessedMoveTimestamp);
        
        // which map are we on?
        inOutputStream.write(_map);
        
        inClientProxy->setLastMoveTimestampDirty(false);
    }
}

void NetworkManagerServer::handleInputPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inputStream)
{
    uint8_t moveCount = 0;
    inputStream.read<uint8_t, 2>(moveCount);
    
	InputState* referenceInputState = NULL;
	bool isRefInputStateOrphaned = false;
    
    for (; moveCount > 0; --moveCount)
    {
        Move move = Move(_inputStateCreationFunc());
        
        bool isCopy;
        inputStream.read(isCopy);
        if (isCopy)
        {
            if (!referenceInputState)
            {
                LOG("Unexpected Network State!");
                
                return;
            }
            
            float timeStamp;
            inputStream.read(timeStamp);
            move.setTimestamp(timeStamp);
            
            move.copyInputState(referenceInputState);
        }
        else
        {
            move.read(inputStream);
        }

		if (isRefInputStateOrphaned && referenceInputState)
		{
            _inputStateReleaseFunc(referenceInputState);
            referenceInputState = NULL;
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
        _inputStateReleaseFunc(referenceInputState);
        referenceInputState = NULL;
    }
}

void NetworkManagerServer::handleAddLocalPlayerPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inputStream)
{
    if (_playerIDToClientMap.size() < MAX_NUM_PLAYERS_PER_SERVER)
    {
        // read the current number of local players for this client at the time when the request was made
        uint8_t requestedIndex;
        inputStream.read(requestedIndex);
        
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

void NetworkManagerServer::handleDropLocalPlayerPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inputStream)
{
    // read the index to drop
    uint8_t index;
    inputStream.read(index);
    
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

NetworkManagerServer::NetworkManagerServer(ServerHelper* inServerHelper, HandleEntityCreatedFunc handleEntityCreatedFunc, HandleEntityDeletionFunc handleEntityDeletionFunc, HandleNewClientFunc handleNewClientFunc, HandleLostClientFunc handleLostClientFunc, InputStateCreationFunc inputStateCreationFunc, InputStateReleaseFunc inputStateReleaseFunc) :
_serverHelper(inServerHelper),
_handleNewClientFunc(handleNewClientFunc),
_handleLostClientFunc(handleLostClientFunc),
_inputStateCreationFunc(inputStateCreationFunc),
_inputStateReleaseFunc(inputStateReleaseFunc),
_entityManager(new EntityManager(handleEntityCreatedFunc, handleEntityDeletionFunc)),
_timing(static_cast<Timing*>(INSTANCE_MANAGER->getInstance(INSTANCE_TIME_SERVER))),
_nextPlayerId(1),
_map(0)
{
    // Empty
}

NetworkManagerServer::~NetworkManagerServer()
{
    delete _serverHelper;
    delete _entityManager;
    
    NGSTDUtil::cleanUpMapOfPointers(_addressHashToClientMap);
    
    _playerIDToClientMap.clear();
}
