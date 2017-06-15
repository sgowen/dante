//
//  ReplicationManagerClient.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ReplicationManagerClient__
#define __noctisgames__ReplicationManagerClient__

class EntityRegistry;
class InputMemoryBitStream;

class ReplicationManagerClient
{
public:
    ReplicationManagerClient(EntityRegistry* entityRegistry);
    
    void read(InputMemoryBitStream& inInputStream);
    
private:
    EntityRegistry* m_entityRegistry;
    
    void readAndDoCreateAction(InputMemoryBitStream& inInputStream, int inNetworkId);
    
    void readAndDoUpdateAction(InputMemoryBitStream& inInputStream, int inNetworkId);
    
    void readAndDoDestroyAction(InputMemoryBitStream& inInputStream, int inNetworkId);
};

#endif /* defined(__noctisgames__ReplicationManagerClient__) */
