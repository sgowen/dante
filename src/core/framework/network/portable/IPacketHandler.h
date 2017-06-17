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
typedef void (*ProcessPacketFunc)(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress);

class IPacketHandler
{
public:
    IPacketHandler();
    
    virtual ~IPacketHandler();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream, IMachineAddress* inFromAddress) = 0;
    
    virtual void readIncomingPacketsIntoQueue() = 0;
    
    virtual void processQueuedPackets() = 0;
    
    void updateBytesSentLastFrame();
    
    void updateBytesReceivedLastFrame(int totalReadByteCount);
    
    const WeightedTimedMovingAverage& getBytesReceivedPerSecond() const;
    
    const WeightedTimedMovingAverage& getBytesSentPerSecond() const;
    
protected:
    int m_bytesSentThisFrame;
    
private:
    WeightedTimedMovingAverage* m_bytesReceivedPerSecond;
    WeightedTimedMovingAverage* m_bytesSentPerSecond;
};

#endif /* defined(__noctisgames__IPacketHandler__) */
