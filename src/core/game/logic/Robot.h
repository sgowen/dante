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
        ROBT_Pose = 1 << 0,
        ROBT_Color = 1 << 1,
        ROBT_PlayerId = 1 << 2,
        ROBT_Health = 1 << 3,
        
        ROBT_AllState = ROBT_Pose | ROBT_Color | ROBT_PlayerId | ROBT_Health
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
    
    bool isFacingLeft();
    
    bool isGrounded();
    
    bool isShooting();
    
private:
    float m_fSpeed;
    float m_fJumpSpeed;
    float m_fTimeOfNextShot;
    
    //bounce fraction when hitting various things
    float m_fWallRestitution;
    float m_fRobotRestitution;
    
    float m_fTimeAccelerationBecameOutOfSync;
    float m_fTimeVelocityBecameOutOfSync;
    float m_fTimePositionBecameOutOfSync;
    
    int m_iHealth;
    
    bool m_isFacingLeft;
    bool m_isGrounded;
    bool m_isFalling;
    bool m_isShooting;
    bool m_isJumping;
    
    uint32_t m_iPlayerId;
    
    void processMove(const Move& inMove);
    
    void processInput(float inDeltaTime, IInputState* inInputState);
    
    void updateInternal(float inDeltaTime);
    
    void processCollisions();
    
    void processCollisionsWithScreenWalls();
    
#ifdef NG_SERVER
    void handleShooting();
#elif NG_CLIENT
    void doClientSidePredictionAfterReplicationForLocalRobot(uint32_t inReadState);
    
    void doClientSidePredictionAfterReplicationForRemoteRobot(uint32_t inReadState);
    
    void interpolateClientSidePrediction(float& inOldStateTime, Vector2& inOldAcceleration, Vector2& inOldVelocity, Vector2& inOldPos);
    
    void interpolateClientSidePrediction(float& inOldStateTime, Vector2& inOldPos);
    
    bool interpolateVectorsIfNecessary(Vector2& inA, Vector2& inB, float& syncTracker);
#endif
    
    void playSound(int soundId);
    
    Robot();
};

#endif /* defined(__noctisgames__Robot__) */
