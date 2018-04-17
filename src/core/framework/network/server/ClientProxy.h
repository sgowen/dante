//
//  ClientProxy.h
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ClientProxy__
#define __noctisgames__ClientProxy__

#include <framework/network/portable/DeliveryNotificationManager.h>
#include <framework/network/portable/MoveList.h>

#include <string>
#include <vector>

class EntityManager;
class ReplicationManagerServer;
class MachineAddress;
class Timing;

class ClientProxy
{
public:
    ClientProxy(EntityManager* entityManager, MachineAddress* inMachineAddress, const std::string& inName, uint8_t inPlayerId);
    ~ClientProxy();
    
    MachineAddress* getMachineAddress() const;
    uint8_t getPlayerId(uint8_t index = 0) const;
    const std::string& getName() const;
    void updateLastPacketTime();
    float getLastPacketFromClientTime()	const;
    DeliveryNotificationManager& getDeliveryNotificationManager();
    ReplicationManagerServer* getReplicationManagerServer();
    MoveList& getUnprocessedMoveList();
    void setLastMoveTimestampDirty(bool inIsDirty);
    bool isLastMoveTimestampDirty() const;
    void onLocalPlayerAdded(uint8_t playerId);
    void onLocalPlayerRemoved(uint8_t playerId);
    std::vector<uint8_t>& getPlayerIds();
    uint8_t getNumPlayers();
    
private:
    Timing* _timing;
    DeliveryNotificationManager	_deliveryNotificationManager;
    ReplicationManagerServer* _replicationManagerServer;
    MachineAddress* _machineAddress;
    std::string _name;
    std::vector<uint8_t> _playerIds;
    float _lastPacketFromClientTime;
    MoveList _unprocessedMoveList;
    bool _isLastMoveTimestampDirty;
};

#endif /* defined(__noctisgames__ClientProxy__) */
