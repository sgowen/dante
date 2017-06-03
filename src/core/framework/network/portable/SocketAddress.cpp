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

SocketAddress::SocketAddress(uint32_t inAddress, uint16_t inPort)
{
    getAsSockAddrIn()->sin_family = AF_INET;
    getIP4Ref() = htonl(inAddress);
    getAsSockAddrIn()->sin_port = htons(inPort);
}

SocketAddress::SocketAddress(const sockaddr& inSockAddr)
{
    memcpy(&m_sockAddr, &inSockAddr, sizeof(sockaddr));
}

SocketAddress::SocketAddress()
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
    return (getIP4Ref()) | ((static_cast<uint32_t>(getAsSockAddrIn()->sin_port)) << 13) | m_sockAddr.sa_family;
}

uint32_t SocketAddress::getSize() const
{
    return sizeof(sockaddr);
}

std::string	SocketAddress::toString() const
{
#if _WIN32
    //const sockaddr_in* s = getAsSockAddrIn();
    //char destinationBuffer[128];
    //InetNtop(s->sin_family, const_cast< in_addr* >(&s->sin_addr), destinationBuffer, sizeof(destinationBuffer));
    //return StringUtil::sprintf("%s:%d",
    //							destinationBuffer,
    //							ntohs(s->sin_port));
    return std::string(StringUtil::toString(getHash()));
#else
    //not implement on mac for now...
    return std::string(StringUtil::toString(getHash()));
#endif
}

#if _WIN32
uint32_t& SocketAddress::getIP4Ref()
{
    return *reinterpret_cast< uint32_t* >(&getAsSockAddrIn()->sin_addr.S_un.S_addr);
}

const uint32_t& SocketAddress::getIP4Ref() const
{
    return *reinterpret_cast< const uint32_t* >(&getAsSockAddrIn()->sin_addr.S_un.S_addr);
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
    return reinterpret_cast< sockaddr_in* >(&m_sockAddr);
}

const sockaddr_in* SocketAddress::getAsSockAddrIn() const
{
    return reinterpret_cast< const sockaddr_in* >(&m_sockAddr);
}
