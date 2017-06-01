//
//  SocketUtil.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "SocketUtil.h"

#include "NetworkConstants.h"
#include "StringUtil.h"

bool SocketUtil::StaticInit()
{
#if _WIN32
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
    {
        ReportError ("Starting Up");
        return false;
    }
#endif
    return true;
}

void SocketUtil::CleanUp()
{
#if _WIN32
    WSACleanup();
#endif
}

void SocketUtil::ReportError(const char* inOperationDesc)
{
#if _WIN32
    LPVOID lpMsgBuf;
    DWORD errorNum = GetLastError();
    
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

int SocketUtil::GetLastError()
{
#if _WIN32
    return WSAGetLastError();
#else
    return errno;
#endif
}

UDPSocket* SocketUtil::CreateUDPSocket(SocketAddressFamily inFamily)
{
    SOCKET s = socket(inFamily, SOCK_DGRAM, IPPROTO_UDP);
    
    if (s != INVALID_SOCKET)
    {
        return new UDPSocket(s);
    }
    else
    {
        ReportError("SocketUtil::CreateUDPSocket");
        return nullptr;
    }
}
