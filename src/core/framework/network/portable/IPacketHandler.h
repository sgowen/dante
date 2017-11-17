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

#include <queue>
#include <list>

class OutputMemoryBitStream;
class WeightedTimedMovingAverage;
class IMachineAddress;

typedef void (*ProcessPacketFunc)(InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress);
typedef void (*HandleNoResponseFunc)();
typedef void (*HandleConnectionResetFunc)(IMachineAddress* inFromAddress);

class IPacketHandler
{
public:
    IPacketHandler(ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc);
    
    virtual ~IPacketHandler();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream, IMachineAddress* inFromAddress) = 0;
    
    void processIncomingPackets();
    
    const WeightedTimedMovingAverage& getBytesReceivedPerSecond() const;
    
    const WeightedTimedMovingAverage& getBytesSentPerSecond() const;
    
protected:
    ProcessPacketFunc _processPacketFunc;
    HandleNoResponseFunc _handleNoResponseFunc;
    HandleConnectionResetFunc _handleConnectionResetFunc;
    int _bytesSentThisFrame;
    
    virtual void readIncomingPacketsIntoQueue() = 0;
    
    virtual void processQueuedPackets() = 0;
    
    void updateBytesSentLastFrame();
    
    void updateBytesReceivedLastFrame(int totalReadByteCount);
    
private:
    WeightedTimedMovingAverage* _bytesReceivedPerSecond;
    WeightedTimedMovingAverage* _bytesSentPerSecond;
};

#endif /* defined(__noctisgames__IPacketHandler__) */
