//
//  NetworkManagerServer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NetworkManagerServer.h"

#include "IServerHelper.h"
#include "InputMemoryBitStream.h"
#include "OutputMemoryBitStream.h"
#include "DeliveryNotificationManager.h"
#include "IMachineAddress.h"
#include "ClientProxy.h"
#include "IInputState.h"
#include "Entity.h"

#include "Server.h"
#include "ReplicationManagerTransmissionData.h"
#include "StringUtil.h"
#include "Timing.h"
#include "EntityManager.h"
#include "StringUtil.h"
#include "FWInstanceManager.h"
#include "NGSTDUtil.h"
#include "FrameworkConstants.h"
#include "NetworkManagerClient.h"

#include <assert.h>

NetworkManagerServer* NetworkManagerServer::s_instance = nullptr;

void NetworkManagerServer::create(IServerHelper* inServerHelper, HandleNewClientFunc inHandleNewClientFunc, HandleLostClientFunc inHandleLostClientFunc, InputStateCreationFunc inInputStateCreationFunc)
{
    assert(!s_instance);
    
    s_instance = new NetworkManagerServer(inServerHelper, inHandleNewClientFunc, inHandleLostClientFunc, inInputStateCreationFunc);
}

void NetworkManagerServer::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = nullptr;
}

NetworkManagerServer * NetworkManagerServer::getInstance()
{
    return s_instance;
}

void NetworkManagerServer::sProcessPacket(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress)
{
    NG_SERVER->processPacket(inInputStream, inFromAddress);
}

void NetworkManagerServer::sHandleNoResponse()
{
    NG_SERVER->handleNoResponse();
}

void NetworkManagerServer::sHandleConnectionReset(IMachineAddress* inFromAddress)
{
    NG_SERVER->handleConnectionReset(inFromAddress);
}

ClientProxy* NetworkManagerServer::sGetClientProxy(int inPlayerIndex)
{
    return NG_SERVER->getClientProxy(inPlayerIndex + 1);
}

void NetworkManagerServer::sHandleClientDisconnected(ClientProxy* inClientProxy)
{
    NG_SERVER->handleClientDisconnected(inClientProxy);
}

void NetworkManagerServer::processIncomingPackets()
{
    m_serverHelper->processIncomingPackets();
}

void NetworkManagerServer::checkForDisconnects()
{
    std::vector<ClientProxy*> clientsToDC;
    
    float minAllowedLastPacketFromClientTime = Timing::getInstance()->getFrameStartTime() - NW_CLIENT_TIMEOUT;
    for (const auto& pair: m_addressHashToClientMap)
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
    for (auto it = m_addressHashToClientMap.begin(), end = m_addressHashToClientMap.end(); it != end; ++it)
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
    for (const auto& pair: m_addressHashToClientMap)
    {
        pair.second->getReplicationManagerServer().replicateCreate(inEntity->getID(), inEntity->getAllStateMask());
    }
}

void NetworkManagerServer::deregisterEntity(Entity* inEntity)
{
    int networkId = inEntity->getID();
    
    FWInstanceManager::getServerEntityManager()->deregisterEntity(inEntity);
    
    //tell all client proxies to STOP replicating!
    //tell all client proxies this is new...
    for (const auto& pair: m_addressHashToClientMap)
    {
        pair.second->getReplicationManagerServer().replicateDestroy(networkId);
    }
}

void NetworkManagerServer::setStateDirty(int inNetworkId, uint32_t inDirtyState)
{
    //tell everybody this is dirty
    for (const auto& pair: m_addressHashToClientMap)
    {
        pair.second->getReplicationManagerServer().setStateDirty(inNetworkId, inDirtyState);
    }
}

ClientProxy* NetworkManagerServer::getClientProxy(int inPlayerId) const
{
    auto it = m_playerIDToClientMap.find(inPlayerId);
    if (it != m_playerIDToClientMap.end())
    {
        return it->second;
    }
    
    return nullptr;
}

int NetworkManagerServer::getAverageMoveCount() const
{
    int ret = 0;
    
    if (m_addressHashToClientMap.size() > 0)
    {
        for (auto const &entry : m_addressHashToClientMap)
        {
            ret += entry.second->getUnprocessedMoveList().getMoveCount();
        }
        
        ret /= m_addressHashToClientMap.size();
    }
    
    return ret;
}

int NetworkManagerServer::getNumClientsConnected()
{
    return static_cast<int>(m_addressHashToClientMap.size());
}

IMachineAddress* NetworkManagerServer::getServerAddress()
{
    return m_serverHelper->getServerAddress();
}

bool NetworkManagerServer::isConnected()
{
    return m_serverHelper->isConnected();
}

IServerHelper* NetworkManagerServer::getServerHelper()
{
    return m_serverHelper;
}

void NetworkManagerServer::processPacket(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress)
{
    //try to get the client proxy for this address
    //pass this to the client proxy to process
    auto it = m_addressHashToClientMap.find(inFromAddress->getHash());
    if (it == m_addressHashToClientMap.end())
    {
        if (m_playerIDToClientMap.size() < MAX_NUM_PLAYERS_PER_SERVER)
        {
            LOG("New Client with %s", inFromAddress->toString().c_str());
            
            //didn't find one? it's a new cilent..is the a HELO? if so, create a client proxy...
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

void NetworkManagerServer::handleConnectionReset(IMachineAddress* inFromAddress)
{
    //just dc the client right away...
    auto it = m_addressHashToClientMap.find(inFromAddress->getHash());
    if (it != m_addressHashToClientMap.end())
    {
        handleClientDisconnected(it->second);
    }
}

void NetworkManagerServer::sendPacket(const OutputMemoryBitStream& inOutputStream, IMachineAddress* inFromAddress)
{
    m_serverHelper->sendPacket(inOutputStream, inFromAddress);
}

void NetworkManagerServer::handlePacketFromNewClient(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress)
{
    // read the beginning- is it a hello?
    uint32_t packetType;
    inInputStream.read(packetType);
    if (packetType == NW_PACKET_TYPE_HELLO)
    {
        // read the name
        std::string name;
        inInputStream.read(name);
        
        if (m_addressHashToClientMap.size() == 0)
        {
            if (NG_CLIENT->getPlayerName().compare(name) != 0)
            {
                // The server host MUST be the first client to join
                return;
            }
        }
        
        ClientProxy* newClientProxy = new ClientProxy(inFromAddress, name, m_iNextPlayerId);
        m_addressHashToClientMap[inFromAddress->getHash()] = newClientProxy;
        m_playerIDToClientMap[newClientProxy->getPlayerId()] = newClientProxy;
        
        int playerId = newClientProxy->getPlayerId();
        std::string playerName = newClientProxy->getName();
        
        // tell the server about this client
        m_handleNewClientFunc(playerId, playerName);
        
        //and welcome the client...
        sendWelcomePacket(newClientProxy);
        
        // and now init the replication manager with everything we know about!
        for (const auto& pair: FWInstanceManager::getServerEntityManager()->getMap())
        {
            Entity* pe = pair.second;
            newClientProxy->getReplicationManagerServer().replicateCreate(pair.first, pe->getAllStateMask());
        }
        
        updateNextPlayerId();
    }
    else
    {
        m_serverHelper->processSpecialPacket(packetType, inInputStream, inFromAddress);
    }
}

void NetworkManagerServer::processPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream)
{
    //remember we got a packet so we know not to disconnect for a bit
    inClientProxy->updateLastPacketTime();
    
    uint32_t packetType;
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
            m_serverHelper->processSpecialPacket(packetType, inInputStream, inClientProxy->getMachineAddress());
            break;
    }
}

void NetworkManagerServer::sendWelcomePacket(ClientProxy* inClientProxy)
{
    OutputMemoryBitStream packet;
    
    packet.write(NW_PACKET_TYPE_WELCOME);
    packet.write(inClientProxy->getPlayerId());
    
    LOG("Server welcoming new client '%s' as player %d", inClientProxy->getName().c_str(), inClientProxy->getPlayerId());
    
    sendPacket(packet, inClientProxy->getMachineAddress());
}

void NetworkManagerServer::sendStatePacketToClient(ClientProxy* inClientProxy)
{
    //build state packet
    OutputMemoryBitStream statePacket;
    
    //it's state!
    statePacket.write(NW_PACKET_TYPE_STATE);
    
    InFlightPacket* ifp = inClientProxy->getDeliveryNotificationManager().writeState(statePacket);
    
    writeLastMoveTimestampIfDirty(statePacket, inClientProxy);
    
    ReplicationManagerTransmissionData* rmtd = new ReplicationManagerTransmissionData(&inClientProxy->getReplicationManagerServer());
    inClientProxy->getReplicationManagerServer().write(statePacket, rmtd);
    ifp->setTransmissionData('RPLM', rmtd);
    
    sendPacket(statePacket, inClientProxy->getMachineAddress());
}

void NetworkManagerServer::writeLastMoveTimestampIfDirty(OutputMemoryBitStream& inOutputStream, ClientProxy* inClientProxy)
{
    //first, dirty?
    bool isTimestampDirty = inClientProxy->isLastMoveTimestampDirty();
    inOutputStream.write(isTimestampDirty);
    if (isTimestampDirty)
    {
        inOutputStream.write(inClientProxy->getUnprocessedMoveList().getLastProcessedMoveTimestamp());
        inClientProxy->setIsLastMoveTimestampDirty(false);
    }
}

void NetworkManagerServer::handleInputPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream)
{
    uint8_t moveCount = 0;
    Move move = Move(m_inputStateCreationFunc());
    inInputStream.read(moveCount, 4);
    
    for (; moveCount > 0; --moveCount)
    {
        if (move.read(inInputStream))
        {
            if (inClientProxy->getUnprocessedMoveList().addMoveIfNew(move))
            {
                inClientProxy->setIsLastMoveTimestampDirty(true);
            }
        }
    }
}

void NetworkManagerServer::handleAddLocalPlayerPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream)
{
    if (m_playerIDToClientMap.size() < MAX_NUM_PLAYERS_PER_SERVER)
    {
        // read the current number of local players for this client at the time when the request was made
        uint8_t requestedIndex;
        inInputStream.read(requestedIndex);
        
        int playerId = inClientProxy->getPlayerId(requestedIndex);
        if (playerId == INPUT_UNASSIGNED)
        {
            std::string localPlayerName = StringUtil::format("%s(%d)", inClientProxy->getName().c_str(), requestedIndex);
            
            int playerId = m_iNextPlayerId;
            
            inClientProxy->onLocalPlayerAdded(playerId);
            
            m_playerIDToClientMap[playerId] = inClientProxy;
            
            // tell the server about this client
            m_handleNewClientFunc(playerId, localPlayerName);
            
            updateNextPlayerId();
        }
        
        // and welcome the new local player...
        sendLocalPlayerAddedPacket(inClientProxy, inClientProxy->getNumPlayers() - 1);
    }
    else
    {
        OutputMemoryBitStream packet;
        packet.write(NW_PACKET_TYPE_LOCAL_PLAYER_DENIED);
        
        sendPacket(packet, inClientProxy->getMachineAddress());
    }
}

void NetworkManagerServer::sendLocalPlayerAddedPacket(ClientProxy* inClientProxy, int index)
{
    int playerId = inClientProxy->getPlayerId(index);
    
    OutputMemoryBitStream packet;
    
    packet.write(NW_PACKET_TYPE_LOCAL_PLAYER_ADDED);
    packet.write(playerId);
    
    std::string localPlayerName = StringUtil::format("%s(%d)", inClientProxy->getName().c_str(), index);
    
    LOG("Server welcoming new client local player '%s' as player %d", localPlayerName.c_str(), playerId);
    
    sendPacket(packet, inClientProxy->getMachineAddress());
}

void NetworkManagerServer::handleDropLocalPlayerPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream)
{
    // read the index to drop
    int index;
    inInputStream.read(index);
    
    if (index < 1)
    {
        // If the primary player on this client wants to drop, a disconnect request should be fired off instead of a drop
        return;
    }
    
    int playerId = inClientProxy->getPlayerId(index);
    if (playerId != INPUT_UNASSIGNED)
    {
        m_playerIDToClientMap.erase(playerId);
        
        m_handleLostClientFunc(inClientProxy, index);
        
        inClientProxy->onLocalPlayerRemoved(playerId);
        
        updateNextPlayerId();
    }
}

void NetworkManagerServer::handleClientDisconnected(ClientProxy* inClientProxy)
{
    for (int i = 0; i < inClientProxy->getNumPlayers(); ++i)
    {
        m_playerIDToClientMap.erase(inClientProxy->getPlayerId(i));
    }
    
    m_addressHashToClientMap.erase(inClientProxy->getMachineAddress()->getHash());
    
    m_handleLostClientFunc(inClientProxy, 0);
    
    m_serverHelper->onClientDisconnected(inClientProxy);
    
    delete inClientProxy;
    
    updateNextPlayerId();
}

void NetworkManagerServer::updateNextPlayerId()
{
    LOG("updateNextPlayerId");
    
    // Find the next available Player ID
    m_iNextPlayerId = 1;
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        for (auto const &entry : m_playerIDToClientMap)
        {
            if (entry.first == m_iNextPlayerId)
            {
                ++m_iNextPlayerId;
            }
        }
    }
    
    LOG("m_iNextPlayerId: %d", m_iNextPlayerId);
}

NetworkManagerServer::NetworkManagerServer(IServerHelper* inServerHelper, HandleNewClientFunc inHandleNewClientFunc, HandleLostClientFunc inHandleLostClientFunc, InputStateCreationFunc inInputStateCreationFunc) :
m_serverHelper(inServerHelper),
m_handleNewClientFunc(inHandleNewClientFunc),
m_handleLostClientFunc(inHandleLostClientFunc),
m_inputStateCreationFunc(inInputStateCreationFunc),
m_iNextPlayerId(1)
{
    // Empty
}

NetworkManagerServer::~NetworkManagerServer()
{
    delete m_serverHelper;
    
    NGSTDUtil::cleanUpMapOfPointers(m_addressHashToClientMap);
    
    m_playerIDToClientMap.clear();
}
