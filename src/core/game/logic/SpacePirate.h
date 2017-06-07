//
//  SpacePirate.h
//  dante
//
//  Created by Stephen Gowen on 6/4/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SpacePirate__
#define __noctisgames__SpacePirate__

#include "Entity.h"

#include "GameConstants.h"

#include "RTTI.h"

#include <stdint.h>

class Robot;

class SpacePirate : public Entity
{
    RTTI_DECL;
    
    NETWORK_TYPE_DECL(NETWORK_TYPE_SpacePirate);
    
public:
    enum SpacePirateReplicationState
    {
        SPCP_Pose = 1 << 0,
        SPCP_Color = 1 << 1,
        SPCP_Health = 1 << 2,
        
        SPCP_AllState = SPCP_Pose | SPCP_Color | SPCP_Health
    };
    
    static Entity* create();
    
    virtual void onDeletion();
    
    virtual void update();
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    void init(float x, float y, float speed);
    
    void takeDamage();
    
    bool isFacingLeft();
    
private:
    float m_fSpeed;
    int m_iHealth;
    bool m_isFacingLeft;
    bool m_isGrounded;
    bool m_isFalling;
    
    //bounce fraction when hitting various things
    float m_fWallRestitution;
    float m_fRobotRestitution;
    
    float m_fTimeAccelerationBecameOutOfSync;
    float m_fTimeVelocityBecameOutOfSync;
    float m_fTimePositionBecameOutOfSync;
    
    void updateInternal(float inDeltaTime);
    
    void processCollisions();
    
    void processCollisionsWithScreenWalls();
    
#ifdef NG_CLIENT
    void interpolateClientSidePrediction(float& inOldStateTime, Vector2& inOldPos);
    
    bool interpolateVectorsIfNecessary(Vector2& inA, Vector2& inB, float& syncTracker);
#endif
    
    SpacePirate();
};

#endif /* defined(__noctisgames__SpacePirate__) */