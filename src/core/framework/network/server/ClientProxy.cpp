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
#include "FrameworkConstants.h"

ClientProxy::ClientProxy(IMachineAddress* inMachineAddress, const std::string& inName, uint8_t inPlayerId) :
m_deliveryNotificationManager(DeliveryNotificationManager(false, true)),
m_machineAddress(inMachineAddress->createNewCopy()),
m_name(inName),
m_fLastPacketFromClientTime(0),
m_isLastMoveTimestampDirty(false)
{
    m_playerIds.push_back(inPlayerId);
    
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

uint8_t ClientProxy::getPlayerId(int index) const
{
    return m_playerIds.size() > index ? m_playerIds[index] : INPUT_UNASSIGNED;
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

void ClientProxy::setIsLastMoveTimestampDirty(bool inIsDirty)
{
    m_isLastMoveTimestampDirty = inIsDirty;
}

bool ClientProxy::isLastMoveTimestampDirty() const
{
    return m_isLastMoveTimestampDirty;
}

void ClientProxy::onLocalPlayerAdded(uint8_t playerId)
{
    for (typename std::vector<uint8_t>::iterator i = m_playerIds.begin(); i != m_playerIds.end(); )
    {
        if ((*i) == playerId)
        {
            return;
        }
        else
        {
            ++i;
        }
    }
    
    m_playerIds.push_back(playerId);
}

void ClientProxy::onLocalPlayerRemoved(uint8_t playerId)
{
    for (typename std::vector<uint8_t>::iterator i = m_playerIds.begin(); i != m_playerIds.end(); )
    {
        if ((*i) == playerId)
        {
            i = m_playerIds.erase(i);
        }
        else
        {
            ++i;
        }
    }
}

std::vector<uint8_t>& ClientProxy::getPlayerIds()
{
    return m_playerIds;
}

int ClientProxy::getNumPlayers()
{
    return static_cast<int>(m_playerIds.size());
}
