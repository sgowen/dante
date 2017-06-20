//
//  SocketClientHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SocketClientHelper__
#define __noctisgames__SocketClientHelper__

#include "IClientHelper.h"

#include "IPacketHandler.h"

class SocketAddress;

class SocketClientHelper : public IClientHelper
{
public:
    SocketClientHelper(std::string inServerIPAddress, std::string inName, uint16_t inPort, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc);
    
    virtual ~SocketClientHelper();
    
    virtual void processSpecialPacket(uint32_t packetType, InputMemoryBitStream& inInputStream, IMachineAddress* inFromAddress);
    
    virtual int handleUninitialized();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream);
    
    virtual std::string& getName();
    
private:
    SocketAddress* m_serverAddress;
    std::string m_name;
};

#endif /* defined(__noctisgames__SocketClientHelper__) */
