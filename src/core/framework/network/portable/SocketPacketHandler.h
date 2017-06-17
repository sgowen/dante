//
//  SocketPacketHandler.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SocketPacketHandler__
#define __noctisgames__SocketPacketHandler__

#include "IPacketHandler.h"

#include "InputMemoryBitStream.h"
#include "SocketAddress.h"

#include <queue>
#include <list>

class OutputMemoryBitStream;
class UDPSocket;
class IMachineAddress;

class SocketPacketHandler : public IPacketHandler
{
public:
    SocketPacketHandler(uint16_t inPort, ProcessPacketFunc processPacketFunc, HandleConnectionResetFunc handleConnectionResetFunc);
    
    virtual ~SocketPacketHandler();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream, IMachineAddress* inFromAddress);
    
    virtual void readIncomingPacketsIntoQueue();
    
    virtual void processQueuedPackets();
    
private:
    class ReceivedPacket;
    
    ProcessPacketFunc m_processPacketFunc;
    HandleConnectionResetFunc m_handleConnectionResetFunc;
    
    std::queue<ReceivedPacket, std::list<ReceivedPacket>> m_packetQueue;
    
    UDPSocket* m_socket;
    
    class ReceivedPacket
    {
    public:
        ReceivedPacket(float inReceivedTime, InputMemoryBitStream& inInputMemoryBitStream, SocketAddress inFromAddress);
        
        SocketAddress& getFromAddress();
        
        float getReceivedTime()	const;
        
        InputMemoryBitStream& getPacketBuffer();
        
    private:
        float m_fReceivedTime;
        InputMemoryBitStream m_packetBuffer;
        SocketAddress m_fromAddress;
    };
};

#endif /* defined(__noctisgames__SocketPacketHandler__) */
