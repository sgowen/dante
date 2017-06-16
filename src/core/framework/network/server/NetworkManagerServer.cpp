//
//  NetworkManagerServer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NetworkManagerServer.h"

#include "IInputState.h"

#include "Server.h"
#include "ReplicationManagerTransmissionData.h"
#include "StringUtil.h"
#include "Timing.h"
#include "EntityManager.h"
#include "Entity.h"
#include "OutputMemoryBitStream.h"
#include "StringUtil.h"

NetworkManagerServer* NetworkManagerServer::getInstance()
{
    static NetworkManagerServer instance = NetworkManagerServer();
    return &instance;
}

void NetworkManagerServer::processPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
    //try to get the client proxy for this address
    //pass this to the client proxy to process
    auto it = m_addressToClientMap.find(inFromAddress);
    if (it == m_addressToClientMap.end()
        && m_addressToClientMap.size() < 4)
    {
        LOG("Client socket %s", inFromAddress.toString().c_str());
        
        //didn't find one? it's a new cilent..is the a HELO? if so, create a client proxy...
        handlePacketFromNewClient(inInputStream, inFromAddress);
    }
    else
    {
        processPacket((*it).second, inInputStream);
    }
}

void NetworkManagerServer::handleConnectionReset(const SocketAddress& inFromAddress)
{
    //just dc the client right away...
    auto it = m_addressToClientMap.find(inFromAddress);
    if (it != m_addressToClientMap.end())
    {
        handleClientDisconnected(it->second);
    }
}

bool NetworkManagerServer::init(uint16_t inPort, HandleEntityDeletionFunc handleEntityDeletion, HandleNewClientFunc handleNewClientFunc, HandleLostClientFunc handleLostClientFunc, InputStateCreationFunc inputStateCreationFunc)
{
    m_handleNewClientFunc = handleNewClientFunc;
    m_handleLostClientFunc = handleLostClientFunc;
    m_inputStateCreationFunc = inputStateCreationFunc;
    
    return INetworkManager::init(inPort, handleEntityDeletion);
}

void NetworkManagerServer::sendOutgoingPackets()
{
    //let's send a client a state packet whenever their move has come in...
    for (auto it = m_addressToClientMap.begin(), end = m_addressToClientMap.end(); it != end; ++it)
    {
        ClientProxy* clientProxy = it->second;
        //process any timed out packets while we're going through the list
        clientProxy->getDeliveryNotificationManager().processTimedOutPackets(Timing::getInstance()->getFrameStartTime());
        
        if (clientProxy->IsLastMoveTimestampDirty())
        {
            sendStatePacketToClient(clientProxy);
        }
    }
}

void NetworkManagerServer::checkForDisconnects()
{
    std::vector<ClientProxy*> clientsToDC;
    
    float minAllowedLastPacketFromClientTime = Timing::getInstance()->getFrameStartTime() - m_fClientDisconnectTimeout;
    for (const auto& pair: m_addressToClientMap)
    {
        if (pair.second->getLastPacketFromClientTime() < minAllowedLastPacketFromClientTime)
        {
            //can't remove from map while in iterator, so just remember for later...
            clientsToDC.push_back(pair.second);
        }
    }
    
    for (ClientProxy* client: clientsToDC)
    {
        handleClientDisconnected(client);
    }
}

void NetworkManagerServer::registerEntity(Entity* inEntity)
{
    //add mapping from network id to game object
    m_entityManager->registerEntity(inEntity);
    
    //tell all client proxies this is new...
    for (const auto& pair: m_addressToClientMap)
    {
        pair.second->getReplicationManagerServer().replicateCreate(inEntity->getID(), inEntity->getAllStateMask());
    }
}

void NetworkManagerServer::unregisterEntity(Entity* inEntity)
{
    int networkId = inEntity->getID();
    
    removeFromNetworkIdToEntityMap(inEntity);
    
    //tell all client proxies to STOP replicating!
    //tell all client proxies this is new...
    for (const auto& pair: m_addressToClientMap)
    {
        pair.second->getReplicationManagerServer().replicateDestroy(networkId);
    }
}

void NetworkManagerServer::setStateDirty(int inNetworkId, uint32_t inDirtyState)
{
    //tell everybody this is dirty
    for (const auto& pair: m_addressToClientMap)
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

int NetworkManagerServer::getNumClientsConnected()
{
    return static_cast<int>(m_addressToClientMap.size());
}

void NetworkManagerServer::handlePacketFromNewClient(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
    //read the beginning- is it a hello?
    uint32_t packetType;
    inInputStream.read(packetType);
    if (packetType == kHelloCC)
    {
        //read the name
        std::string name;
        inInputStream.read(name);
        
        ClientProxy* newClientProxy = new ClientProxy(inFromAddress, name, m_iNewPlayerId++);
        m_addressToClientMap[inFromAddress] = newClientProxy;
        m_playerIDToClientMap[newClientProxy->getPlayerId()] = newClientProxy;
        
        // tell the server about this client
        m_handleNewClientFunc(newClientProxy);
        
        //and welcome the client...
        sendWelcomePacket(newClientProxy);
        
        //and now init the replication manager with everything we know about!
        for (const auto& pair: m_entityManager->getMap())
        {
            Entity* pe = pair.second;
            newClientProxy->getReplicationManagerServer().replicateCreate(pair.first, pe->getAllStateMask());
        }
    }
    else
    {
        //bad incoming packet from unknown client- we're under attack!!
        LOG("Bad incoming packet from unknown client at socket %s", inFromAddress.toString().c_str());
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
        case kHelloCC:
            //need to resend welcome. to be extra safe we should check the name is the one we expect from this address,
            //otherwise something weird is going on...
            sendWelcomePacket(inClientProxy);
            break;
        case kInputCC:
            if (inClientProxy->getDeliveryNotificationManager().readAndProcessState(inInputStream))
            {
                handleInputPacket(inClientProxy, inInputStream);
            }
            break;
        default:
            LOG("Unknown packet type received from %s", inClientProxy->getSocketAddress().toString().c_str());
            break;
    }
}

void NetworkManagerServer::sendWelcomePacket(ClientProxy* inClientProxy)
{
    OutputMemoryBitStream welcomePacket;
    
    welcomePacket.write(kWelcomeCC);
    welcomePacket.write(inClientProxy->getPlayerId());
    
    LOG("Server Welcoming, new client '%s' as player %d", inClientProxy->getName().c_str(), inClientProxy->getPlayerId());
    
    sendPacket(welcomePacket, inClientProxy->getSocketAddress());
}

void NetworkManagerServer::sendStatePacketToClient(ClientProxy* inClientProxy)
{
    //build state packet
    OutputMemoryBitStream statePacket;
    
    //it's state!
    statePacket.write(kStateCC);
    
    InFlightPacket* ifp = inClientProxy->getDeliveryNotificationManager().writeState(statePacket);
    
    writeLastMoveTimestampIfDirty(statePacket, inClientProxy);
    
    ReplicationManagerTransmissionData* rmtd = new ReplicationManagerTransmissionData(&inClientProxy->getReplicationManagerServer());
    inClientProxy->getReplicationManagerServer().write(statePacket, rmtd);
    ifp->setTransmissionData('RPLM', rmtd);
    
    sendPacket(statePacket, inClientProxy->getSocketAddress());
}

void NetworkManagerServer::writeLastMoveTimestampIfDirty(OutputMemoryBitStream& inOutputStream, ClientProxy* inClientProxy)
{
    //first, dirty?
    bool isTimestampDirty = inClientProxy->IsLastMoveTimestampDirty();
    inOutputStream.write(isTimestampDirty);
    if (isTimestampDirty)
    {
        inOutputStream.write(inClientProxy->getUnprocessedMoveList().getLastMoveTimestamp());
        inClientProxy->SetIsLastMoveTimestampDirty(false);
    }
}

void NetworkManagerServer::handleInputPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream)
{
    uint32_t moveCount = 0;
    Move move = Move(m_inputStateCreationFunc());
    inInputStream.read(moveCount, 2);
    
    for (; moveCount > 0; --moveCount)
    {
        if (move.read(inInputStream))
        {
            if (inClientProxy->getUnprocessedMoveList().addMoveIfNew(move))
            {
                inClientProxy->SetIsLastMoveTimestampDirty(true);
            }
        }
    }
}

void NetworkManagerServer::handleClientDisconnected(ClientProxy* inClientProxy)
{
    m_playerIDToClientMap.erase(inClientProxy->getPlayerId());
    m_addressToClientMap.erase(inClientProxy->getSocketAddress());
    
    m_handleLostClientFunc(inClientProxy);
    
    m_iNewPlayerId = 1;
    for (auto const &entry : m_addressToClientMap)
    {
        if (entry.second->getPlayerId() == m_iNewPlayerId)
        {
            ++m_iNewPlayerId;
        }
    }
}

NetworkManagerServer::NetworkManagerServer() : INetworkManager(), m_iNewPlayerId(1), m_fClientDisconnectTimeout(3.f)
{
    // Empty
}

NetworkManagerServer::~NetworkManagerServer()
{
    // Empty
}
