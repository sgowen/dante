//
//  NetworkManagerClient.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NetworkManagerClient__
#define __noctisgames__NetworkManagerClient__

#include "NetworkManager.h"

#include "ReplicationManagerClient.h"
#include "DeliveryNotificationManager.h"
#include "SocketAddress.h"

class NetworkManagerClient : public NetworkManager
{
public:
    static NetworkManagerClient* getInstance();
    
    static void	StaticInit(const SocketAddress& inServerAddress, const std::string& inName);
    
    void SendOutgoingPackets();
    
    virtual void ProcessPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress) override;
    
    const WeightedTimedMovingAverage& GetAvgRoundTripTime()	const { return mAvgRoundTripTime; }
    float GetRoundTripTime() const { return mAvgRoundTripTime.getValue(); }
    int GetPlayerId() const	{ return m_iPlayerId; }
    float GetLastMoveProcessedByServerTimestamp() const	{ return m_fLastMoveProcessedByServerTimestamp; }
    
private:
    enum NetworkClientState
    {
        NCS_Uninitialized,
        NCS_SayingHello,
        NCS_Welcomed
    };
    
    DeliveryNotificationManager mDeliveryNotificationManager;
    ReplicationManagerClient mReplicationManagerClient;
    
    SocketAddress mServerAddress;
    
    NetworkClientState mState;
    
    float m_fTimeOfLastHello;
    float m_fTimeOfLastInputPacket;
    
    std::string mName;
    int m_iPlayerId;
    
    float m_fLastMoveProcessedByServerTimestamp;
    
    WeightedTimedMovingAverage mAvgRoundTripTime;
    float m_fLastRoundTripTime;
    
    NetworkManagerClient();
    
    void Init(const SocketAddress& inServerAddress, const std::string& inName);
    
    void UpdateSayingHello();
    void SendHelloPacket();
    
    void HandleWelcomePacket(InputMemoryBitStream& inInputStream);
    void HandleStatePacket(InputMemoryBitStream& inInputStream);
    void ReadLastMoveProcessedOnServerTimestamp(InputMemoryBitStream& inInputStream);
    
    void HandleNWPhysicalEntityState(InputMemoryBitStream& inInputStream);
    
    void UpdateSendingInputPacket();
    void SendInputPacket();
    
    void DestroyNWPhysicalEntitysInMap(const std::unordered_map<int, NWPhysicalEntity*>& inObjectsToDestroy);
};

#endif /* defined(__noctisgames__NetworkManagerClient__) */
