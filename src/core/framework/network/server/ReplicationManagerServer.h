//
//  ReplicationManagerServer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ReplicationManagerServer__
#define __noctisgames__ReplicationManagerServer__

#include "Network.h"

#include "ReplicationCommand.h"

#include <map>

class OutputMemoryBitStream;
class ReplicationManagerTransmissionData;

class ReplicationManagerServer
{
public:
    void replicateCreate(uint32_t inNetworkId, uint32_t inInitialDirtyState);
    
    void replicateDestroy(uint32_t inNetworkId);
    
    void setStateDirty(uint32_t inNetworkId, uint32_t inDirtyState);
    
    void handleCreateAckd(uint32_t inNetworkId);
    
    void removeFromReplication(uint32_t inNetworkId);
    
    void write(OutputMemoryBitStream& inOutputStream, ReplicationManagerTransmissionData* ioTransmissinData);
    
private:
    std::map<int, ReplicationCommand> m_networkIdToReplicationCommand;
    
    uint32_t writeCreateAction(OutputMemoryBitStream& inOutputStream, uint32_t inNetworkId, uint32_t inDirtyState);
    
    uint32_t writeUpdateAction(OutputMemoryBitStream& inOutputStream, uint32_t inNetworkId, uint32_t inDirtyState);
    
    uint32_t writeDestroyAction(OutputMemoryBitStream& inOutputStream, uint32_t inNetworkId, uint32_t inDirtyState);
};

#endif /* defined(__noctisgames__ReplicationManagerServer__) */
