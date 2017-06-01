//
//  RoboCat.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RoboCat__
#define __noctisgames__RoboCat__

#include "GameObject.h"

#include "NetworkConstants.h"
#include "InputState.h"

class RoboCat : public GameObject
{
    RTTI_DECL;
    
    NETWORK_TYPE_DECL(NETWORK_TYPE_RoboCat);
    
public:
    enum ECatReplicationState
    {
        ECRS_Pose = 1 << 0,
        ECRS_Color = 1 << 1,
        ECRS_PlayerId = 1 << 2,
        
        ECRS_AllState = ECRS_Pose | ECRS_Color | ECRS_PlayerId
    };
    
    static GameObject* create() { return new RoboCat(); }
    
    virtual uint32_t getAllStateMask() const override { return ECRS_AllState; }
    
    virtual void update() override;
    
    void ProcessInput(float inDeltaTime, const InputState& inInputState);
    void SimulateMovement(float inDeltaTime);
    
    void ProcessCollisions();
    void ProcessCollisionsWithScreenWalls();
    
    void SetPlayerId(uint32_t inPlayerId) { m_iPlayerId = inPlayerId; }
    uint32_t GetPlayerId() const { return m_iPlayerId; }
    
    void SetVelocity(Vector2 inVelocity) { mVelocity = inVelocity; }
    Vector2& GetVelocity() { return mVelocity; }
    
protected:
    Vector2 mVelocity;
    
    float m_fMaxLinearSpeed;
    float m_fMaxRotationSpeed;
    
    //bounce fraction when hitting various things
    float m_fWallRestitution;
    float m_fCatRestitution;
    
    uint32_t m_iPlayerId;
    
    RoboCat();
    
    void AdjustVelocityByThrust(float inDeltaTime);
    
protected:
    ///move down here for padding reasons...
    
    float m_fLastMoveTimestamp;
    
    float m_fThrustDir;
    int m_iHealth;
};

#endif /* defined(__noctisgames__RoboCat__) */
