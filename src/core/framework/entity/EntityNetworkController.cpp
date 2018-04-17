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

EntityNetworkController::EntityNetworkController(Entity* inEntity) : _entity(inEntity)
{
    // Empty
}

EntityNetworkController::~EntityNetworkController()
{
    // Empty
}

void EntityNetworkController::read(InputMemoryBitStream& ip)
{
    _entity->_readState = 0;
    
    bool stateBit;
    
    ip.read(stateBit);
    if (stateBit)
    {
        MemoryBitStreamUtil::read(ip, _entity->_pose.velocity);
        MemoryBitStreamUtil::read(ip, _entity->_pose.position);
        
        if (!_entity->isFixedRotation())
        {
            ip.read(_entity->_pose.angle);
        }
        
        if (_entity->_groundSensorFixture)
        {
            ip.read<uint8_t, 4>(_entity->_pose.numGroundContacts);
        }
        
        ip.read(_entity->_pose.isFacingLeft);
        
        _entity->updateBodyFromPose();
        
        _entity->_readState |= Entity::ReadStateFlag_Pose;
        _entity->_poseNetworkCache = _entity->_pose;
    }
    
    if (_entity->_entityDef.stateSensitive)
    {
        ip.read(stateBit);
        if (stateBit)
        {
            ip.read(_entity->_state.stateTime);
            ip.read(_entity->_state.state);
            ip.read(_entity->_state.stateFlags);
            
            _entity->_readState |= Entity::ReadStateFlag_State;
            _entity->_stateNetworkCache = _entity->_state;
        }
    }
}

void EntityNetworkController::recallCache()
{
    _entity->_pose = _entity->_poseNetworkCache;
    _entity->_state = _entity->_stateNetworkCache;
    
    _entity->updateBodyFromPose();
}

uint16_t EntityNetworkController::write(OutputMemoryBitStream& op, uint16_t dirtyState)
{
    uint16_t writtenState = 0;
    
    bool pose = dirtyState & Entity::ReadStateFlag_Pose;
    op.write(pose);
    if (pose)
    {
        MemoryBitStreamUtil::write(op, _entity->_pose.velocity);
        MemoryBitStreamUtil::write(op, _entity->_pose.position);
        
        if (!_entity->isFixedRotation())
        {
            op.write(_entity->_pose.angle);
        }
        
        if (_entity->_groundSensorFixture)
        {
            op.write<uint8_t, 4>(_entity->_pose.numGroundContacts);
        }
        
        op.write(_entity->_pose.isFacingLeft);
        
        writtenState |= Entity::ReadStateFlag_Pose;
    }
    
    if (_entity->_entityDef.stateSensitive)
    {
        bool state = dirtyState & Entity::ReadStateFlag_State;
        op.write(state);
        if (state)
        {
            op.write(_entity->_state.stateTime);
            op.write(_entity->_state.state);
            op.write(_entity->_state.stateFlags);
            
            writtenState |= Entity::ReadStateFlag_State;
        }
    }
    
    return writtenState;
}
