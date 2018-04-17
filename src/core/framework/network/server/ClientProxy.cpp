//
//  ClientProxy.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/server/ClientProxy.h>

#include <framework/entity/EntityManager.h>
#include <framework/network/server/ReplicationManagerServer.h>
#include <framework/network/portable/MachineAddress.h>
#include <framework/util/Timing.h>

#include <framework/util/Constants.h>
#include <framework/util/InstanceManager.h>

ClientProxy::ClientProxy(EntityManager* entityManager, MachineAddress* inMachineAddress, const std::string& inName, uint8_t inPlayerId) :
_timing(static_cast<Timing*>(INSTANCE_MANAGER->getInstance(INSTANCE_TIME_SERVER))),
_deliveryNotificationManager(DeliveryNotificationManager(_timing, false, true)),
_replicationManagerServer(new ReplicationManagerServer(entityManager)),
_machineAddress(inMachineAddress->createNewCopy()),
_name(inName),
_lastPacketFromClientTime(0),
_isLastMoveTimestampDirty(false)
{
    _playerIds.push_back(inPlayerId);
    
    updateLastPacketTime();
}

ClientProxy::~ClientProxy()
{
    delete _machineAddress;
}

MachineAddress* ClientProxy::getMachineAddress() const
{
    return _machineAddress;
}

uint8_t ClientProxy::getPlayerId(uint8_t index) const
{
    return _playerIds.size() > index ? _playerIds[index] : INPUT_UNASSIGNED;
}

const std::string& ClientProxy::getName() const
{
    return _name;
}

void ClientProxy::updateLastPacketTime()
{
    _lastPacketFromClientTime = _timing->getTime();
}

float ClientProxy::getLastPacketFromClientTime() const
{
    return _lastPacketFromClientTime;
}

DeliveryNotificationManager& ClientProxy::getDeliveryNotificationManager()
{
    return _deliveryNotificationManager;
}

ReplicationManagerServer* ClientProxy::getReplicationManagerServer()
{
    return _replicationManagerServer;
}

MoveList& ClientProxy::getUnprocessedMoveList()
{
    return _unprocessedMoveList;
}

void ClientProxy::setLastMoveTimestampDirty(bool inIsDirty)
{
    _isLastMoveTimestampDirty = inIsDirty;
}

bool ClientProxy::isLastMoveTimestampDirty() const
{
    return _isLastMoveTimestampDirty;
}

void ClientProxy::onLocalPlayerAdded(uint8_t playerId)
{
    for (typename std::vector<uint8_t>::iterator i = _playerIds.begin(); i != _playerIds.end(); )
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
    
    _playerIds.push_back(playerId);
}

void ClientProxy::onLocalPlayerRemoved(uint8_t playerId)
{
    for (typename std::vector<uint8_t>::iterator i = _playerIds.begin(); i != _playerIds.end(); )
    {
        if ((*i) == playerId)
        {
            i = _playerIds.erase(i);
        }
        else
        {
            ++i;
        }
    }
}

std::vector<uint8_t>& ClientProxy::getPlayerIds()
{
    return _playerIds;
}

uint8_t ClientProxy::getNumPlayers()
{
    return static_cast<uint8_t>(_playerIds.size());
}
