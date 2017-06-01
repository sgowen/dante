//
//  RoboCatServer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RoboCatServer__
#define __noctisgames__RoboCatServer__

#include "RoboCat.h"

#include "NetworkType.h"
#include "GameConstants.h"

class RoboCatServer : public RoboCat
{
    RTTI_DECL;
    
    NETWORK_TYPE_DECL(NETWORK_TYPE_RoboCat);
    
public:
    static NWPhysicalEntity* create();
    
    virtual void onDeletion();
    
    virtual void update();
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) override;
    
protected:
    RoboCatServer();
};

#endif /* defined(__noctisgames__RoboCatServer__) */
