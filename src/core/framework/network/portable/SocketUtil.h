//
//  SocketUtil.h
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SocketUtil__
#define __noctisgames__SocketUtil__

#include <framework/network/portable/UDPSocket.h>

#include <framework/network/portable/SocketAddressFamily.h>

#define SOCKET_UTIL (SocketUtil::getInstance())

class SocketUtil
{
public:
    static SocketUtil* getInstance();
    
    bool init();
    
    void reportError(const char* inOperationDesc);
    
    int getLastError();
    
    UDPSocket* createUDPSocket(SocketAddressFamily inFamily);
    
    bool isInitialized();
    
private:
    bool _isInitialized;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    SocketUtil();
    ~SocketUtil();
    SocketUtil(const SocketUtil&);
    SocketUtil& operator=(const SocketUtil&);
};

#endif /* defined(__noctisgames__SocketUtil__) */
