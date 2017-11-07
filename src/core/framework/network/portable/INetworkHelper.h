//
//  INetworkHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__INetworkHelper__
#define __noctisgames__INetworkHelper__

#include <cstdint>

class InputMemoryBitStream;
class OutputMemoryBitStream;
class IMachineAddress;
class WeightedTimedMovingAverage;
class IPacketHandler;

class INetworkHelper
{
public:
    INetworkHelper(IPacketHandler* packetHandler);
    
    virtual ~INetworkHelper();
    
    virtual void processSpecialPacket(uint32_t packetType, InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress) = 0;
    
    virtual void processIncomingPackets();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream, IMachineAddress* inFromAddress);
    
    const WeightedTimedMovingAverage& getBytesReceivedPerSecond() const;
    
    const WeightedTimedMovingAverage& getBytesSentPerSecond() const;
    
protected:
    IPacketHandler* m_packetHandler;
};

#endif /* defined(__noctisgames__INetworkHelper__) */
