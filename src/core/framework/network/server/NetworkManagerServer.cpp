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

void NetworkManagerServer::staticProcessPacket(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress)
{
    NG_SERVER->processPacket(inInputStream, inFromAddress);
}

void NetworkManagerServer::staticHandleNoResponse()
{
    NG_SERVER->handleNoResponse();
}

void NetworkManagerServer::staticHandleConnectionReset(IMachineAddress* inFromAddress)
{
    NG_SERVER->handleConnectionReset(inFromAddress);
}

void NetworkManagerServer::processIncomingPackets()
{
    m_serverHelper->processIncomingPackets();
}

void NetworkManagerServer::checkForDisconnects()
{
    std::vector<ClientProxy*> clientsToDC;
    
    float minAllowedLastPacketFromClientTime = Timing::getInstance()->getFrameStartTime() - NETWORK_CLIENT_TIMEOUT;
    for (const auto& pair: m_addressHashToClientMap)
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

void NetworkManagerServer::sendOutgoingPackets()
{
    //let's send a client a state packet whenever their move has come in...
    for (auto it = m_addressHashToClientMap.begin(), end = m_addressHashToClientMap.end(); it != end; ++it)
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

void NetworkManagerServer::unregisterEntity(Entity* inEntity)
{
    int networkId = inEntity->getID();
    
    FWInstanceManager::getServerEntityManager()->removeEntity(inEntity);
    
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

void NetworkManagerServer::processPacket(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress)
{
    //try to get the client proxy for this address
    //pass this to the client proxy to process
    auto it = m_addressHashToClientMap.find(inFromAddress->getHash());
    if (it == m_addressHashToClientMap.end()
        && m_addressHashToClientMap.size() < MAX_NUM_PLAYERS_PER_SERVER)
    {
        LOG("Client %s", inFromAddress->toString().c_str());
        
        //didn't find one? it's a new cilent..is the a HELO? if so, create a client proxy...
        handlePacketFromNewClient(inInputStream, inFromAddress);
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
    //read the beginning- is it a hello?
    uint32_t packetType;
    inInputStream.read(packetType);
    if (packetType == NETWORK_PACKET_TYPE_HELLO)
    {
        //read the name
        std::string name;
        inInputStream.read(name);
        
        IMachineAddress* clientProxyInFromAddress = inFromAddress->createCopy();
        ClientProxy* newClientProxy = new ClientProxy(clientProxyInFromAddress, name, m_iNewPlayerId++);
        m_addressHashToClientMap[clientProxyInFromAddress->getHash()] = newClientProxy;
        m_playerIDToClientMap[newClientProxy->getPlayerId()] = newClientProxy;
        
        // tell the server about this client
        m_handleNewClientFunc(newClientProxy);
        
        //and welcome the client...
        sendWelcomePacket(newClientProxy);
        
        //and now init the replication manager with everything we know about!
        for (const auto& pair: FWInstanceManager::getServerEntityManager()->getMap())
        {
            Entity* pe = pair.second;
            newClientProxy->getReplicationManagerServer().replicateCreate(pair.first, pe->getAllStateMask());
        }
    }
    else
    {
        //bad incoming packet from unknown client- we're under attack!!
        LOG("Bad incoming packet from unknown client at %s", inFromAddress->toString().c_str());
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
        case NETWORK_PACKET_TYPE_HELLO:
            //need to resend welcome. to be extra safe we should check the name is the one we expect from this address,
            //otherwise something weird is going on...
            sendWelcomePacket(inClientProxy);
            break;
        case NETWORK_PACKET_TYPE_INPUT:
            if (inClientProxy->getDeliveryNotificationManager().readAndProcessState(inInputStream))
            {
                handleInputPacket(inClientProxy, inInputStream);
            }
            break;
        default:
            LOG("Unknown packet type received from %s", inClientProxy->toString().c_str());
            break;
    }
}

void NetworkManagerServer::sendWelcomePacket(ClientProxy* inClientProxy)
{
    OutputMemoryBitStream welcomePacket;
    
    welcomePacket.write(NETWORK_PACKET_TYPE_WELCOME);
    welcomePacket.write(inClientProxy->getPlayerId());
    
    LOG("Server Welcoming, new client '%s' as player %d", inClientProxy->getName().c_str(), inClientProxy->getPlayerId());
    
    sendPacket(welcomePacket, inClientProxy->getMachineAddress());
}

void NetworkManagerServer::sendStatePacketToClient(ClientProxy* inClientProxy)
{
    //build state packet
    OutputMemoryBitStream statePacket;
    
    //it's state!
    statePacket.write(NETWORK_PACKET_TYPE_STATE);
    
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
    m_addressHashToClientMap.erase(inClientProxy->getMachineAddress()->getHash());
    
    m_handleLostClientFunc(inClientProxy);
    
    m_iNewPlayerId = 1;
    for (auto const &entry : m_addressHashToClientMap)
    {
        if (entry.second->getPlayerId() == m_iNewPlayerId)
        {
            ++m_iNewPlayerId;
        }
    }
}

NetworkManagerServer::NetworkManagerServer(IServerHelper* inServerHelper, HandleNewClientFunc inHandleNewClientFunc, HandleLostClientFunc inHandleLostClientFunc, InputStateCreationFunc inInputStateCreationFunc) :
m_serverHelper(inServerHelper),
m_handleNewClientFunc(inHandleNewClientFunc),
m_handleLostClientFunc(inHandleLostClientFunc),
m_inputStateCreationFunc(inInputStateCreationFunc),
m_iNewPlayerId(1)
{
    // Empty
}

NetworkManagerServer::~NetworkManagerServer()
{
    delete m_serverHelper;
    
    NGSTDUtil::cleanUpMapOfPointers(m_addressHashToClientMap);
    
    m_playerIDToClientMap.clear();
}
