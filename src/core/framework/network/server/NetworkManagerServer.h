//
//  NetworkManagerServer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NetworkManagerServer__
#define __noctisgames__NetworkManagerServer__

#include <map>

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

typedef void (*HandleNewClientFunc)(uint8_t playerId, std::string playerName);
typedef void (*HandleLostClientFunc)(ClientProxy* inClientProxy, uint8_t index);
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
    
    static ClientProxy* sGetClientProxy(uint8_t inPlayerId);
    
    static void sHandleClientDisconnected(ClientProxy* inClientProxy);
    
    void processIncomingPackets();
    
    void checkForDisconnects();
    
    void sendOutgoingPackets();
    
    void registerEntity(Entity* inEntity);
    
    void deregisterEntity(Entity* inEntity);
    
    void setStateDirty(uint32_t inNetworkId, uint32_t inDirtyState);
    
    ClientProxy* getClientProxy(uint8_t inPlayerId) const;
    
    uint16_t getAverageMoveCount() const;
    
    int16_t getLowestNonHostMoveCount() const;
    
    uint16_t getHostMoveCount() const;
    
    uint8_t getNumClientsConnected();
    
    IMachineAddress* getServerAddress();
    
    bool isConnected();
    
    IServerHelper* getServerHelper();
    
private:
    static NetworkManagerServer* s_instance;
    
    IServerHelper* _serverHelper;
    
    HandleNewClientFunc _handleNewClientFunc;
    HandleLostClientFunc _handleLostClientFunc;
    InputStateCreationFunc _inputStateCreationFunc;
    
    std::map<size_t, ClientProxy*> _addressHashToClientMap;
    std::map<int, ClientProxy*> _playerIDToClientMap;
    uint8_t _nextPlayerId;
    
    void processPacket(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress);
    
    void handleNoResponse();
    
    void handleConnectionReset(IMachineAddress* inFromAddress);
    
    void sendPacket(const OutputMemoryBitStream& inOutputStream, IMachineAddress* inFromAddress);
    
    void handlePacketFromNewClient(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress);
    
    void processPacket(ClientProxy* inClientProxy, InputMemoryBitStream& inInputStream);
    
    void sendWelcomePacket(ClientProxy* inClientProxy);
    
    void sendLocalPlayerAddedPacket(ClientProxy* inClientProxy, uint8_t index);
    
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
