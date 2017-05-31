//
//  NetworkManagerServer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NetworkManagerServer__
#define __noctisgames__NetworkManagerServer__

#include "NetworkManager.h"

#include "ClientProxy.h"

class InputMemoryBitStream;

class NetworkManagerServer : public NetworkManager
{
public:
    static NetworkManagerServer* getInstance();
    
    static bool StaticInit(uint16_t inPort);
    
    virtual void ProcessPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress) override;
    virtual void HandleConnectionReset(const SocketAddress& inFromAddress) override;
    
    void SendOutgoingPackets();
    void CheckForDisconnects();
    
    void RegisterGameObject(GameObject* inGameObject);
    void UnregisterGameObject(GameObject* inGameObject);
    void SetStateDirty(int inNetworkId, uint32_t inDirtyState);
    
    void RespawnCats();
    
    ClientProxy* GetClientProxy(int inPlayerId) const;
    
private:
    typedef std::unordered_map<int, ClientProxy*> IntToClientMap;
    typedef std::unordered_map<SocketAddress, ClientProxy*> AddressToClientMap;
    
    AddressToClientMap mAddressToClientMap;
    IntToClientMap m_iPlayerIdToClientMap;
    
    int m_iNewPlayerId;
    int m_iNewNetworkId;
    
    float m_fTimeOfLastSatePacket;
    float m_fTimeBetweenStatePackets;
    float m_fClientDisconnectTimeout;
    
    NetworkManagerServer();
    
    void HandlePacketFromNewClient(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress);
    void ProcessPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream);
    
    void SendWelcomePacket(ClientProxy* inClientProxy);
    void UpdateAllClients();
    
    void AddWorldStateToPacket(OutputMemoryBitStream& inOutputStream);
    
    void SendStatePacketToClient(ClientProxy* inClientProxy);
    void WriteLastMoveTimestampIfDirty(OutputMemoryBitStream& inOutputStream, ClientProxy* inClientProxy);
    
    void HandleInputPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream);
    
    void HandleClientDisconnected(ClientProxy* inClientProxy);
    
    int GetNewNetworkId();
};

#endif /* defined(__noctisgames__NetworkManagerServer__) */
