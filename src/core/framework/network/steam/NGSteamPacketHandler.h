//
//  NGSteamPacketHandler.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamPacketHandler__
#define __noctisgames__NGSteamPacketHandler__

#include "IPacketHandler.h"

#include "InputMemoryBitStream.h"
#include "NGSteamAddress.h"

#include <queue>
#include <list>

class OutputMemoryBitStream;
class IMachineAddress;

class NGSteamPacketHandler : public IPacketHandler
{
public:
    static const int kMaxPacketsPerFrameCount = 10;
    
    NGSteamPacketHandler(ProcessPacketFunc processPacketFunc, HandleConnectionResetFunc handleConnectionResetFunc);
    virtual ~NGSteamPacketHandler();
    
    void sendPacket(const OutputMemoryBitStream& inOutputStream, IMachineAddress* inFromAddress);
    
    void readIncomingPacketsIntoQueue();
    
    void processQueuedPackets();
    
private:
    class ReceivedPacket;
    
    ProcessPacketFunc m_processPacketFunc;
    HandleConnectionResetFunc m_handleConnectionResetFunc;
    
    std::queue<ReceivedPacket, std::list<ReceivedPacket>> m_packetQueue;
    
    class ReceivedPacket
    {
    public:
        ReceivedPacket(float inReceivedTime, InputMemoryBitStream& inInputMemoryBitStream, NGSteamAddress inFromAddress);
        
        NGSteamAddress& getFromAddress();
        
        float getReceivedTime()	const;
        
        InputMemoryBitStream& getPacketBuffer();
        
    private:
        float m_fReceivedTime;
        InputMemoryBitStream m_packetBuffer;
        NGSteamAddress m_fromAddress;
    };
};

#endif /* defined(__noctisgames__NGSteamPacketHandler__) */
