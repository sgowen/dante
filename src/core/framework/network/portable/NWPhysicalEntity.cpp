//
//  NWPhysicalEntity.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NWPhysicalEntity.h"

#include "MemoryBitStream.h"

#include "macros.h"

NWPhysicalEntity::NWPhysicalEntity() : PhysicalEntity(0, 0, 1, 1)
{
    // Empty
}

NWPhysicalEntity::~NWPhysicalEntity()
{
    // Empty
}

uint32_t NWPhysicalEntity::getAllStateMask() const
{
    return 0;
}

uint32_t NWPhysicalEntity::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    UNUSED(inOutputStream);
    UNUSED(inDirtyState);
    
    return 0;
}

void NWPhysicalEntity::read(InputMemoryBitStream& inInputStream)
{
    UNUSED(inInputStream);
}

RTTI_IMPL(NWPhysicalEntity, PhysicalEntity);

NETWORK_TYPE_IMPL(NWPhysicalEntity);
