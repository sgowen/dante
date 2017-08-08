//
//  NetworkManagerServer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NetworkManagerServer__
#define __noctisgames__NetworkManagerServer__

#include <unordered_map>

class IServerHelper;
class InputMemoryBitStream;
class OutputMemoryBitStream;
class DeliveryNotificationManager;
class IMachineAddress;
class ClientProxy;
class IInputState;
class Entity;

#define NG_SERVER (NetworkManagerServer::getInstance())

#define NG_SERVER_CALLBACKS NetworkManagerServer::sProcessPacket, NetworkManagerServer::sHandleNoResponse, NetworkManagerServer::sHandleConnectionReset, NetworkManagerServer::sGetClientProxy, NetworkManagerServer::sHandleClientDisconnected

typedef void (*HandleNewClientFunc)(int playerId, std::string playerName);
typedef void (*HandleLostClientFunc)(ClientProxy* inClientProxy, int index);
typedef IInputState* (*InputStateCreationFunc)();

class NetworkManagerServer
{
public:
    static void create(IServerHelper* inServerHelper, HandleNewClientFunc inHandleNewClientFunc, HandleLostClientFunc inHandleLostClientFunc, InputStateCreationFunc inInputStateCreationFunc);
    
    static void destroy();
    
    static NetworkManagerServer* getInstance();
    
    static void sProcessPacket(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress);
    
    static void sHandleNoResponse();
    
    static void sHandleConnectionReset(IMachineAddress* inFromAddress);
    
    static ClientProxy* sGetClientProxy(int inPlayerId);
    
    static void sHandleClientDisconnected(ClientProxy* inClientProxy);
    
    void processIncomingPackets();
    
    void checkForDisconnects();
    
    void sendOutgoingPackets();
    
    void registerEntity(Entity* inEntity);
    
    void deregisterEntity(Entity* inEntity);
    
    void setStateDirty(int inNetworkId, uint32_t inDirtyState);
    
    ClientProxy* getClientProxy(int inPlayerId) const;
    
    int getAverageMoveCount() const;
    
    int getNumClientsConnected();
    
    IMachineAddress* getServerAddress();
    
    bool isConnected();
    
    IServerHelper* getServerHelper();
    
private:
    static NetworkManagerServer* s_instance;
    
    IServerHelper* m_serverHelper;
    
    HandleNewClientFunc m_handleNewClientFunc;
    HandleLostClientFunc m_handleLostClientFunc;
    InputStateCreationFunc m_inputStateCreationFunc;
    
    std::unordered_map<size_t, ClientProxy*> m_addressHashToClientMap;
    std::unordered_map<int, ClientProxy*> m_playerIDToClientMap;
    uint8_t m_iNextPlayerId;
    
    void processPacket(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress);
    
    void handleNoResponse();
    
    void handleConnectionReset(IMachineAddress* inFromAddress);
    
    void sendPacket(const OutputMemoryBitStream& inOutputStream, IMachineAddress* inFromAddress);
    
    void handlePacketFromNewClient(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress);
    
    void processPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream);
    
    void sendWelcomePacket(ClientProxy* inClientProxy);
    
    void sendLocalPlayerAddedPacket(ClientProxy* inClientProxy, int index);
    
    void sendStatePacketToClient(ClientProxy* inClientProxy);
    
    void writeLastMoveTimestampIfDirty(OutputMemoryBitStream& inOutputStream, ClientProxy* inClientProxy);
    
    void handleInputPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream);
    
    void handleAddLocalPlayerPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream);
    
    void handleDropLocalPlayerPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream);
    
    void handleClientDisconnected(ClientProxy* inClientProxy);
    
    void updateNextPlayerId();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    NetworkManagerServer(IServerHelper* inServerHelper, HandleNewClientFunc inHandleNewClientFunc, HandleLostClientFunc inHandleLostClientFunc, InputStateCreationFunc inInputStateCreationFunc);
    ~NetworkManagerServer();
    NetworkManagerServer(const NetworkManagerServer&);
    NetworkManagerServer& operator=(const NetworkManagerServer&);
};

#endif /* defined(__noctisgames__NetworkManagerServer__) */
