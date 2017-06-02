//
//  RoboCat.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RoboCat__
#define __noctisgames__RoboCat__

#include "PhysicalEntity.h"

#include "NetworkType.h"
#include "GameConstants.h"

#include "RTTI.h"

#include <stdint.h>

class IInputState;

class RoboCat : public PhysicalEntity
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
    
protected:
    float m_fSpeed;
    float m_fJumpSpeed;
    
    //bounce fraction when hitting various things
    float m_fWallRestitution;
    float m_fCatRestitution;
    
    bool m_isFacingLeft;
    bool m_isGrounded;
    bool m_isFalling;
    
    uint32_t m_iPlayerId;
    uint32_t m_iReadState;
    
    RoboCat();
    
private:
    int m_iIndexInWorld;
};

#endif /* defined(__noctisgames__RoboCat__) */
