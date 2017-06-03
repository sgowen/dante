//
//  ReplicationCommand.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ReplicationCommand__
#define __noctisgames__ReplicationCommand__

#include "ReplicationAction.h"
#include "Network.h"

class ReplicationManagerTransmissionData;
class OutputMemoryBitStream;
class InputMemoryBitStream;

class ReplicationCommand
{
public:
    ReplicationCommand();
    
    ReplicationCommand(uint32_t inInitialDirtyState);
    
    void handleCreateAckd();
    
    void addDirtyState(uint32_t inState);
    
    void setDestroy();
    
    bool hasDirtyState() const;
    
    ReplicationAction getAction() const;
    
    uint32_t getDirtyState() const;
    
    void clearDirtyState(uint32_t inStateToClear);
    
private:
    uint32_t m_iDirtyState;
    ReplicationAction m_action;
};

#endif /* defined(__noctisgames__ReplicationCommand__) */
