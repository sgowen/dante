//
//  SocketAddress.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SocketAddress__
#define __noctisgames__SocketAddress__

#include <framework/network/portable/MachineAddress.h>

#include <framework/network/portable/Network.h>

#include <framework/util/NGRTTI.h>

#include <string>

class SocketAddress : public MachineAddress
{
    NGRTTI_DECL;
    
    friend class UDPSocket;
    
public:
    SocketAddress(uint32_t inAddress, uint16_t inPort);
    
    SocketAddress(const sockaddr& inSockAddr);
    
    SocketAddress(sockaddr& inSockAddr);
    
    SocketAddress();
    
    virtual MachineAddress* createNewCopy();
    
    bool operator==(const SocketAddress& inOther) const;
    
    virtual uint64_t getHash() const;
    
    uint32_t getSize() const;
    
    virtual std::string toString() const;
    
private:
    sockaddr _sockAddr;
    
    uint32_t& getIP4Ref();
    const uint32_t& getIP4Ref() const;
    
    sockaddr_in* getAsSockAddrIn();
    const sockaddr_in* getAsSockAddrIn() const;
    
    sockaddr_in6* getAsSockAddrIn6();
    const sockaddr_in6* getAsSockAddrIn6() const;
};

#endif /* defined(__noctisgames__SocketAddress__) */
