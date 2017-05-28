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

#include "InputState.h"

class OutputMemoryBitStream;

class RoboCat : public GameObject
{
public:
    SUB_CLASS_IDENTIFICATION('RCAT', GameObject)
    
    enum ECatReplicationState
    {
        ECRS_Pose = 1 << 0,
        ECRS_Color = 1 << 1,
        ECRS_PlayerId = 1 << 2,
        
        ECRS_AllState = ECRS_Pose | ECRS_Color | ECRS_PlayerId
    };
    
    static	GameObject*	StaticCreate() { return new RoboCat(); }
    
    virtual uint32_t GetAllStateMask() const override { return ECRS_AllState; }
    
    virtual	RoboCat* GetAsCat()	override { return this; }
    
    virtual void Update() override;
    
    void ProcessInput(float inDeltaTime, const InputState& inInputState);
    void SimulateMovement(float inDeltaTime);
    
    void ProcessCollisions();
    void ProcessCollisionsWithScreenWalls();
    
    void SetPlayerId(uint32_t inPlayerId) { m_iPlayerId = inPlayerId; }
    uint32_t GetPlayerId() const { return m_iPlayerId; }
    
    void SetVelocity(Vector2 inVelocity) { mVelocity = inVelocity; }
    Vector2& GetVelocity() { return mVelocity; }
    
    virtual uint32_t Write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) override;
    
protected:
    RoboCat();
    
private:
    void AdjustVelocityByThrust(float inDeltaTime);
    
    Vector2 mVelocity;
    
    float m_fMaxLinearSpeed;
    float m_fMaxRotationSpeed;
    
    //bounce fraction when hitting various things
    float m_fWallRestitution;
    float m_fCatRestitution;
    
    uint32_t m_iPlayerId;
    
protected:
    ///move down here for padding reasons...
    
    float m_fLastMoveTimestamp;
    
    float m_fThrustDir;
    int m_iHealth;
};

typedef std::shared_ptr<RoboCat> RoboCatPtr;

#endif /* defined(__noctisgames__RoboCat__) */
