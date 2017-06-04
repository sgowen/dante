//
//  Robot.h
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Robot__
#define __noctisgames__Robot__

#include "Entity.h"

#include "GameConstants.h"

#include "RTTI.h"

#include <stdint.h>

class IInputState;
class Move;

class Robot : public Entity
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
    
    static Entity* create();
    
    virtual void onDeletion();
    
    virtual void update();
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    void takeDamage();
    
    void setPlayerId(uint32_t inPlayerId);
    
    uint32_t getPlayerId() const;
    
    void setIndexInWorld(int inIndex);
    
    int getIndexInWorld() const;
    
    bool isFacingLeft();
    
    bool isGrounded();
    
    bool isShooting();
    
private:
    float m_fSpeed;
    float m_fJumpSpeed;
    float m_fTimeOfNextShot;
    
    //bounce fraction when hitting various things
    float m_fWallRestitution;
    float m_fCatRestitution;
    
    float m_fTimePositionBecameOutOfSync;
    float m_fTimeVelocityBecameOutOfSync;
    
    int m_iHealth;
    
    bool m_isFacingLeft;
    bool m_isGrounded;
    bool m_isFalling;
    bool m_isShooting;
    
    uint32_t m_iPlayerId;
    
    int m_iIndexInWorld;
    
    void processMove(Move& inMove);
    
    void updateInternal(float inDeltaTime);
    
    void processInput(float inDeltaTime, IInputState* inInputState);
    
    void processCollisions();
    
    void processCollisionsWithScreenWalls();
    
#ifdef NG_SERVER
    void handleShooting();
#elif NG_CLIENT
    void doClientSidePredictionAfterReplicationForLocalCat(uint32_t inReadState);
    
    void doClientSidePredictionAfterReplicationForRemoteCat(uint32_t inReadState);
    
    void interpolateClientSidePrediction(Vector2& inOldLocation, Vector2& inOldVelocity);
#endif
    
    Robot();
};

#endif /* defined(__noctisgames__Robot__) */
