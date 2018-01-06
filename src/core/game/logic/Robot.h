//
//  Robot.h
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Robot__
#define __noctisgames__Robot__

#include "framework/entity/Entity.h"

#include "game/logic/GameConstants.h"

#include <string>

class InputState;
class Move;
class Ground;
class Crate;
class Projectile;

class Robot : public Entity
{
    NGRTTI_DECL;
    
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
    
    virtual void update();
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual uint32_t getAllStateMask() const;
    virtual void read(InputMemoryBitStream& inInputStream);
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    virtual bool needsMoveReplay();
    
    void processInput(InputState* inInputState, bool isPending = false);
    
    void setAddressHash(uint64_t addressHash);
    uint64_t getAddressHash() const;
    void setPlayerId(uint8_t inPlayerId);
    uint8_t getPlayerId() const;
    void setPlayerName(std::string playerName);
    std::string& getPlayerName();
    uint8_t getHealth();
    uint8_t getNumJumps();
    bool isFacingLeft();
    bool isMainAction();
    bool isSprinting();
    
private:
    struct PlayerInfo
    {
        PlayerInfo()
        {
            _addressHash = 0;
            _playerId = 0;
            _playerName = "Robot";
        }
        
        uint64_t _addressHash;
        uint8_t _playerId;
        std::string _playerName;
    };
    
    PlayerInfo _playerInfo;
    uint8_t _numJumps;
    bool _isFacingLeft;
    bool _isMainAction;
    bool _isSprinting;
    bool _isFirstJumpCompleted;
    
    uint8_t _health;
    
    float _speed;
    float _sprintSpeed;
    float _movementForce;
    float _sprintMovementForce;
    
    // Cached Last Known Values (from previous frame)
    uint8_t _numJumpsLastKnown;
    uint8_t _healthLastKnown;
    bool _isFacingLeftLastKnown;
    bool _isSprintingLastKnown;
    bool _isFirstJumpCompletedLastKnown;
    
    void playNetworkBoundSounds(uint8_t numJumpsLastKnown, bool isSprintingLastKnown);
};

#endif /* defined(__noctisgames__Robot__) */
