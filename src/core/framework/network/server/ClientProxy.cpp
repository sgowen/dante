//
//  ClientProxy.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "ClientProxy.h"

#include "Timing.h"

ClientProxy::ClientProxy(const SocketAddress& inSocketAddress, const std::string& inName, int inPlayerId) :
m_socketAddress(inSocketAddress),
m_name(inName),
m_iPlayerId(inPlayerId),
m_deliveryNotificationManager(false, true),
m_isLastMoveTimestampDirty(false)
{
    updateLastPacketTime();
}

const SocketAddress& ClientProxy::getSocketAddress() const
{
    return m_socketAddress;
}

int ClientProxy::getPlayerId() const
{
    return m_iPlayerId;
}

const std::string& ClientProxy::getName() const
{
    return m_name;
}

void ClientProxy::updateLastPacketTime()
{
    m_fLastPacketFromClientTime = Timing::getInstance()->getFrameStartTime();
}

float ClientProxy::getLastPacketFromClientTime() const
{
    return m_fLastPacketFromClientTime;
}

DeliveryNotificationManager& ClientProxy::getDeliveryNotificationManager()
{
    return m_deliveryNotificationManager;
}

ReplicationManagerServer& ClientProxy::getReplicationManagerServer()
{
    return m_replicationManagerServer;
}

MoveList& ClientProxy::getUnprocessedMoveList()
{
    return m_unprocessedMoveList;
}

void ClientProxy::SetIsLastMoveTimestampDirty(bool inIsDirty)
{
    m_isLastMoveTimestampDirty = inIsDirty;
}

bool ClientProxy::IsLastMoveTimestampDirty() const
{
    return m_isLastMoveTimestampDirty;
}
