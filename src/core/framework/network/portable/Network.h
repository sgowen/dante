//
//  Network.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Network__
#define __noctisgames__Network__

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX

    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>

    // Need to link with Ws2_32.lib
    #pragma comment (lib, "Ws2_32.lib")

    typedef int socklen_t;

    #define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <sys/types.h>
    #include <sys/select.h>
    #include <arpa/inet.h>
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

#ifndef htonll
#define htonll(x) \
((__uint64_t)((((__uint64_t)(x) & 0xff00000000000000ULL) >> 56) | \
(((__uint64_t)(x) & 0x00ff000000000000ULL) >> 40) | \
(((__uint64_t)(x) & 0x0000ff0000000000ULL) >> 24) | \
(((__uint64_t)(x) & 0x000000ff00000000ULL) >>  8) | \
(((__uint64_t)(x) & 0x00000000ff000000ULL) <<  8) | \
(((__uint64_t)(x) & 0x0000000000ff0000ULL) << 24) | \
(((__uint64_t)(x) & 0x000000000000ff00ULL) << 40) | \
(((__uint64_t)(x) & 0x00000000000000ffULL) << 56)))
#endif

#ifndef ntohll
#define ntohll(x) \
((__uint64_t)((((__uint64_t)(x) & 0xff00000000000000ULL) >> 56) | \
(((__uint64_t)(x) & 0x00ff000000000000ULL) >> 40) | \
(((__uint64_t)(x) & 0x0000ff0000000000ULL) >> 24) | \
(((__uint64_t)(x) & 0x000000ff00000000ULL) >>  8) | \
(((__uint64_t)(x) & 0x00000000ff000000ULL) <<  8) | \
(((__uint64_t)(x) & 0x0000000000ff0000ULL) << 24) | \
(((__uint64_t)(x) & 0x000000000000ff00ULL) << 40) | \
(((__uint64_t)(x) & 0x00000000000000ffULL) << 56)))
#endif

inline float float_swap(float value, bool isHostToNetwork)
{
    union v
    {
        float f;
        unsigned int i;
    };
    
    union v val;
    
    val.f = value;
    val.i = isHostToNetwork ? htonl(val.i) : ntohl(val.i);
    
    return val.f;
}

#endif /* defined(__noctisgames__Network__) */
