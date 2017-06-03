//
//  SocketAddress.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SocketAddress__
#define __noctisgames__SocketAddress__

#include "Network.h"

#include <string>

class SocketAddress
{
public:
    SocketAddress(uint32_t inAddress, uint16_t inPort);
    
    SocketAddress(const sockaddr& inSockAddr);
    
    SocketAddress();
    
    bool operator==(const SocketAddress& inOther) const;
    
    size_t getHash() const;
    
    uint32_t getSize() const;
    
    std::string toString() const;
    
private:
    friend class UDPSocket;
    
    sockaddr m_sockAddr;
    
    uint32_t& getIP4Ref();
    const uint32_t& getIP4Ref() const;
    
    sockaddr_in* getAsSockAddrIn();
    const sockaddr_in* getAsSockAddrIn() const;
};

namespace std
{
    template<> struct hash< SocketAddress >
    {
        size_t operator()(const SocketAddress& inAddress) const
        {
            return inAddress.getHash();
        }
    };
}

#endif /* defined(__noctisgames__SocketAddress__) */
