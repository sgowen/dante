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

enum SocketAddressFamily
{
	INET = AF_INET,
	INET6 = AF_INET6
};

class SocketUtil
{
public:

	static bool			StaticInit();
	static void			CleanUp();

	static void			ReportError(const char* inOperationDesc);
	static int			GetLastError();

	static UDPSocketPtr	CreateUDPSocket(SocketAddressFamily inFamily);
};

#endif /* defined(__noctisgames__SocketUtil__) */
