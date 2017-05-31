//
//  SocketAddress.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SocketAddress__
#define __noctisgames__SocketAddress__

#include "NetworkConstants.h"

#include <string>

class SocketAddress
{
public:
    SocketAddress(uint32_t inAddress, uint16_t inPort);
    
    SocketAddress(const sockaddr& inSockAddr);
    
    SocketAddress();
    
    bool operator==(const SocketAddress& inOther) const;
    
    size_t GetHash() const;
    
    uint32_t GetSize() const;
    
    std::string ToString() const;
    
private:
    friend class UDPSocket;
    
    sockaddr mSockAddr;
    
    uint32_t& GetIP4Ref();
    const uint32_t& GetIP4Ref() const;
    
    sockaddr_in* GetAsSockAddrIn();
    const sockaddr_in* GetAsSockAddrIn() const;
};

namespace std
{
    template<> struct hash< SocketAddress >
    {
        size_t operator()(const SocketAddress& inAddress) const
        {
            return inAddress.GetHash();
        }
    };
}

#endif /* defined(__noctisgames__SocketAddress__) */
