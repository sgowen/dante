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
    
    Projectile(b2World& world, bool isServer);
    
    virtual void update();
    
    virtual void handleContact(Entity* inEntity);
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    void initFromShooter(Robot* inRobot);
    
    ProjectileState getState();
    
    void setPlayerId(uint32_t inPlayerId);
    
    uint32_t getPlayerId() const;
    
    bool isFacingLeft();
    
private:
    bool m_isServer;
    uint32_t m_iPlayerId;
    ProjectileState m_state;
    bool m_isFacingLeft;
    
    void updateInternal(float inDeltaTime);
    
    void playSound(int soundId);
};

#endif /* defined(__noctisgames__Projectile__) */
