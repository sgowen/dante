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
    static NetworkManagerServer* sInstance;
    
    static bool StaticInit(uint16_t inPort);
    
    virtual void ProcessPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress) override;
    virtual void HandleConnectionReset(const SocketAddress& inFromAddress) override;
    
    void SendOutgoingPackets();
    void CheckForDisconnects();
    
    void RegisterGameObject(GameObjectPtr inGameObject);
    inline GameObjectPtr RegisterAndReturn(GameObject* inGameObject);
    void UnregisterGameObject(GameObject* inGameObject);
    void SetStateDirty(int inNetworkId, uint32_t inDirtyState);
    
    void RespawnCats();
    
    ClientProxyPtr GetClientProxy(int inPlayerId) const;
    
private:
    NetworkManagerServer();
    
    void HandlePacketFromNewClient(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress);
    void ProcessPacket(ClientProxyPtr inClientProxy, InputMemoryBitStream& inInputStream);
    
    void SendWelcomePacket(ClientProxyPtr inClientProxy);
    void UpdateAllClients();
    
    void AddWorldStateToPacket(OutputMemoryBitStream& inOutputStream);
    
    void SendStatePacketToClient(ClientProxyPtr inClientProxy);
    void WriteLastMoveTimestampIfDirty(OutputMemoryBitStream& inOutputStream, ClientProxyPtr inClientProxy);
    
    void HandleInputPacket(ClientProxyPtr inClientProxy, InputMemoryBitStream& inInputStream);
    
    void HandleClientDisconnected(ClientProxyPtr inClientProxy);
    
    int GetNewNetworkId();
    
    typedef std::unordered_map<int, ClientProxyPtr> IntToClientMap;
    typedef std::unordered_map<SocketAddress, ClientProxyPtr> AddressToClientMap;
    
    AddressToClientMap mAddressToClientMap;
    IntToClientMap m_iPlayerIdToClientMap;
    
    int m_iNewPlayerId;
    int m_iNewNetworkId;
    
    float m_fTimeOfLastSatePacket;
    float m_fTimeBetweenStatePackets;
    float m_fClientDisconnectTimeout;
};

inline GameObjectPtr NetworkManagerServer::RegisterAndReturn(GameObject* inGameObject)
{
    GameObjectPtr toRet(inGameObject);
    RegisterGameObject(toRet);
    return toRet;
}

#endif /* defined(__noctisgames__NetworkManagerServer__) */
