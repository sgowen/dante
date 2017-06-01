//
//  RoboCat.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RoboCat__
#define __noctisgames__RoboCat__

#include "NWPhysicalEntity.h"

#include <stdint.h>

class InputState;

class RoboCat : public NWPhysicalEntity
{
    RTTI_DECL;
    
public:
    enum ECatReplicationState
    {
        ECRS_Pose = 1 << 0,
        ECRS_Color = 1 << 1,
        ECRS_PlayerId = 1 << 2,
        
        ECRS_AllState = ECRS_Pose | ECRS_Color | ECRS_PlayerId
    };
    
    static NWPhysicalEntity* create();
    
    virtual uint32_t getAllStateMask() const override;
    
    void ProcessInput(float inDeltaTime, const InputState& inInputState);
    void SimulateMovement(float inDeltaTime);
    
    void ProcessCollisions();
    void ProcessCollisionsWithScreenWalls();
    
    void setPlayerId(uint32_t inPlayerId);
    uint32_t getPlayerId() const;
    
    void setIndexInWorld(int inIndex);
    int getIndexInWorld() const;
    
protected:
    float m_fMaxLinearSpeed;
    float m_fMaxRotationSpeed;
    
    //bounce fraction when hitting various things
    float m_fWallRestitution;
    float m_fCatRestitution;
    
    float m_fLastMoveTimestamp;
    float m_fThrustDir;
    
    uint32_t m_iPlayerId;
    
    void AdjustVelocityByThrust(float inDeltaTime);
    
    RoboCat();
    
private:
    int m_iIndexInWorld;
};

#endif /* defined(__noctisgames__RoboCat__) */
