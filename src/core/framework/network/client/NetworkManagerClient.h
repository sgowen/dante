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
#include <unordered_map>
#include <vector>

class IClientHelper;
class InputMemoryBitStream;
class OutputMemoryBitStream;
class DeliveryNotificationManager;
class IMachineAddress;
class EntityRegistry;
class Entity;
class MoveList;
class ReplicationManagerClient;
class WeightedTimedMovingAverage;
class SocketAddress;

#define NG_CLIENT (NetworkManagerClient::getInstance())

#define NG_CLIENT_CALLBACKS NetworkManagerClient::staticProcessPacket, NetworkManagerClient::staticHandleNoResponse, NetworkManagerClient::staticHandleConnectionReset

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
    static void create(IClientHelper* inClientHelper, float inFrameRate, RemoveProcessedMovesFunc inRemoveProcessedMovesFunc, GetMoveListFunc inGetMoveListFunc, OnPlayerWelcomedFunc inOnPlayerWelcomedFunc);
    
    static void destroy();
    
    static NetworkManagerClient* getInstance();
    
    static void staticProcessPacket(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress);
    
    static void staticHandleNoResponse();
    
    static void staticHandleConnectionReset(IMachineAddress* inFromAddress);
    
    void processIncomingPackets();
    
    void sendOutgoingPackets();
    
    void requestToAddLocalPlayer();
    
    void requestToDropLocalPlayer(int index);
    
    const WeightedTimedMovingAverage& getBytesReceivedPerSecond() const;
    
    const WeightedTimedMovingAverage& getBytesSentPerSecond() const;
    
    const WeightedTimedMovingAverage& getAvgRoundTripTime()	const;
    
    float getRoundTripTime() const;
    
    bool isPlayerIdLocal(uint8_t inPlayerId) const;
    
    std::unordered_map<uint8_t, uint8_t>& getPlayerIds();
    
    std::string& getPlayerName();
    
    NetworkClientState getState() const;
    
private:
    static NetworkManagerClient* s_instance;
    
    IClientHelper* m_clientHelper;    
    
    RemoveProcessedMovesFunc m_removeProcessedMovesFunc;
    GetMoveListFunc m_getMoveListFunc;
    OnPlayerWelcomedFunc m_onPlayerWelcomedFunc;
    
    DeliveryNotificationManager* m_deliveryNotificationManager;
    ReplicationManagerClient* m_replicationManagerClient;
    
    NetworkClientState m_state;
    
    float m_fTimeOfLastHello;
    float m_fTimeOfLastInputPacket;
    
    std::unordered_map<uint8_t, uint8_t> m_indexToPlayerIdMap;
    uint8_t m_iNextIndex;
    float m_fFrameRate;
    
    float m_fLastMoveProcessedByServerTimestamp;
    float m_fLastServerCommunicationTimestamp;
    
    WeightedTimedMovingAverage* m_avgRoundTripTime;
    
    bool m_isRequestingToAddLocalPlayer;
    int m_isRequestingToDropLocalPlayer;
    
    void processPacket(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress);
    
    void handleNoResponse();
    
    void handleConnectionReset(IMachineAddress* inFromAddress);
    
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
    NetworkManagerClient(IClientHelper* inClientHelper, float inFrameRate, RemoveProcessedMovesFunc inRemoveProcessedMovesFunc, GetMoveListFunc inGetMoveListFunc, OnPlayerWelcomedFunc inOnPlayerWelcomedFunc);
    ~NetworkManagerClient();
    NetworkManagerClient(const NetworkManagerClient&);
    NetworkManagerClient& operator=(const NetworkManagerClient&);
};

#endif /* defined(__noctisgames__NetworkManagerClient__) */
