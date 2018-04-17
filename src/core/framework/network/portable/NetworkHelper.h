//
//  NetworkHelper.h
//  noctisgames
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NetworkHelper__
#define __noctisgames__NetworkHelper__

#include <cstdint>

class InputMemoryBitStream;
class OutputMemoryBitStream;
class MachineAddress;
class WeightedTimedMovingAverage;
class PacketHandler;

class NetworkHelper
{
public:
    NetworkHelper(PacketHandler* packetHandler);
    
    virtual ~NetworkHelper();
    
    virtual void processSpecialPacket(uint8_t packetType, InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress) = 0;
    
    virtual void processIncomingPackets();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream, MachineAddress* inFromAddress);
    
    const WeightedTimedMovingAverage& getBytesReceivedPerSecond() const;
    
    const WeightedTimedMovingAverage& getBytesSentPerSecond() const;
    
protected:
    PacketHandler* _packetHandler;
};

#endif /* defined(__noctisgames__NetworkHelper__) */
