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

typedef void (*HandleNewClientFunc)(ClientProxy* inClientProxy);
typedef void (*HandleLostClientFunc)(ClientProxy* inClientProxy);

class NetworkManagerServer : public INetworkManager
{
public:
    static NetworkManagerServer* getInstance();
    
    virtual void processPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress) override;
    
    virtual void handleConnectionReset(const SocketAddress& inFromAddress) override;
    
    bool init(uint16_t inPort, HandleEntityDeletion handleEntityDeletion, HandleNewClientFunc handleNewClientFunc, HandleLostClientFunc handleLostClientFunc);
    
    void sendOutgoingPackets();
    
    void checkForDisconnects();
    
    void registerEntity(Entity* inEntity);
    
    void unregisterEntity(Entity* inEntity);
    
    void setStateDirty(int inNetworkId, uint32_t inDirtyState);
    
    void respawnCats();
    
    ClientProxy* getClientProxy(int inPlayerId) const;
    
private:
    HandleNewClientFunc m_handleNewClientFunc;
    HandleLostClientFunc m_handleLostClientFunc;
    
    std::unordered_map<SocketAddress, ClientProxy*> m_addressToClientMap;
    std::unordered_map<int, ClientProxy*> m_playerIDToClientMap;
    int m_iNewPlayerId;
    float m_fTimeOfLastSatePacket;
    float m_fTimeBetweenStatePackets;
    float m_fClientDisconnectTimeout;
    
    void handlePacketFromNewClient(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress);
    
    void processPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream);
    
    void sendWelcomePacket(ClientProxy* inClientProxy);
    
    void UpdateAllClients();
    
    void sendStatePacketToClient(ClientProxy* inClientProxy);
    
    void writeLastMoveTimestampIfDirty(OutputMemoryBitStream& inOutputStream, ClientProxy* inClientProxy);
    
    void handleInputPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream);
    
    void handleClientDisconnected(ClientProxy* inClientProxy);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    NetworkManagerServer();
    virtual ~NetworkManagerServer();
    NetworkManagerServer(const NetworkManagerServer&);
    NetworkManagerServer& operator=(const NetworkManagerServer&);
};

#endif /* defined(__noctisgames__NetworkManagerServer__) */
