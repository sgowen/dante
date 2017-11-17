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
class Projectile;

#define NUM_PROJECTILES 6

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
    
    virtual bool needsMoveReplay();
    
    void processInput(IInputState* inInputState, bool isPending = false);
    
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
    
    bool isPending();
    
private:
    uint64_t _addressHash;
    uint32_t _firstProjectileId;
    uint8_t _playerId;
    std::string _playerName;
    
    uint8_t _numJumps;
    uint8_t _nextProjectileIndex;
    bool _isFacingLeft;
    bool _isShooting;
    bool _isSprinting;
    bool _isFirstJumpCompleted;
    
    uint8_t _health;
    
    uint32_t _numKills;
    bool _wasLastKillHeadshot;
    
    float _speed;
    float _jumpSpeed;
    
    float _shotCooldownTime;
    int _numSpacePiratesTouching;
    bool _hasInitializedProjectiles;
    bool _isPending;
    
    // Cached Last Known Values (from previous frame)
    uint8_t _numJumpsLastKnown;
    uint8_t _nextProjectileIndexLastKnown;
    uint8_t _healthLastKnown;
    uint32_t _numKillsLastKnown;
    float _shotCooldownTimeLastKnown;
    bool _wasLastKillHeadshotLastKnown;
    bool _isFacingLeftLastKnown;
    bool _isShootingLastKnown;
    bool _isSprintingLastKnown;
    bool _isFirstJumpCompletedLastKnown;
    
    Projectile* _projectiles[NUM_PROJECTILES];
    
    void fireProjectile();
    
    void playNetworkBoundSounds(int numJumpsLastKnown, bool isSprintingLastKnown);
};

#endif /* defined(__noctisgames__Robot__) */
