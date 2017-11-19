//
//  UDPSocket.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__UDPSocket__
#define __noctisgames__UDPSocket__

#include "framework/network/portable/Network.h"

class SocketAddress;

class UDPSocket
{
    friend class SocketUtil;
    friend class SocketPacketHandler;
    
public:
    int bindSocket(const SocketAddress& inToAddress);
    
    int sendToAddress(const void* inToSend, int inLength, const SocketAddress& inToAddress);
    
    int receiveFromAddress(void* inToReceive, int inMaxLength, SocketAddress& outFromAddress);
    
    int setNonBlockingMode(bool inShouldBeNonBlocking);
    
private:
    SOCKET _socket;
    
    // Only allow SocketUtil/SocketPacketHandler to create or delete an instance of UDPSocket
    UDPSocket(SOCKET inSocket);
    ~UDPSocket();
};

#endif /* defined(__noctisgames__UDPSocket__) */
