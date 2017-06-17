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

IMachineAddress* SocketAddress::createCopy()
{
    return new SocketAddress(m_sockAddr);
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
	static char buffer[256];
#ifdef _WIN32
	USHORT port;
#else
	in_port_t port;
#endif
	
	switch (m_sockAddr.sa_family)
	{
	case AF_INET:
	{
		const sockaddr_in *addr_in = getAsSockAddrIn();
#ifdef _WIN32
		void *addr = (void*) &(addr_in->sin_addr);
		inet_ntop(AF_INET, addr, buffer, INET_ADDRSTRLEN);
#else
		inet_ntop(AF_INET, &(addr_in->sin_addr), buffer, INET_ADDRSTRLEN);
#endif
		port = addr_in->sin_port;
	}
		break;
	case AF_INET6:
	{
		const sockaddr_in6 *addr_in6 = getAsSockAddrIn6();
#ifdef _WIN32
		void *addr = (void*) &(addr_in6->sin6_addr);
		inet_ntop(AF_INET6, addr, buffer, INET6_ADDRSTRLEN);
#else
		inet_ntop(AF_INET6, &(addr_in6->sin6_addr), buffer, INET6_ADDRSTRLEN);
#endif
		port = addr_in6->sin6_port;
	}
		break;
	default:
		break;
	}

	return StringUtil::format("%s:%d", buffer, ntohs(port));
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
