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
#include <string>

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
        ROBT_PlayerInfo = 1 << 2,
        ROBT_Health = 1 << 3,
        ROBT_NumKills = 1 << 4,
        
        ROBT_AllState = ROBT_Pose | ROBT_Color | ROBT_PlayerInfo | ROBT_Health | ROBT_NumKills
    };
    
    Robot(bool isServer);
    
    virtual void update();
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    void takeDamage();
    
    void awardKill(bool isHeadshot);
    
    void setAddressHash(uint64_t addressHash);
    
    uint64_t getAddressHash() const;
    
    void setPlayerId(uint8_t inPlayerId);
    
    uint8_t getPlayerId() const;
    
    void setPlayerName(std::string playerName);
    
    std::string& getPlayerName();
    
    uint32_t getNumKills();
    
    uint8_t getHealth();
    
    bool isFacingLeft();
    
    bool isGrounded();
    
    bool isShooting();
    
    bool isSprinting();
    
private:
    uint64_t m_iAddressHash;
    uint8_t m_iPlayerId;
    std::string m_playerName;
    
    uint8_t m_iRttMs;
    uint8_t m_iNumJumps;
    bool m_isGrounded;
    bool m_isFalling;
    bool m_isFacingLeft;
    bool m_isShooting;
    bool m_isSprinting;
    
    uint8_t m_iHealth;
    
    uint32_t m_iNumKills;
    bool m_wasLastKillHeadshot;
    
    float m_fSpeed;
    float m_fJumpSpeed;
    float m_fTimeOfNextShot;
    float m_fRobotRestitution;
    float m_fTimeAccelerationBecameOutOfSync;
    float m_fTimeVelocityBecameOutOfSync;
    float m_fTimePositionBecameOutOfSync;
    
    bool m_isServer;
    bool m_isFirstJumpCompleted;
    
    void processMove(const Move& inMove);
    
    void processInput(float inDeltaTime, IInputState* inInputState);
    
    void updateInternal(float inDeltaTime);
    
    void processCollisions();
    
    void processCollisionsWithScreenWalls();
    
    void handleShooting();

    void doClientSidePredictionForLocalRobot(uint32_t inReadState);
    
    void doClientSidePredictionForRemoteRobot(uint32_t inReadState);
    
    void interpolateClientSidePrediction(Vector2& inOldAcceleration, Vector2& inOldVelocity, Vector2& inOldPos);
    
    void interpolateVectorsIfNecessary(Vector2& inA, Vector2& inB, float& syncTracker, const char* vectorType);
    
    void playNetworkBoundSounds(uint8_t old_m_iNumJumps, bool old_m_isSprinting);
    
    void playSound(int soundId);
};

#endif /* defined(__noctisgames__Robot__) */
