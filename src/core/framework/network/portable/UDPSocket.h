//
//  UDPSocket.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__UDPSocket__
#define __noctisgames__UDPSocket__

#include "Network.h"

class SocketAddress;

class UDPSocket
{
public:
    int bindSocket(const SocketAddress& inToAddress);
    
    int sendToAddress(const void* inToSend, int inLength, const SocketAddress& inToAddress);
    
    int receiveFromAddress(void* inToReceive, int inMaxLength, SocketAddress& outFromAddress);
    
    int setNonBlockingMode(bool inShouldBeNonBlocking);
    
private:
    friend class SocketUtil;
    friend class INetworkManager;
    
    SOCKET m_socket;
    
    // Only allow SocketUtil/INetworkManager to create or delete an instance of UDPSocket
    UDPSocket(SOCKET inSocket);
    ~UDPSocket();
};

#endif /* defined(__noctisgames__UDPSocket__) */
