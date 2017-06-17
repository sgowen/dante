//
//  INetworkManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__INetworkManager__
#define __noctisgames__INetworkManager__

#include "InputMemoryBitStream.h"

#include "IPacketHandler.h"

#include <unordered_map>
#include <queue>
#include <list>

class OutputMemoryBitStream;
class WeightedTimedMovingAverage;
class SocketAddress;
class IMachineAddress;

class INetworkManager
{
public:
    static const uint32_t kHelloCC = 'HELO';
    static const uint32_t kWelcomeCC = 'WLCM';
    static const uint32_t kStateCC = 'STAT';
    static const uint32_t kInputCC = 'INPT';
    
    void processIncomingPackets();
    
    virtual void sendOutgoingPackets() = 0;
    
    const WeightedTimedMovingAverage& getBytesReceivedPerSecond() const;
    
    const WeightedTimedMovingAverage& getBytesSentPerSecond() const;
    
protected:
    virtual void processPacket(InputMemoryBitStream& inInputStream, SocketAddress* inFromAddress) = 0;
    
    virtual void handleConnectionReset(SocketAddress* inFromAddress);
    
    void sendPacket(const OutputMemoryBitStream& inOutputStream, IMachineAddress* inFromAddress);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    INetworkManager(uint16_t inPort, ProcessPacketFunc processPacketFunc, HandleConnectionResetFunc handleConnectionResetFunc);
    virtual ~INetworkManager();
    INetworkManager(const INetworkManager&);
    INetworkManager& operator=(const INetworkManager&);
    
private:
    IPacketHandler* m_packetHandler;
};

#endif /* defined(__noctisgames__INetworkManager__) */
