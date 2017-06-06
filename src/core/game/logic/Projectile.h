//
//  Projectile.h
//  dante
//
//  Created by Stephen Gowen on 6/4/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Projectile__
#define __noctisgames__Projectile__

#include "Entity.h"

#include "GameConstants.h"

#include "RTTI.h"

#include <stdint.h>

class Robot;

class Projectile : public Entity
{
    RTTI_DECL;
    
    NETWORK_TYPE_DECL(NETWORK_TYPE_Projectile);
    
public:
    enum ProjectileReplicationState
    {
        PRJC_Pose = 1 << 0,
        PRJC_Color = 1 << 1,
        PRJC_PlayerId = 1 << 2,
        
        PRJC_AllState = PRJC_Pose | PRJC_Color | PRJC_PlayerId
    };
    
    enum ProjectileState
    {
        ProjectileState_Active = 1,
        ProjectileState_Exploding = 2
    };
    
    static Entity* create();
    
    virtual void onDeletion();
    
    virtual void update();
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    void initFromShooter(Robot* inRobot);
    
    ProjectileState getState();
    
    void setPlayerId(uint32_t inPlayerId);
    
    uint32_t getPlayerId() const;
    
    bool isFacingLeft();
    
private:
    uint32_t m_iPlayerId;
    ProjectileState m_state;
    bool m_isFacingLeft;
    
    float m_fTimePositionBecameOutOfSync;
    
    void updateInternal(float inDeltaTime);
    
    void processCollisions();
    
    void processCollisionsWithScreenWalls();
    
#ifdef NG_CLIENT
    void interpolateClientSidePrediction(Vector2& inOldPos);
    
    bool interpolateVectorsIfNecessary(Vector2& inA, Vector2& inB, float& syncTracker);
#endif
    
    void playSound(int soundId);
    
    Projectile();
};

#endif /* defined(__noctisgames__Projectile__) */
