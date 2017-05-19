//
//  RoboCatShared.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RoboCatShared__
#define __noctisgames__RoboCatShared__

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX

	#include <windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>

	// Need to link with Ws2_32.lib
	#pragma comment (lib, "Ws2_32.lib")

	typedef int socklen_t;
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <sys/types.h>
	#include <sys/select.h>
	#include <netdb.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <unistd.h>

	typedef int SOCKET;
	const int NO_ERROR = 0;
	const int INVALID_SOCKET = -1;
	const int WSAECONNRESET = ECONNRESET;
	const int WSAEWOULDBLOCK = EAGAIN;
	const int SOCKET_ERROR = -1;
#endif

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <list>
#include <queue>
#include <deque>
#include <unordered_set>
#include <cassert>
#include <math.h>
#include <cstring>

#include "RoboMath.h"

#include "StringUtils.h"
#include "SocketAddress.h"
#include "SocketAddressFactory.h"
#include "UDPSocket.h"
#include "SocketUtil.h"

#include "ByteSwap.h"
#include "LinkingContext.h"
#include "MemoryBitStream.h"

#include "TransmissionData.h"
#include "InFlightPacket.h"
#include "AckRange.h"
#include "DeliveryNotificationManager.h"

#include "InputAction.h"
#include "InputState.h"
#include "Move.h"
#include "MoveList.h"

#include "GameObject.h"
#include "GameObjectRegistry.h"
#include "RoboCat.h"
#include "World.h"
#include "Timing.h"
#include "StringUtils.h"
#include "ScoreBoardManager.h"

#include "WeightedTimedMovingAverage.h"
#include "ReplicationCommand.h"
#include "NetworkManager.h"
#include "Engine.h"

#endif /* defined(__noctisgames__RoboCatShared__) */
