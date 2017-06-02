//
//  Robot.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Robot__
#define __noctisgames__Robot__

#include "PhysicalEntity.h"

#include "GameConstants.h"

#include "RTTI.h"

#include <stdint.h>

class IInputState;

class Robot : public PhysicalEntity
{
    RTTI_DECL;
    
    NETWORK_TYPE_DECL(NETWORK_TYPE_Robot);
    
public:
    enum RobotReplicationState
    {
        RBRS_Pose = 1 << 0,
        RBRS_Color = 1 << 1,
        RBRS_PlayerId = 1 << 2,
        RBRS_Health = 1 << 3,
        
        RBRS_AllState = RBRS_Pose | RBRS_Color | RBRS_PlayerId | RBRS_Health
    };
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    void processInput(float inDeltaTime, IInputState* inInputState);
    
    void simulateMovement(float inDeltaTime);
    
    void processCollisions();
    
    void processCollisionsWithScreenWalls();
    
    void setPlayerId(uint32_t inPlayerId);
    
    uint32_t getPlayerId() const;
    
    void setIndexInWorld(int inIndex);
    
    int getIndexInWorld() const;
    
    bool isFacingLeft();
    
    bool isGrounded();
    
    bool isShooting();
    
protected:
    float m_fSpeed;
    float m_fJumpSpeed;
    
    //bounce fraction when hitting various things
    float m_fWallRestitution;
    float m_fCatRestitution;
    
    int m_iHealth;
    
    bool m_isFacingLeft;
    bool m_isGrounded;
    bool m_isFalling;
    bool m_isShooting;
    
    uint32_t m_iPlayerId;
    uint32_t m_iReadState;
    
    Robot();
    
private:
    int m_iIndexInWorld;
};

#endif /* defined(__noctisgames__Robot__) */
