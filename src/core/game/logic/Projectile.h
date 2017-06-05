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
        
        PRJC_AllState = PRJC_Pose | PRJC_Color
    };
    
    static Entity* create();
    
    virtual void onDeletion();
    
    virtual void update();
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    void initFromShooter(Robot* inRobot);
    
    bool isFacingLeft();
    
private:
    Robot* m_owner;
    float m_fTimeVelocityBecameOutOfSync;
    float m_fTimePositionBecameOutOfSync;
    
    bool m_isFacingLeft;
    
    void updateInternal(float inDeltaTime);
    
    void processCollisions();
    
    void processCollisionsWithScreenWalls();
    
#ifdef NG_CLIENT
    void interpolateClientSidePrediction(float& inOldStateTime, Vector2& inOldVelocity, Vector2& inOldLocation);
#endif
    
    Projectile();
};

#endif /* defined(__noctisgames__Projectile__) */
