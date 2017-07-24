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
#include "Box2D/Common/b2Math.h"

#include <string>

class IInputState;
class Move;
class Ground;
class Crate;

class Robot : public Entity
{
    RTTI_DECL;
    
    NW_TYPE_DECL(NW_TYPE_Robot);
    
public:
    enum RobotReplicationState
    {
        ROBT_PlayerInfo = 1 << 0,
        ROBT_Pose = 1 << 1,
        
        ROBT_AllState = ROBT_PlayerInfo
    };
    
    Robot(b2World& world, bool isServer);
    
    virtual ~Robot();
    
    virtual EntityDef constructEntityDef();
    
    virtual void update();
    
    virtual bool shouldCollide(Entity* inEntity);
    
    virtual void handleContact(Entity* inEntity);
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    void postRead();
    
    void syncToHost();
    
    void handleContactWithGround(Ground* inGround);
    
    void handleContactWithCrate(Crate* inCrate);
    
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
    
    uint8_t m_iNumJumps;
    bool m_isGrounded;
    bool m_isFalling;
    bool m_isFacingLeft;
    bool m_isShooting;
    bool m_isSprinting;
    
    uint8_t m_iHealth;
    
    uint32_t m_iNumKills;
    bool m_wasLastKillHeadshot;
    
    uint32_t m_iReadState;
    
    float m_fSpeed;
    float m_fJumpSpeed;
    float m_fTimeOfNextShot;
    float m_fTimeVelocityBecameOutOfSync;
    float m_fTimePositionBecameOutOfSync;
    
    bool m_isServer;
    bool m_isFirstJumpCompleted;
    
    // Cached Values
    b2Vec2 m_velocityOld;
    b2Vec2 m_positionOld;
    uint8_t m_iNumJumpsOld;
    bool m_isSprintingOld;
    
    bool m_isHost;
    b2Body* m_hostBody;
    b2Fixture* m_hostFixture;
    
    void processMove(const Move& inMove);
    
    void processInput(IInputState* inInputState);
    
    void updateInternal(float inDeltaTime);
    
    void stepPhysics(float deltaTime);
    
    void handleShooting();

    void doClientSidePredictionForLocalRobot();
    
    void doClientSidePredictionForRemoteRobot();
    
    void interpolateClientSidePrediction(b2Vec2& inOldVelocity, b2Vec2& inOldPos);
    
    bool interpolateVectorsIfNecessary(b2Vec2& inA, const b2Vec2& inB, float& syncTracker, const char* vectorType);
    
    void playNetworkBoundSounds(uint8_t old_m_iNumJumps, bool old_m_isSprinting);
    
    void playSound(int soundId);
};

#endif /* defined(__noctisgames__Robot__) */
