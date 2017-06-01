//
//  NWPhysicalEntity.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NWPhysicalEntity__
#define __noctisgames__NWPhysicalEntity__

#include "PhysicalEntity.h"

#include "NetworkType.h"
#include "FrameworkConstants.h"

#include "RTTI.h"

#include <stdint.h>

class OutputMemoryBitStream;
class InputMemoryBitStream;

class NWPhysicalEntity : public PhysicalEntity
{
    RTTI_DECL;
    
    NETWORK_TYPE_DECL(NETWORK_TYPE_NWPhysicalEntity);
    
public:
    NWPhysicalEntity();
    
    virtual ~NWPhysicalEntity();
    
    virtual uint32_t getAllStateMask() const;
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    virtual void read(InputMemoryBitStream& inInputStream);
};

#endif /* defined(__noctisgames__NWPhysicalEntity__) */
