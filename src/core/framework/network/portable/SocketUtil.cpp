//
//  SocketUtil.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SocketUtil.h"

#include "Network.h"
#include "StringUtil.h"

SocketUtil* SocketUtil::getInstance()
{
    static SocketUtil instance = SocketUtil();
    return &instance;
}

bool SocketUtil::init()
{
    if (m_isInitialized)
    {
        return true;
    }
    
#if _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
    {
        reportError("Starting Up");
        return false;
    }
#endif
    
    m_isInitialized = true;
    
    return true;
}

void SocketUtil::reportError(const char* inOperationDesc)
{
#if _WIN32
    LPVOID lpMsgBuf;
    DWORD errorNum = getLastError();
    
    FormatMessage(
                  FORMAT_MESSAGE_ALLOCATE_BUFFER |
                  FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  errorNum,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR) &lpMsgBuf,
                  0, NULL);
    
    
    LOG("Error %s: %d- %s", inOperationDesc, errorNum, lpMsgBuf);
#else
    LOG("Error: %hs", inOperationDesc);
#endif
}

int SocketUtil::getLastError()
{
#if _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

UDPSocket* SocketUtil::createUDPSocket(SocketAddressFamily inFamily)
{
    SOCKET s = socket(inFamily, SOCK_DGRAM, IPPROTO_UDP);
    
    if (s != INVALID_SOCKET)
    {
        return new UDPSocket(s);
    }
    else
    {
        reportError("SocketUtil::createUDPSocket");
        
        return nullptr;
    }
}

SocketUtil::SocketUtil() : m_isInitialized(false)
{
    // Empty
}

SocketUtil::~SocketUtil()
{
    if (m_isInitialized)
    {
#if _WIN32
        WSACleanup();
#endif
    }
}
