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
#include "NetworkConstants.h"

class ReplicationManagerTransmissionData;
class OutputMemoryBitStream;
class InputMemoryBitStream;

class ReplicationCommand
{
public:
    ReplicationCommand();
    ReplicationCommand(uint32_t inInitialDirtyState);
	
    void HandleCreateAckd();
    void AddDirtyState(uint32_t inState);
    void SetDestroy();
	
    bool HasDirtyState() const;
	
    ReplicationAction GetAction() const;
    uint32_t GetDirtyState() const;
    
	void ClearDirtyState(uint32_t inStateToClear);
	
private:
	uint32_t mDirtyState;
	ReplicationAction mAction;
};

#endif /* defined(__noctisgames__ReplicationCommand__) */
