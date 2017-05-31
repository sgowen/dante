//
//  NetworkManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NetworkManager__
#define __noctisgames__NetworkManager__

#include "MemoryBitStream.h"

#include "GameObject.h"
#include "SocketAddress.h"
#include "UDPSocket.h"
#include "WeightedTimedMovingAverage.h"
#include "macros.h"

#include <unordered_map>
#include <queue>
#include <list>

class NetworkManager
{
public:
    static NetworkManager* sInstance;
    
    static const uint32_t	kHelloCC = 'HELO';
    static const uint32_t	kWelcomeCC = 'WLCM';
    static const uint32_t	kStateCC = 'STAT';
    static const uint32_t	kInputCC = 'INPT';
    static const int		kMaxPacketsPerFrameCount = 10;
    
    NetworkManager();
    virtual ~NetworkManager();
    
    bool Init(uint16_t inPort);
    void ProcessIncomingPackets();
    
    virtual void ProcessPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress) = 0;
    virtual void HandleConnectionReset(const SocketAddress& inFromAddress)
    {
        UNUSED(inFromAddress);
    }
    
    void	SendPacket(const OutputMemoryBitStream& inOutputStream, const SocketAddress& inFromAddress);
    
    const WeightedTimedMovingAverage& GetBytesReceivedPerSecond()	const	{ return mBytesReceivedPerSecond; }
    const WeightedTimedMovingAverage& GetBytesSentPerSecond()		const	{ return mBytesSentPerSecond; }
    
    inline	GameObject*	GetGameObject(int inNetworkId) const;
    void	AddToNetworkIdToGameObjectMap(GameObject* inGameObject);
    void	RemoveFromNetworkIdToGameObjectMap(GameObject* inGameObject);
    
protected:
    std::unordered_map<int, GameObject*> m_networkIdToGameObjectMap;
    
private:
    class ReceivedPacket
    {
    public:
        ReceivedPacket(float inReceivedTime, InputMemoryBitStream& inInputMemoryBitStream, const SocketAddress& inAddress);
        
        const	SocketAddress&			GetFromAddress()	const	{ return mFromAddress; }
        float					GetReceivedTime()	const	{ return mReceivedTime; }
        InputMemoryBitStream&	GetPacketBuffer()			{ return mPacketBuffer; }
        
    private:
        
        float					mReceivedTime;
        InputMemoryBitStream	mPacketBuffer;
        SocketAddress			mFromAddress;
        
    };
    
    void	UpdateBytesSentLastFrame();
    void	ReadIncomingPacketsIntoQueue();
    void	ProcessQueuedPackets();
    
    std::queue< ReceivedPacket, std::list< ReceivedPacket > >	mPacketQueue;
    
    UDPSocket*	mSocket;
    
    WeightedTimedMovingAverage	mBytesReceivedPerSecond;
    WeightedTimedMovingAverage	mBytesSentPerSecond;
    
    int							mBytesSentThisFrame;
};

inline GameObject* NetworkManager::GetGameObject(int inNetworkId) const
{
    auto gameObjectIt = m_networkIdToGameObjectMap.find(inNetworkId);
    if (gameObjectIt != m_networkIdToGameObjectMap.end())
    {
        return gameObjectIt->second;
    }
    
    return nullptr;
}

#endif /* defined(__noctisgames__NetworkManager__) */
