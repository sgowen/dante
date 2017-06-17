//
//  NetworkManagerClient.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NetworkManagerClient__
#define __noctisgames__NetworkManagerClient__

#include "INetworkManager.h"

#include "DeliveryNotificationManager.h"
#include "SocketAddress.h"

class EntityRegistry;
class Entity;
class MoveList;
class ReplicationManagerClient;

#define NG_CLIENT (NetworkManagerClient::getInstance())

typedef void (*RemoveProcessedMovesFunc)(float lastMoveProcessedByServerTimestamp);
typedef MoveList& (*GetMoveListFunc)();

class NetworkManagerClient : public INetworkManager
{
public:
    static void create(const std::string& inServerIPAddress, const std::string& inName, float inFrameRate, RemoveProcessedMovesFunc removeProcessedMovesFunc, GetMoveListFunc getMoveListFunc);
    
    static void destroy();
    
    static NetworkManagerClient* getInstance();
    
    static void staticProcessPacket(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress);
    
    static void staticHandleConnectionReset(IMachineAddress* inFromAddress);
    
    virtual void processPacket(InputMemoryBitStream& inInputStream, SocketAddress* inFromAddress) override;
    
    virtual void sendOutgoingPackets();
    
    const WeightedTimedMovingAverage& getAvgRoundTripTime()	const;
    
    float getRoundTripTime() const;
    
    int getPlayerId() const;
    
private:
    static NetworkManagerClient* s_instance;
    
    enum NetworkClientState
    {
        NCS_Uninitialized,
        NCS_SayingHello,
        NCS_Welcomed
    };
    
    RemoveProcessedMovesFunc m_removeProcessedMovesFunc;
    GetMoveListFunc m_getMoveListFunc;
    
    DeliveryNotificationManager m_deliveryNotificationManager;
    ReplicationManagerClient* m_replicationManagerClient;
    
    SocketAddress* m_serverAddress;
    
    NetworkClientState m_state;
    
    float m_fTimeOfLastHello;
    float m_fTimeOfLastInputPacket;
    
    std::string m_name;
    int m_iPlayerId;
    float m_fFrameRate;
    
    float m_fLastMoveProcessedByServerTimestamp;
    
    WeightedTimedMovingAverage* m_avgRoundTripTime;
    
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
    NetworkManagerClient(const std::string& inServerIPAddress, const std::string& inName, float inFrameRate, RemoveProcessedMovesFunc removeProcessedMovesFunc, GetMoveListFunc getMoveListFunc);
    virtual ~NetworkManagerClient();
    NetworkManagerClient(const NetworkManagerClient&);
    NetworkManagerClient& operator=(const NetworkManagerClient&);
};

#endif /* defined(__noctisgames__NetworkManagerClient__) */
