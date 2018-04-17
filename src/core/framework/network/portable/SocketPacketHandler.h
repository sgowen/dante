//
//  SocketPacketHandler.h
//  noctisgames
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SocketPacketHandler__
#define __noctisgames__SocketPacketHandler__

#include <framework/network/portable/PacketHandler.h>

#include <framework/network/portable/InputMemoryBitStream.h>
#include <framework/network/portable/SocketAddress.h>

#include <queue>
#include <list>

class OutputMemoryBitStream;
class UDPSocket;
class MachineAddress;

class SocketPacketHandler : public PacketHandler
{
public:
    SocketPacketHandler(Timing* timing, bool isServer, uint16_t inPort, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc);
    virtual ~SocketPacketHandler();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream, MachineAddress* inFromAddress);
    
    SocketAddress* getSocketAddress();
    bool isInitialized();
    
protected:
    virtual void readIncomingPacketsIntoQueue();
    virtual void processQueuedPackets();
    
private:
    SocketAddress* _socketAddress;
    UDPSocket* _socket;
    
    class ReceivedPacket;
    std::queue<ReceivedPacket, std::list<ReceivedPacket> > _packetQueue;
    
    bool _isInitialized;
    
    class ReceivedPacket
    {
    public:
        ReceivedPacket(float inReceivedTime, InputMemoryBitStream& inInputMemoryBitStream, SocketAddress inFromAddress);
        
        SocketAddress& getFromAddress();
        
        float getReceivedTime()	const;
        
        InputMemoryBitStream& getPacketBuffer();
        
    private:
        float _receivedTime;
        InputMemoryBitStream _packetBuffer;
        SocketAddress _fromAddress;
    };
};

#endif /* defined(__noctisgames__SocketPacketHandler__) */
