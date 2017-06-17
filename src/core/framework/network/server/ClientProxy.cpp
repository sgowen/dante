//
//  ClientProxy.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "ClientProxy.h"

#include "IMachineAddress.h"

#include "Timing.h"

ClientProxy::ClientProxy(IMachineAddress* inMachineAddress, const std::string& inName, int inPlayerId) :
m_machineAddress(inMachineAddress),
m_name(inName),
m_iPlayerId(inPlayerId),
m_deliveryNotificationManager(DeliveryNotificationManager(false, true)),
m_isLastMoveTimestampDirty(false)
{
    updateLastPacketTime();
}

ClientProxy::~ClientProxy()
{
    delete m_machineAddress;
}

IMachineAddress* ClientProxy::getMachineAddress() const
{
    return m_machineAddress;
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

std::string ClientProxy::toString() const
{
    return m_machineAddress->toString();
}
