//
//  SocketAddress.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "RoboCatShared.h"


string	SocketAddress::ToString() const
{
#if _WIN32
	//const sockaddr_in* s = GetAsSockAddrIn();
	//char destinationBuffer[ 128 ];
	//InetNtop(s->sin_family, const_cast< in_addr* >(&s->sin_addr), destinationBuffer, sizeof(destinationBuffer));
	//return StringUtils::Sprintf("%s:%d",
	//							destinationBuffer,
	//							ntohs(s->sin_port));
	return string("not implemented on windows for now");
#else
	//not implement on mac for now...
	return string("not implemented on mac for now");
#endif
}

