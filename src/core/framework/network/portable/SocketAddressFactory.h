//
//  SocketAddressFactory.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SocketAddressFactory__
#define __noctisgames__SocketAddressFactory__

#include <string>

class SocketAddress;

class SocketAddressFactory
{
public:
    static SocketAddress* CreateIPv4FromString(const std::string& inString);
};

#endif /* defined(__noctisgames__SocketAddressFactory__) */
