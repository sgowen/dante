//
//  ClientProxy.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ClientProxy__
#define __noctisgames__ClientProxy__

#include "ReplicationManagerServer.h"
#include "DeliveryNotificationManager.h"
#include "MoveList.h"

#include <string>
#include <vector>

class IMachineAddress;

class ClientProxy
{
public:
    ClientProxy(IMachineAddress* inMachineAddress, const std::string& inName, uint8_t inPlayerId);
    
    ~ClientProxy();
    
    IMachineAddress* getMachineAddress() const;
    
    uint8_t getPlayerId(int index = 0) const;
    
    const std::string& getName() const;
    
    void updateLastPacketTime();
    
    float getLastPacketFromClientTime()	const;
    
    DeliveryNotificationManager& getDeliveryNotificationManager();
    
    ReplicationManagerServer& getReplicationManagerServer();
    
    MoveList& getUnprocessedMoveList();
    
    void setIsLastMoveTimestampDirty(bool inIsDirty);
    
    bool isLastMoveTimestampDirty() const;
    
    void onLocalPlayerAdded(uint8_t playerId);
    
    void onLocalPlayerRemoved(uint8_t playerId);
    
    std::vector<uint8_t>& getPlayerIds();
    
    int getNumPlayers();
    
private:
    DeliveryNotificationManager	m_deliveryNotificationManager;
    ReplicationManagerServer m_replicationManagerServer;
    IMachineAddress* m_machineAddress;
    std::string m_name;
    std::vector<uint8_t> m_playerIds;
    float m_fLastPacketFromClientTime;
    MoveList m_unprocessedMoveList;
    bool m_isLastMoveTimestampDirty;
};

#endif /* defined(__noctisgames__ClientProxy__) */
