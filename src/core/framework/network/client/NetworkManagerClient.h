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

typedef void (*RemoveProcessedMovesFunc)(float lastMoveProcessedByServerTimestamp);
typedef MoveList& (*GetMoveListFunc)();

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
    static void create(IClientHelper* inClientHelper, float inFrameRate, RemoveProcessedMovesFunc inRemoveProcessedMovesFunc, GetMoveListFunc inGetMoveListFunc);
    
    static void destroy();
    
    static NetworkManagerClient* getInstance();
    
    static void staticProcessPacket(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress);
    
    static void staticHandleNoResponse();
    
    static void staticHandleConnectionReset(IMachineAddress* inFromAddress);
    
    void processIncomingPackets();
    
    void sendOutgoingPackets();
    
    const WeightedTimedMovingAverage& getBytesReceivedPerSecond() const;
    
    const WeightedTimedMovingAverage& getBytesSentPerSecond() const;
    
    const WeightedTimedMovingAverage& getAvgRoundTripTime()	const;
    
    float getRoundTripTime() const;
    
    std::string& getClientUserName();
    
    int getPlayerId() const;
    
    NetworkClientState getState() const;
    
private:
    static NetworkManagerClient* s_instance;
    
    IClientHelper* m_clientHelper;    
    
    RemoveProcessedMovesFunc m_removeProcessedMovesFunc;
    GetMoveListFunc m_getMoveListFunc;
    
    DeliveryNotificationManager* m_deliveryNotificationManager;
    ReplicationManagerClient* m_replicationManagerClient;
    
    NetworkClientState m_state;
    
    float m_fTimeOfLastHello;
    float m_fTimeOfLastInputPacket;
    
    int m_iPlayerId;
    float m_fFrameRate;
    
    float m_fLastMoveProcessedByServerTimestamp;
    float m_fLastServerCommunicationTimestamp;
    
    WeightedTimedMovingAverage* m_avgRoundTripTime;
    
    void processPacket(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress);
    
    void handleNoResponse();
    
    void handleConnectionReset(IMachineAddress* inFromAddress);
    
    void sendPacket(const OutputMemoryBitStream& inOutputStream);
    
    void updateSayingHello();
    
    void sendHelloPacket();
    
    void handleWelcomePacket(InputMemoryBitStream& inInputStream);
    
    void handleStatePacket(InputMemoryBitStream& inInputStream);
    
    void readLastMoveProcessedOnServerTimestamp(InputMemoryBitStream& inInputStream);
    
    void handleEntityState(InputMemoryBitStream& inInputStream);
    
    void updateSendingInputPacket();
    
    void sendInputPacket();
    
    void destroyAllInMap(const std::unordered_map<int, Entity*>& inObjectsToDestroy);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    NetworkManagerClient(IClientHelper* inClientHelper, float inFrameRate, RemoveProcessedMovesFunc inRemoveProcessedMovesFunc, GetMoveListFunc inGetMoveListFunc);
    ~NetworkManagerClient();
    NetworkManagerClient(const NetworkManagerClient&);
    NetworkManagerClient& operator=(const NetworkManagerClient&);
};

#endif /* defined(__noctisgames__NetworkManagerClient__) */
