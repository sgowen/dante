//
//  SocketAddressFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SocketAddressFactory.h"

#include "SocketAddress.h"

#include "SocketUtil.h"

#include <cstring>	// memset()

SocketAddress* SocketAddressFactory::createIPv4FromString(const std::string& inString)
{
    auto pos = inString.find_last_of(':');
    std::string host, service;
    if (pos != std::string::npos)
    {
        host = inString.substr(0, pos);
        service = inString.substr(pos + 1);
    }
    else
    {
        host = inString;
        //use default port...
        service = "0";
    }
    addrinfo hint;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    
    addrinfo* result = nullptr;
    int error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
    if (error != 0 && result != nullptr)
    {
        SOCKET_UTIL->reportError("SocketAddressFactory::createIPv4FromString");
        return nullptr;
    }
    
    if (result == nullptr)
    {
        SOCKET_UTIL->reportError("SocketAddressFactory::createIPv4FromString");
        return nullptr;
    }
    
    while (!result->ai_addr && result->ai_next)
    {
        result = result->ai_next;
    }
    
    if (!result->ai_addr)
    {
        return nullptr;
    }
    
    auto toRet = new SocketAddress(*result->ai_addr);
    
    freeaddrinfo(result);
    
    return toRet;
}
