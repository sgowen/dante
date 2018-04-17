//
//  NGSteamPacketHandler.h
//  noctisgames
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamPacketHandler__
#define __noctisgames__NGSteamPacketHandler__

#include <framework/network/portable/PacketHandler.h>

#include <framework/network/portable/InputMemoryBitStream.h>
#include <framework/network/steam/NGSteamAddress.h>

#include <queue>
#include <list>

class OutputMemoryBitStream;
class MachineAddress;

class NGSteamPacketHandler : public PacketHandler
{
public:
    NGSteamPacketHandler(Timing* timing, bool isServer, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc);
    virtual ~NGSteamPacketHandler();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream, MachineAddress* inFromAddress);

protected:
    virtual void readIncomingPacketsIntoQueue();
    virtual void processQueuedPackets();
    
private:
    class ReceivedPacket;
    std::queue<ReceivedPacket, std::list<ReceivedPacket> > _packetQueue;
    
    class ReceivedPacket
    {
    public:
        ReceivedPacket(float inReceivedTime, InputMemoryBitStream& inInputMemoryBitStream, NGSteamAddress inFromAddress);
        
        NGSteamAddress& getFromAddress();
        
        float getReceivedTime()	const;
        
        InputMemoryBitStream& getPacketBuffer();
        
    private:
        float _receivedTime;
        InputMemoryBitStream _packetBuffer;
        NGSteamAddress _fromAddress;
    };
};

#endif /* defined(__noctisgames__NGSteamPacketHandler__) */
