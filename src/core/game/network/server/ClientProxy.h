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
#include "SocketAddress.h"
#include "DeliveryNotificationManager.h"
#include "MoveList.h"

class ClientProxy
{
public:
    ClientProxy(const SocketAddress& inSocketAddress, const std::string& inName, int inPlayerId);
    
    const SocketAddress& getSocketAddress()	const;
    
    int getPlayerId() const;
    
    const std::string& getName() const;
    
    void updateLastPacketTime();
    
    float getLastPacketFromClientTime()	const;
    
    DeliveryNotificationManager& getDeliveryNotificationManager();
    
    ReplicationManagerServer& getReplicationManagerServer();
    
    const MoveList& getUnprocessedMoveList() const;
    
    MoveList& getUnprocessedMoveList();
    
    void SetIsLastMoveTimestampDirty(bool inIsDirty);
    
    bool IsLastMoveTimestampDirty() const;
    
private:
    DeliveryNotificationManager	m_deliveryNotificationManager;
    ReplicationManagerServer m_replicationManagerServer;
    
    SocketAddress m_socketAddress;
    std::string m_name;
    int m_iPlayerId;
    
    float m_fLastPacketFromClientTime;
    
    MoveList m_unprocessedMoveList;
    bool m_isLastMoveTimestampDirty;
};

#endif /* defined(__noctisgames__ClientProxy__) */
