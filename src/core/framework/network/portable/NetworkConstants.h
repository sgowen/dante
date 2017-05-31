//
//  NetworkConstants.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NetworkConstants__
#define __noctisgames__NetworkConstants__

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX

    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>

    // Need to link with Ws2_32.lib
    #pragma comment (lib, "Ws2_32.lib")

    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <sys/types.h>
    #include <sys/select.h>
    #include <netdb.h>
    #include <errno.h>
    #include <fcntl.h>
    #include <unistd.h>

    typedef int SOCKET;
    const int NO_ERROR = 0;
    const int INVALID_SOCKET = -1;
    const int WSAECONNRESET = ECONNRESET;
    const int WSAEWOULDBLOCK = EAGAIN;
    const int SOCKET_ERROR = -1;
#endif

//in case we decide to change the type of the sequence number to use fewer or more bits
typedef uint16_t PacketSequenceNumber;

#define NETWORK_TYPE_GameObject 'GOBJ'
#define NETWORK_TYPE_RoboCat 'PLYR'

#define NETWORK_TYPE_DECL(inCode) \
public: \
    enum { kClassId = inCode }; \
    virtual uint32_t getNetworkType();

#define NETWORK_TYPE_IMPL(name) \
uint32_t name::getNetworkType() { return kClassId; }

#endif /* defined(__noctisgames__NetworkConstants__) */
