//
//  ClientProxy.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Server.h"
#include "Timing.h"

namespace
{
    const float kRespawnDelay = 3.f;
}

ClientProxy::ClientProxy(const SocketAddress& inSocketAddress, const std::string& inName, int inPlayerId) :
mSocketAddress(inSocketAddress),
mName(inName),
m_iPlayerId(inPlayerId),
mDeliveryNotificationManager(false, true),
mIsLastMoveTimestampDirty(false),
m_fTimeToRespawn(0.f)
{
    UpdateLastPacketTime();
}

void ClientProxy::UpdateLastPacketTime()
{
    m_fLastPacketFromClientTime = Timing::getInstance()->GetTime();
}

void ClientProxy::HandleCatDied()
{
    m_fTimeToRespawn = Timing::getInstance()->GetFrameStartTime() + kRespawnDelay;
}

void ClientProxy::RespawnCatIfNecessary()
{
    if (m_fTimeToRespawn != 0.f && Timing::getInstance()->GetFrameStartTime() > m_fTimeToRespawn)
    {
        static_cast<Server*>(Engine::sInstance.get())->SpawnCatForPlayer(m_iPlayerId);
        m_fTimeToRespawn = 0.f;
    }
}
