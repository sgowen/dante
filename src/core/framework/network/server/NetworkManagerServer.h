//
//  NetworkManagerServer.h
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NetworkManagerServer__
#define __noctisgames__NetworkManagerServer__

#include <framework/entity/EntityManager.h>
#include <framework/network/server/ReplicationManagerTransmissionData.h>
#include <framework/util/NGPool.h>

#include <map>

class ServerHelper;
class InputMemoryBitStream;
class OutputMemoryBitStream;
class DeliveryNotificationManager;
class MachineAddress;
class ClientProxy;
class InputState;
class Entity;
class Timing;

#define NG_SERVER (NetworkManagerServer::getInstance())

#define NG_SERVER_CALLBACKS NetworkManagerServer::sProcessPacket, NetworkManagerServer::sHandleNoResponse, NetworkManagerServer::sHandleConnectionReset, NetworkManagerServer::sGetClientProxy, NetworkManagerServer::sHandleClientDisconnected

typedef void (*HandleNewClientFunc)(uint8_t playerId, std::string playerName);
typedef void (*HandleLostClientFunc)(ClientProxy* inClientProxy, uint8_t index);
typedef InputState* (*InputStateCreationFunc)();
typedef void (*InputStateReleaseFunc)(InputState* inputState);

class NetworkManagerServer
{
public:
    static void create(ServerHelper* inServerHelper, HandleNewClientFunc inHandleNewClientFunc, HandleLostClientFunc inHandleLostClientFunc, InputStateCreationFunc inInputStateCreationFunc, InputStateReleaseFunc inputStateReleaseFunc);
    static NetworkManagerServer* getInstance();
    static void destroy();
    
    static void sProcessPacket(InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress);
    static void sHandleNoResponse();
    static void sHandleConnectionReset(MachineAddress* inFromAddress);
    static ClientProxy* sGetClientProxy(uint8_t inPlayerId);
    static void sHandleClientDisconnected(ClientProxy* inClientProxy);
    
    void processIncomingPackets();
    void sendOutgoingPackets();
    void registerEntity(Entity* e);
    void deregisterEntity(Entity* e);
    void setStateDirty(uint32_t inNetworkId, uint16_t inDirtyState);
    ClientProxy* getClientProxy(uint8_t inPlayerId) const;
    int getMoveCount();
    int getAverageMoveCount() const;
    int getLowestNonHostMoveCount() const;
    int getHostMoveCount() const;
    uint8_t getNumClientsConnected();
    MachineAddress* getServerAddress();
    bool isConnected();
    ServerHelper* getServerHelper();
    void setMap(uint32_t inValue);
    EntityManager* getEntityManager();
    
private:
    static NetworkManagerServer* s_instance;
    
    ServerHelper* _serverHelper;
    HandleNewClientFunc _handleNewClientFunc;
    HandleLostClientFunc _handleLostClientFunc;
    InputStateCreationFunc _inputStateCreationFunc;
    InputStateReleaseFunc _inputStateReleaseFunc;
    EntityManager* _entityManager;
    Timing* _timing;
    NoctisGames::NGPool<ReplicationManagerTransmissionData> _replicationManagerTransmissionDatas;
    std::map<size_t, ClientProxy*> _addressHashToClientMap;
    std::map<int, ClientProxy*> _playerIDToClientMap;
    uint8_t _nextPlayerId;
    uint32_t _map;
    
    void processPacket(InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress);
    void handleNoResponse();
    void handleConnectionReset(MachineAddress* inFromAddress);
    void sendPacket(const OutputMemoryBitStream& inOutputStream, MachineAddress* inFromAddress);
    void handlePacketFromNewClient(InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress);
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
    NetworkManagerServer(ServerHelper* inServerHelper, HandleNewClientFunc inHandleNewClientFunc, HandleLostClientFunc inHandleLostClientFunc, InputStateCreationFunc inInputStateCreationFunc, InputStateReleaseFunc inputStateReleaseFunc);
    ~NetworkManagerServer();
    NetworkManagerServer(const NetworkManagerServer&);
    NetworkManagerServer& operator=(const NetworkManagerServer&);
};

#endif /* defined(__noctisgames__NetworkManagerServer__) */
