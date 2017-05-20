//
//  ReplicationManagerServer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ReplicationManagerServer__
#define __noctisgames__ReplicationManagerServer__

#include "NetworkConstants.h"

#include "ReplicationCommand.h"

#include <unordered_map>

class OutputMemoryBitStream;
class ReplicationManagerTransmissionData;

class ReplicationManagerServer
{
public:
	void ReplicateCreate(int inNetworkId, uint32_t inInitialDirtyState);
	void ReplicateDestroy(int inNetworkId);
	void SetStateDirty(int inNetworkId, uint32_t inDirtyState);
	void HandleCreateAckd(int inNetworkId);
	void RemoveFromReplication(int inNetworkId);

	void Write(OutputMemoryBitStream& inOutputStream, ReplicationManagerTransmissionData* ioTransmissinData);

private:

	uint32_t WriteCreateAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint32_t inDirtyState);
	uint32_t WriteUpdateAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint32_t inDirtyState);
	uint32_t WriteDestroyAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint32_t inDirtyState);

	std::unordered_map< int, ReplicationCommand >	mNetworkIdToReplicationCommand;
};

#endif /* defined(__noctisgames__ReplicationManagerServer__) */
