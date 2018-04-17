//
//  SocketAddress.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/portable/SocketAddress.h>

#include <framework/util/StringUtil.h>

#include <cstring>	// memcpy()

IMPL_RTTI(SocketAddress, MachineAddress);

SocketAddress::SocketAddress(uint32_t inAddress, uint16_t inPort) : MachineAddress()
{
    getAsSockAddrIn()->sin_family = AF_INET;
    getIP4Ref() = htonl(inAddress);
    getAsSockAddrIn()->sin_port = htons(inPort);
}

SocketAddress::SocketAddress(const sockaddr& inSockAddr) : MachineAddress()
{
    memcpy(&_sockAddr, &inSockAddr, sizeof(sockaddr));
}

SocketAddress::SocketAddress(sockaddr& inSockAddr) : MachineAddress()
{
    memcpy(&_sockAddr, &inSockAddr, sizeof(sockaddr));
}

SocketAddress::SocketAddress() : MachineAddress()
{
    getAsSockAddrIn()->sin_family = AF_INET;
    getIP4Ref() = INADDR_ANY;
    getAsSockAddrIn()->sin_port = 0;
}

MachineAddress* SocketAddress::createNewCopy()
{
    return new SocketAddress(_sockAddr);
}

bool SocketAddress::operator==(const SocketAddress& inOther) const
{
    return (_sockAddr.sa_family == AF_INET && getAsSockAddrIn()->sin_port == inOther.getAsSockAddrIn()->sin_port) && (getIP4Ref() == inOther.getIP4Ref());
}

uint64_t SocketAddress::getHash() const
{
    return (getIP4Ref()) | ((static_cast<uint32_t>(getAsSockAddrIn()->sin_port)) <<13) | _sockAddr.sa_family;
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
	
	switch (_sockAddr.sa_family)
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

	return StringUtil::format("Socket Address %s:%d", buffer, ntohs(port));
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
    return reinterpret_cast<sockaddr_in*>(&_sockAddr);
}

const sockaddr_in* SocketAddress::getAsSockAddrIn() const
{
    return reinterpret_cast<const sockaddr_in*>(&_sockAddr);
}

sockaddr_in6* SocketAddress::getAsSockAddrIn6()
{
    return reinterpret_cast<sockaddr_in6*>(&_sockAddr);
}

const sockaddr_in6* SocketAddress::getAsSockAddrIn6() const
{
    return reinterpret_cast<const sockaddr_in6*>(&_sockAddr);
}
