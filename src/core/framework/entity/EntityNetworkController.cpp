//
//  EntityNetworkController.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 4/17/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/entity/EntityNetworkController.h>

#include <framework/entity/Entity.h>
#include <framework/network/portable/InputMemoryBitStream.h>
#include <framework/network/portable/OutputMemoryBitStream.h>

#include <framework/network/portable/MemoryBitStreamUtil.h>

IMPL_RTTI_NOPARENT(EntityNetworkController);

IMPL_EntityNetworkController_create(EntityNetworkController);

EntityNetworkController::EntityNetworkController(Entity* e, bool isServer) : _entity(e), _isServer(isServer)
{
    // Empty
}

EntityNetworkController::~EntityNetworkController()
{
    // Empty
}

void EntityNetworkController::read(InputMemoryBitStream& ip)
{
    Entity& e = *_entity;
    
    bool stateBit;
    
    ip.read(stateBit);
    if (stateBit)
    {
        MemoryBitStreamUtil::read(ip, e._pose.velocity);
        MemoryBitStreamUtil::read(ip, e._pose.position);
        
        if (!e.isFixedRotation())
        {
            ip.read(e._pose.angle);
        }
        
        if (e._groundSensorFixture)
        {
            ip.read<uint8_t, 4>(e._pose.numGroundContacts);
        }
        
        ip.read(e._pose.isFacingLeft);
        
        e.updateBodyFromPose();
        
        e._poseNetworkCache = e._pose;
    }
    
    if (e._entityDef.stateSensitive)
    {
        ip.read(stateBit);
        if (stateBit)
        {
            ip.read(e._state.stateTime);
            ip.read(e._state.state);
            ip.read(e._state.stateFlags);
            
            e._stateNetworkCache = e._state;
        }
    }
}

uint16_t EntityNetworkController::write(OutputMemoryBitStream& op, uint16_t dirtyState)
{
    Entity& e = *_entity;
    
    uint16_t writtenState = 0;
    
    bool pose = dirtyState & Entity::ReadStateFlag_Pose;
    op.write(pose);
    if (pose)
    {
        MemoryBitStreamUtil::write(op, e._pose.velocity);
        MemoryBitStreamUtil::write(op, e._pose.position);
        
        if (!e.isFixedRotation())
        {
            op.write(e._pose.angle);
        }
        
        if (e._groundSensorFixture)
        {
            op.write<uint8_t, 4>(e._pose.numGroundContacts);
        }
        
        op.write(e._pose.isFacingLeft);
        
        writtenState |= Entity::ReadStateFlag_Pose;
    }
    
    if (e._entityDef.stateSensitive)
    {
        bool state = dirtyState & Entity::ReadStateFlag_State;
        op.write(state);
        if (state)
        {
            op.write(e._state.stateTime);
            op.write(e._state.state);
            op.write(e._state.stateFlags);
            
            writtenState |= Entity::ReadStateFlag_State;
        }
    }
    
    return writtenState;
}

void EntityNetworkController::recallNetworkCache()
{
    Entity& e = *_entity;
    
    e._pose = e._poseNetworkCache;
    e._state = e._stateNetworkCache;
    
    e.updateBodyFromPose();
}

uint16_t EntityNetworkController::getDirtyState()
{
    uint16_t ret = 0;
    
    Entity& e = *_entity;
    
    if (e._poseNetworkCache != e._pose)
    {
        e._poseNetworkCache = e._pose;
        ret |= Entity::ReadStateFlag_Pose;
    }
    
    if (e._stateNetworkCache != e._state)
    {
        e._stateNetworkCache = e._state;
        ret |= Entity::ReadStateFlag_State;
    }
    
    return ret;
}

bool EntityNetworkController::isServer()
{
    return _isServer;
}
