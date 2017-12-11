//
//  NetworkManagerClient.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NetworkManagerClient__
#define __noctisgames__NetworkManagerClient__

#include <string>
#include <map>
#include <vector>

class ClientHelper;
class InputMemoryBitStream;
class OutputMemoryBitStream;
class DeliveryNotificationManager;
class MachineAddress;
class EntityRegistry;
class Entity;
class MoveList;
class ReplicationManagerClient;
class WeightedTimedMovingAverage;
class SocketAddress;

#define NG_CLIENT (NetworkManagerClient::getInstance())

#define NG_CLIENT_CALLBACKS NetworkManagerClient::sProcessPacket, NetworkManagerClient::sHandleNoResponse, NetworkManagerClient::sHandleConnectionReset

typedef void (*RemoveProcessedMovesFunc)(float lastMoveProcessedByServerTimestamp);
typedef MoveList& (*GetMoveListFunc)();
typedef void (*OnPlayerWelcomedFunc)(uint8_t playerId);

enum NetworkClientState
{
    NCS_Uninitialized,
    NCS_SayingHello,
    NCS_Welcomed,
    NCS_Disconnected
};

class NetworkManagerClient
{
public:
    static void create(ClientHelper* inClientHelper, RemoveProcessedMovesFunc inRemoveProcessedMovesFunc, GetMoveListFunc inGetMoveListFunc, OnPlayerWelcomedFunc inOnPlayerWelcomedFunc);
    
    static void destroy();
    
    static NetworkManagerClient* getInstance();
    
    static void sProcessPacket(InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress);
    
    static void sHandleNoResponse();
    
    static void sHandleConnectionReset(MachineAddress* inFromAddress);
    
    void processIncomingPackets();
    
    void sendOutgoingPackets();
    
    void requestToAddLocalPlayer();
    
    void requestToDropLocalPlayer(uint8_t index);
    
    const WeightedTimedMovingAverage& getBytesReceivedPerSecond() const;
    
    const WeightedTimedMovingAverage& getBytesSentPerSecond() const;
    
    const WeightedTimedMovingAverage& getAvgRoundTripTime()	const;
    
    float getRoundTripTime() const;
    
    bool isPlayerIdLocal(uint8_t inPlayerId) const;
    
    std::map<uint8_t, uint8_t>& getPlayerIds();
    
    std::string& getPlayerName();
    
    NetworkClientState getState() const;
    
private:
    static NetworkManagerClient* s_instance;
    
    ClientHelper* _clientHelper;    
    
    RemoveProcessedMovesFunc _removeProcessedMovesFunc;
    GetMoveListFunc _getMoveListFunc;
    OnPlayerWelcomedFunc _onPlayerWelcomedFunc;
    
    DeliveryNotificationManager* _deliveryNotificationManager;
    ReplicationManagerClient* _replicationManagerClient;
    
    NetworkClientState _state;
    
    float _timeOfLastHello;
    
    std::map<uint8_t, uint8_t> _indexToPlayerIdMap;
    uint8_t _nextIndex;
    float _frameRate;
    
    float _lastMoveProcessedByServerTimestamp;
    float _lastServerCommunicationTimestamp;
    
    WeightedTimedMovingAverage* _avgRoundTripTime;
    
    bool _isRequestingToAddLocalPlayer;
    uint8_t _isRequestingToDropLocalPlayer;
    
    void processPacket(InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress);
    
    void handleNoResponse();
    
    void handleConnectionReset(MachineAddress* inFromAddress);
    
    void sendPacket(const OutputMemoryBitStream& inOutputStream);
    
    void updateSayingHello();
    
    void handleWelcomePacket(InputMemoryBitStream& inInputStream);
    
    void handleLocalPlayerAddedPacket(InputMemoryBitStream& inInputStream);
    
    void handleLocalPlayerDeniedPacket();
    
    void handleStatePacket(InputMemoryBitStream& inInputStream);
    
    void readLastMoveProcessedOnServerTimestamp(InputMemoryBitStream& inInputStream);
    
    void updateSendingInputPacket();
    
    void sendInputPacket();
    
    void updateAddLocalPlayerRequest();
    
    void updateDropLocalPlayerRequest();
    
    void updateNextIndex();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    NetworkManagerClient(ClientHelper* inClientHelper, RemoveProcessedMovesFunc inRemoveProcessedMovesFunc, GetMoveListFunc inGetMoveListFunc, OnPlayerWelcomedFunc inOnPlayerWelcomedFunc);
    ~NetworkManagerClient();
    NetworkManagerClient(const NetworkManagerClient&);
    NetworkManagerClient& operator=(const NetworkManagerClient&);
};

#endif /* defined(__noctisgames__NetworkManagerClient__) */
