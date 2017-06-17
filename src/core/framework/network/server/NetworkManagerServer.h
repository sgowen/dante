//
//  NetworkManagerServer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NetworkManagerServer__
#define __noctisgames__NetworkManagerServer__

#include "INetworkManager.h"

#include "ClientProxy.h"
#include "IMachineAddress.h"

class IInputState;
class Entity;

#define NG_SERVER (NetworkManagerServer::getInstance())

typedef void (*HandleNewClientFunc)(ClientProxy* inClientProxy);
typedef void (*HandleLostClientFunc)(ClientProxy* inClientProxy);
typedef IInputState* (*InputStateCreationFunc)();

class NetworkManagerServer : public INetworkManager
{
public:
    static void create(uint16_t inPort, HandleNewClientFunc handleNewClientFunc, HandleLostClientFunc handleLostClientFunc, InputStateCreationFunc inputStateCreationFunc);
    
    static void destroy();
    
    static NetworkManagerServer* getInstance();
    
    static void staticProcessPacket(InputMemoryBitStream& inInputStream, SocketAddress* inFromAddress);
    
    static void staticHandleConnectionReset(IMachineAddress* inFromAddress);
    
    virtual void processPacket(InputMemoryBitStream& inInputStream, SocketAddress* inFromAddress) override;
    
    virtual void handleConnectionReset(SocketAddress* inFromAddress) override;
    
    virtual void sendOutgoingPackets();
    
    void checkForDisconnects();
    
    void registerEntity(Entity* inEntity);
    
    void unregisterEntity(Entity* inEntity);
    
    void setStateDirty(int inNetworkId, uint32_t inDirtyState);
    
    ClientProxy* getClientProxy(int inPlayerId) const;
    
    int getNumClientsConnected();
    
private:
    static NetworkManagerServer* s_instance;
    
    HandleNewClientFunc m_handleNewClientFunc;
    HandleLostClientFunc m_handleLostClientFunc;
    InputStateCreationFunc m_inputStateCreationFunc;
    
    std::unordered_map<size_t, ClientProxy*> m_addressHashToClientMap;
    std::unordered_map<int, ClientProxy*> m_playerIDToClientMap;
    int m_iNewPlayerId;
    float m_fTimeOfLastSatePacket;
    
    void handlePacketFromNewClient(InputMemoryBitStream& inInputStream, SocketAddress* inFromAddress);
    
    void processPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream);
    
    void sendWelcomePacket(ClientProxy* inClientProxy);
    
    void sendStatePacketToClient(ClientProxy* inClientProxy);
    
    void writeLastMoveTimestampIfDirty(OutputMemoryBitStream& inOutputStream, ClientProxy* inClientProxy);
    
    void handleInputPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream);
    
    void handleClientDisconnected(ClientProxy* inClientProxy);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    NetworkManagerServer(uint16_t inPort, HandleNewClientFunc handleNewClientFunc, HandleLostClientFunc handleLostClientFunc, InputStateCreationFunc inputStateCreationFunc);
    virtual ~NetworkManagerServer();
    NetworkManagerServer(const NetworkManagerServer&);
    NetworkManagerServer& operator=(const NetworkManagerServer&);
};

#endif /* defined(__noctisgames__NetworkManagerServer__) */
