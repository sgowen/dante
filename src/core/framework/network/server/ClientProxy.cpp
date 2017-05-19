//
//  ClientProxy.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Server.h"

namespace
{
	const float kRespawnDelay = 3.f;
}

ClientProxy::ClientProxy(const SocketAddress& inSocketAddress, const std::string& inName, int inPlayerId) :
mSocketAddress(inSocketAddress),
mName(inName),
mPlayerId(inPlayerId),
mDeliveryNotificationManager(false, true),
mIsLastMoveTimestampDirty(false),
mTimeToRespawn(0.f)
{
	UpdateLastPacketTime();
}


void ClientProxy::UpdateLastPacketTime()
{
	mLastPacketFromClientTime = Timing::sInstance.GetTimef(); 
}

void	ClientProxy::HandleCatDied()
{
	mTimeToRespawn = Timing::sInstance.GetFrameStartTime() + kRespawnDelay;
}

void	ClientProxy::RespawnCatIfNecessary()
{
	if (mTimeToRespawn != 0.f && Timing::sInstance.GetFrameStartTime() > mTimeToRespawn)
	{
		static_cast< Server* > (Engine::sInstance.get())->SpawnCatForPlayer(mPlayerId);
		mTimeToRespawn = 0.f;
	}
}
