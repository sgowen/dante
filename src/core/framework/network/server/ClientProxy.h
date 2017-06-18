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

class IMachineAddress;

class ClientProxy
{
public:
    ClientProxy(IMachineAddress* inMachineAddress, const std::string& inName, int inPlayerId);
    
    ~ClientProxy();
    
    IMachineAddress* getMachineAddress() const;
    
    int getPlayerId() const;
    
    const std::string& getName() const;
    
    void updateLastPacketTime();
    
    float getLastPacketFromClientTime()	const;
    
    DeliveryNotificationManager& getDeliveryNotificationManager();
    
    ReplicationManagerServer& getReplicationManagerServer();
    
    MoveList& getUnprocessedMoveList();
    
    void setIsLastMoveTimestampDirty(bool inIsDirty);
    
    bool isLastMoveTimestampDirty() const;
    
    void setAuthorized(bool isAuthorized);
    
    bool isAuthorized() const;
    
private:
    DeliveryNotificationManager	m_deliveryNotificationManager;
    ReplicationManagerServer m_replicationManagerServer;
    IMachineAddress* m_machineAddress;
    std::string m_name;
    int m_iPlayerId;
    float m_fLastPacketFromClientTime;
    MoveList m_unprocessedMoveList;
    bool m_isLastMoveTimestampDirty;
    bool m_isAuthorized;
};

#endif /* defined(__noctisgames__ClientProxy__) */
