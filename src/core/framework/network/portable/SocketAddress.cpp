//
//  SocketAddress.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SocketAddress.h"

#include "StringUtil.h"

#include <cstring>	// memcpy()

SocketAddress::SocketAddress(uint32_t inAddress, uint16_t inPort) : IMachineAddress()
{
    getAsSockAddrIn()->sin_family = AF_INET;
    getIP4Ref() = htonl(inAddress);
    getAsSockAddrIn()->sin_port = htons(inPort);
}

SocketAddress::SocketAddress(const sockaddr& inSockAddr) : IMachineAddress()
{
    memcpy(&m_sockAddr, &inSockAddr, sizeof(sockaddr));
}

SocketAddress::SocketAddress(sockaddr& inSockAddr) : IMachineAddress()
{
    memcpy(&m_sockAddr, &inSockAddr, sizeof(sockaddr));
}

SocketAddress::SocketAddress() : IMachineAddress()
{
    getAsSockAddrIn()->sin_family = AF_INET;
    getIP4Ref() = INADDR_ANY;
    getAsSockAddrIn()->sin_port = 0;
}

bool SocketAddress::operator==(const SocketAddress& inOther) const
{
    return (m_sockAddr.sa_family == AF_INET && getAsSockAddrIn()->sin_port == inOther.getAsSockAddrIn()->sin_port) && (getIP4Ref() == inOther.getIP4Ref());
}

size_t SocketAddress::getHash() const
{
    return (getIP4Ref()) | ((static_cast<uint32_t>(getAsSockAddrIn()->sin_port)) <<13) | m_sockAddr.sa_family;
}

uint32_t SocketAddress::getSize() const
{
    return sizeof(sockaddr);
}

std::string	SocketAddress::toString() const
{
#if _WIN32
    const sockaddr_in* s = getAsSockAddrIn();
    static char buffer[256];
    InetNtop(s->sin_family, const_cast<in_addr*>(&s->sin_addr), buffer, sizeof(buffer));
    
    return StringUtil::format("%s:%d", buffer, ntohs(s->sin_port));
#else
    static char buffer[256];
    in_port_t port;
    switch(m_sockAddr.sa_family)
    {
        case AF_INET:
        {
            const sockaddr_in *addr_in = getAsSockAddrIn();
            inet_ntop(AF_INET, &(addr_in->sin_addr), buffer, INET_ADDRSTRLEN);
            port = addr_in->sin_port;
        }
            break;
        case AF_INET6:
        {
            const sockaddr_in6 *addr_in6 = getAsSockAddrIn6();
            inet_ntop(AF_INET6, &(addr_in6->sin6_addr), buffer, INET6_ADDRSTRLEN);
            port = addr_in6->sin6_port;
        }
            break;
        default:
            break;
    }
    
    return StringUtil::format("%s:%d", buffer, ntohs(port));
#endif
}

sockaddr& SocketAddress::getsockaddr()
{
    return m_sockAddr;
}

#if _WIN32
uint32_t& SocketAddress::getIP4Ref()
{
    return *reinterpret_cast<uint32_t*>(&getAsSockAddrIn()->sin_addr.S_un.S_addr);
}

const uint32_t& SocketAddress::getIP4Ref() const
{
    return *reinterpret_cast<const uint32_t*>(&getAsSockAddrIn()->sin_addr.S_un.S_addr);
}
#else
uint32_t& SocketAddress::getIP4Ref()
{
    return getAsSockAddrIn()->sin_addr.s_addr;
}

const uint32_t& SocketAddress::getIP4Ref() const
{
    return getAsSockAddrIn()->sin_addr.s_addr;
}
#endif

sockaddr_in* SocketAddress::getAsSockAddrIn()
{
    return reinterpret_cast<sockaddr_in*>(&m_sockAddr);
}

const sockaddr_in* SocketAddress::getAsSockAddrIn() const
{
    return reinterpret_cast<const sockaddr_in*>(&m_sockAddr);
}

sockaddr_in6* SocketAddress::getAsSockAddrIn6()
{
    return reinterpret_cast<sockaddr_in6*>(&m_sockAddr);
}

const sockaddr_in6* SocketAddress::getAsSockAddrIn6() const
{
    return reinterpret_cast<const sockaddr_in6*>(&m_sockAddr);
}

RTTI_IMPL(SocketAddress, IMachineAddress);
