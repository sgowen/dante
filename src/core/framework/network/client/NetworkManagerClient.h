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
    enum NetworkClientState
    {
        NCS_Uninitialized,
        NCS_SayingHello,
        NCS_Welcomed
    };
    
public:
    static NetworkManagerClient* sInstance;
    
    static void	StaticInit(const SocketAddress& inServerAddress, const std::string& inName);
    
    void SendOutgoingPackets();
    
    virtual void ProcessPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress) override;
    
    const WeightedTimedMovingAverage& GetAvgRoundTripTime()	const { return mAvgRoundTripTime; }
    float GetRoundTripTime() const { return mAvgRoundTripTime.GetValue(); }
    int GetPlayerId() const	{ return mPlayerId; }
    float GetLastMoveProcessedByServerTimestamp() const	{ return mLastMoveProcessedByServerTimestamp; }
    
private:
    DeliveryNotificationManager mDeliveryNotificationManager;
    ReplicationManagerClient mReplicationManagerClient;
    
    SocketAddress mServerAddress;
    
    NetworkClientState mState;
    
    float mTimeOfLastHello;
    float mTimeOfLastInputPacket;
    
    std::string mName;
    int mPlayerId;
    
    float mLastMoveProcessedByServerTimestamp;
    
    WeightedTimedMovingAverage mAvgRoundTripTime;
    float mLastRoundTripTime;
    
    NetworkManagerClient();
    
    void Init(const SocketAddress& inServerAddress, const std::string& inName);
    
    void UpdateSayingHello();
    void SendHelloPacket();
    
    void HandleWelcomePacket(InputMemoryBitStream& inInputStream);
    void HandleStatePacket(InputMemoryBitStream& inInputStream);
    void ReadLastMoveProcessedOnServerTimestamp(InputMemoryBitStream& inInputStream);
    
    void HandleGameObjectState(InputMemoryBitStream& inInputStream);
    
    void UpdateSendingInputPacket();
    void SendInputPacket();
    
    void DestroyGameObjectsInMap(const IntToGameObjectMap& inObjectsToDestroy);
};

#endif /* defined(__noctisgames__NetworkManagerClient__) */
