//
//  UDPSocket.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__UDPSocket__
#define __noctisgames__UDPSocket__

#include "NetworkConstants.h"

class UDPSocket
{
public:
	~UDPSocket();

	int Bind(const SocketAddress& inToAddress);
	int SendTo(const void* inToSend, int inLength, const SocketAddress& inToAddress);
	int ReceiveFrom(void* inToReceive, int inMaxLength, SocketAddress& outFromAddress);

	int SetNonBlockingMode(bool inShouldBeNonBlocking);
private:
	friend class SocketUtil;
	UDPSocket(SOCKET inSocket) : mSocket(inSocket) {}
	SOCKET mSocket;
};

typedef std::shared_ptr< UDPSocket > UDPSocketPtr;

#endif /* defined(__noctisgames__UDPSocket__) */
