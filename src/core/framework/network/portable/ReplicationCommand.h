//
//  ReplicationCommand.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ReplicationCommand__
#define __noctisgames__ReplicationCommand__

#include "framework/network/portable/ReplicationAction.h"
#include "framework/network/portable/Network.h"

class ReplicationManagerTransmissionData;
class OutputMemoryBitStream;
class InputMemoryBitStream;

class ReplicationCommand
{
public:
    ReplicationCommand();
    
    ReplicationCommand(uint16_t inInitialDirtyState);
    
    void handleCreateAckd();
    
    void addDirtyState(uint16_t inState);
    
    void setDestroy();
    
    bool hasDirtyState() const;
    
    ReplicationAction getAction() const;
    
    uint16_t getDirtyState() const;
    
    void clearDirtyState(uint16_t inStateToClear);
    
private:
    uint16_t _dirtyState;
    ReplicationAction _action;
};

#endif /* defined(__noctisgames__ReplicationCommand__) */
