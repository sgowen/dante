//
//  MemoryBitStreamUtil.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 4/17/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#include <framework/network/portable/MemoryBitStreamUtil.h>

#include <framework/network/portable/InputMemoryBitStream.h>
#include <framework/network/portable/OutputMemoryBitStream.h>
#include <framework/entity/Entity.h>
#include <Box2D/Common/b2Math.h>

#include <framework/entity/EntityController.h>

void MemoryBitStreamUtil::read(InputMemoryBitStream& ip, Entity& e)
{
    e._readState = 0;
    
    bool stateBit;
    
    ip.read(stateBit);
    if (stateBit)
    {
        read(ip, e._pose.velocity);
        read(ip, e._pose.position);
        
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
        
        e._readState |= Entity::ReadStateFlag_Pose;
    }
    
    if (e._entityDef.stateSensitive)
    {
        ip.read(stateBit);
        if (stateBit)
        {
            ip.read(e._state.stateTime);
            ip.read(e._state.state);
            ip.read(e._state.stateFlags);
            
            e._readState |= Entity::ReadStateFlag_State;
        }
    }
    
    e._controller->read(ip, e._readState);
    
    if (e._readState & Entity::ReadStateFlag_Pose)
    {
        e._poseCache = e._pose;
    }
    
    if (e._readState & Entity::ReadStateFlag_State)
    {
        e._stateCache = e._state;
    }
}

uint16_t MemoryBitStreamUtil::write(OutputMemoryBitStream& op, Entity& e, uint16_t dirtyState)
{
    uint16_t writtenState = 0;
    
    bool pose = dirtyState & Entity::ReadStateFlag_Pose;
    op.write(pose);
    if (pose)
    {
        write(op, e._pose.velocity);
        write(op, e._pose.position);
        
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
    
    return e._controller->write(op, writtenState, dirtyState);
}

void MemoryBitStreamUtil::read(InputMemoryBitStream& ip, b2Vec2& v)
{
#ifdef NG_LOG
    // Disable optimizations
    bool isZero;
    ip.read(isZero);
    ip.read(v.x);
    ip.read(isZero);
    ip.read(v.y);
#else
    bool isZero;
    ip.read(isZero);
    if (isZero)
    {
        v.x = 0;
    }
    else
    {
        ip.read(v.x);
    }
    
    ip.read(isZero);
    if (isZero)
    {
        v.y = 0;
    }
    else
    {
        ip.read(v.y);
    }
#endif
}

void MemoryBitStreamUtil::write(OutputMemoryBitStream& op, b2Vec2& v)
{
#ifdef NG_LOG
    // Disable optimizations
    op.write(false);
    op.write(v.x);
    op.write(false);
    op.write(v.y);
#else
    bool isZero = v.x == 0;
    op.write(isZero);
    if (!isZero)
    {
        op.write(v.x);
    }
    
    isZero = v.y == 0;
    op.write(isZero);
    if (!isZero)
    {
        op.write(v.y);
    }
#endif
}
