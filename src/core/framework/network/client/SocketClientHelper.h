//
//  SocketClientHelper.h
//  noctisgames
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SocketClientHelper__
#define __noctisgames__SocketClientHelper__

#include <framework/network/client/ClientHelper.h>

#include <framework/network/portable/PacketHandler.h>

class SocketAddress;

class SocketClientHelper : public ClientHelper
{
public:
    SocketClientHelper(std::string inServerIPAddress, std::string inName, uint16_t inPort, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc);
    virtual ~SocketClientHelper();
    
    virtual void processSpecialPacket(uint8_t packetType, InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress);
    virtual void handleUninitialized();
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream);
    virtual std::string& getName();
    
private:
    SocketAddress* _serverAddress;
    std::string _name;
    
    void updateState();
};

#endif /* defined(__noctisgames__SocketClientHelper__) */
