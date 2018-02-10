//
//  ReplicationManagerClient.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ReplicationManagerClient__
#define __noctisgames__ReplicationManagerClient__

#include <cstdint>

class EntityManager;
class InputMemoryBitStream;

class ReplicationManagerClient
{
public:
    ReplicationManagerClient(EntityManager* entityManager);
    
    void read(InputMemoryBitStream& inInputStream);
    
private:
    EntityManager* _entityManager;
    
    void readAndDoCreateAction(InputMemoryBitStream& inInputStream, uint32_t inNetworkId);
    void readAndDoUpdateAction(InputMemoryBitStream& inInputStream, uint32_t inNetworkId);
    void readAndDoDestroyAction(InputMemoryBitStream& inInputStream, uint32_t inNetworkId);
};

#endif /* defined(__noctisgames__ReplicationManagerClient__) */
