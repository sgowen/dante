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
        ROBT_Stats = 1 << 2,
        
        ROBT_AllState = ROBT_PlayerInfo | ROBT_Pose | ROBT_Stats
    };
    
    Robot(b2World& world, bool isServer);
    
    virtual ~Robot();
    
    virtual EntityDef constructEntityDef();
    
    virtual void update();
    
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    void postRead();
    
    void processInput(IInputState* inInputState, bool isPending = false);
    
    void updateInternal(float inDeltaTime);
    
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
    
    bool isShooting();
    
    bool isSprinting();
    
    bool needsMoveReplay();
    
private:
    uint64_t m_iAddressHash;
    uint8_t m_iPlayerId;
    std::string m_playerName;
    
    uint8_t m_iNumJumps;
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
    
    bool m_isFirstJumpCompleted;
    
    // Cached Last Known Values (from previous frame)
    uint8_t m_iNumJumpsLastKnown;
    uint8_t m_iHealthLastKnown;
    uint32_t m_iNumKillsLastKnown;
    bool m_wasLastKillHeadshotLastKnown;
    bool m_isFacingLeftLastKnown;
    bool m_isShootingLastKnown;
    bool m_isSprintingLastKnown;
    
    void handleShooting();
    
    void playNetworkBoundSounds(int numJumpsLastKnown, bool isSprintingLastKnown);
};

#endif /* defined(__noctisgames__Robot__) */
