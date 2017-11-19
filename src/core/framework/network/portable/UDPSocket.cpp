//
//  UDPSocket.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/network/portable/UDPSocket.h"

#include "framework/network/portable/SocketAddress.h"

#include "framework/network/portable/SocketUtil.h"
#include "framework/util/StringUtil.h"

int UDPSocket::bindSocket(const SocketAddress& inBindAddress)
{
    int error = bind(_socket, &inBindAddress._sockAddr, inBindAddress.getSize());
    if (error != 0)
    {
        SOCKET_UTIL->reportError("UDPSocket::Bind");
        
        return SOCKET_UTIL->getLastError();
    }
    
    return NO_ERROR;
}

int UDPSocket::sendToAddress(const void* inToSend, int inLength, const SocketAddress& inToAddress)
{
    long byteSentCount = sendto(_socket,
                                static_cast<const char*>(inToSend),
                                inLength,
                                0, &inToAddress._sockAddr, inToAddress.getSize());
    
    if (byteSentCount <= 0)
    {
        //we'll return error as negative number to indicate less than requested amount of bytes sent...
        SOCKET_UTIL->reportError("UDPSocket::SendTo");
        
        LOG("Error UDPSocket::SendTo %s", inToAddress.toString().c_str());
        
        return -SOCKET_UTIL->getLastError();
    }
    else
    {
        return static_cast<int>(byteSentCount);
    }
}

int UDPSocket::receiveFromAddress(void* inToReceive, int inMaxLength, SocketAddress& outFromAddress)
{
    socklen_t fromLength = outFromAddress.getSize();
    
    long readByteCount = recvfrom(_socket,
                                  static_cast<char*>(inToReceive),
                                  inMaxLength,
                                  0, &outFromAddress._sockAddr, &fromLength);
    
    if (readByteCount >= 0)
    {
        return static_cast<int>(readByteCount);
    }
    else
    {
        int error = SOCKET_UTIL->getLastError();
        
        if (error == WSAEWOULDBLOCK)
        {
            return 0;
        }
        else if (error == WSAECONNRESET)
        {
            //this can happen if a client closed and we haven't DC'd yet.
            //this is the ICMP message being sent back saying the port on that computer is closed
            LOG("Connection reset from %s", outFromAddress.toString().c_str());
            
            return -WSAECONNRESET;
        }
        else
        {
            SOCKET_UTIL->reportError("UDPSocket::ReceiveFrom");
            
            return -error;
        }
    }
}

int UDPSocket::setNonBlockingMode(bool inShouldBeNonBlocking)
{
#if _WIN32
    u_long arg = inShouldBeNonBlocking ? 1 : 0;
    int result = ioctlsocket(_socket, FIONBIO, &arg);
#else
    int flags = fcntl(_socket, F_GETFL, 0);
    flags = inShouldBeNonBlocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    int result = fcntl(_socket, F_SETFL, flags);
#endif
    
    if (result == SOCKET_ERROR)
    {
        SOCKET_UTIL->reportError("UDPSocket::setNonBlockingMode");
        
        return SOCKET_UTIL->getLastError();
    }
    else
    {
        return NO_ERROR;
    }
}

UDPSocket::UDPSocket(SOCKET inSocket) : _socket(inSocket)
{
    // Empty
}

UDPSocket::~UDPSocket()
{
#if _WIN32
    closesocket(_socket);
#else
    close(_socket);
#endif
}
