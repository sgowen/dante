//
//  IPacketHandler.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__IPacketHandler__
#define __noctisgames__IPacketHandler__

#include "InputMemoryBitStream.h"
#include "SocketAddress.h"

#include <queue>
#include <list>

class OutputMemoryBitStream;
class WeightedTimedMovingAverage;
class IMachineAddress;

typedef void (*HandleConnectionResetFunc)(IMachineAddress* inFromAddress);
typedef void (*ProcessPacketFunc)(InputMemoryBitStream& inInputStream, SocketAddress* inFromAddress);

class IPacketHandler
{
public:
    static const int kMaxPacketsPerFrameCount = 10;
    
    IPacketHandler();
    
    virtual ~IPacketHandler();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream, IMachineAddress* inFromAddress) = 0;
    
    virtual void readIncomingPacketsIntoQueue() = 0;
    
    virtual void processQueuedPackets() = 0;
    
    virtual void updateBytesSentLastFrame();
    
    virtual const WeightedTimedMovingAverage& getBytesReceivedPerSecond() const;
    
    virtual const WeightedTimedMovingAverage& getBytesSentPerSecond() const;
    
protected:
    WeightedTimedMovingAverage* m_bytesReceivedPerSecond;
    WeightedTimedMovingAverage* m_bytesSentPerSecond;
    
    int m_bytesSentThisFrame;
};

#endif /* defined(__noctisgames__IPacketHandler__) */
