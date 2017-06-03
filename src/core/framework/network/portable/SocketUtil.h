//
//  SocketUtil.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SocketUtil__
#define __noctisgames__SocketUtil__

#include "UDPSocket.h"

#include "SocketAddressFamily.h"

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
    bool m_isInitialized;
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    SocketUtil();
    ~SocketUtil();
    SocketUtil(const SocketUtil&);
    SocketUtil& operator=(const SocketUtil&);
};

#endif /* defined(__noctisgames__SocketUtil__) */
