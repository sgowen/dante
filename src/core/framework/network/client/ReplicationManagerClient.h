//
//  ReplicationManagerClient.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ReplicationManagerClient__
#define __noctisgames__ReplicationManagerClient__

class InputMemoryBitStream;

class ReplicationManagerClient
{
public:
    ReplicationManagerClient();
    
    void read(InputMemoryBitStream& inInputStream);
    
private:
    void readAndDoCreateAction(InputMemoryBitStream& inInputStream, int inNetworkId);
    
    void readAndDoUpdateAction(InputMemoryBitStream& inInputStream, int inNetworkId);
    
    void readAndDoDestroyAction(InputMemoryBitStream& inInputStream, int inNetworkId);
};

#endif /* defined(__noctisgames__ReplicationManagerClient__) */
