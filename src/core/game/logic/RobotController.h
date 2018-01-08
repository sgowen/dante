//
//  RobotController.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RobotController__
#define __noctisgames__RobotController__

#include <framework/entity/EntityController.h>

#include <string>

class InputState;
class Move;

class RobotController : public EntityController
{
    NGRTTI_DECL;
    
public:
    static EntityController* create(Entity* inEntity);
    
    RobotController(Entity* inEntity);
    virtual ~RobotController();
    
    virtual void update();
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void read(InputMemoryBitStream& inInputStream);
    virtual uint16_t write(OutputMemoryBitStream& inOutputStream, uint16_t inWrittenState, uint16_t inDirtyState);
    virtual void recallLastReadState();
    
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
    enum RobotState
    {
        ROBT_FACING_LEFT = 1 << 0,
        ROBT_MAIN_ACTION = 1 << 1,
        ROBT_SPRINTING = 1 << 2,
        ROBT_FIRST_JUMP = 1 << 3,
        ROBT_FIRST_JUMP_COMPLETED = 1 << 4,
        ROBT_SECOND_JUMP = 1 << 5
    };
    
    enum RobotReplicationState
    {
        ROBT_PlayerInfo = 1 << 1,
        ROBT_Stats = 1 << 2
    };
    
    struct PlayerInfo
    {
        uint64_t addressHash;
        uint8_t playerId;
        std::string playerName;
        
        PlayerInfo()
        {
            addressHash = 0;
            playerId = 0;
            playerName = "Robot";
        }
        
        friend bool operator==(PlayerInfo& lhs, PlayerInfo& rhs)
        {
            return
            lhs.addressHash == rhs.addressHash &&
            lhs.playerId    == rhs.playerId &&
            lhs.playerName  == rhs.playerName;
        }
        
        friend bool operator!=(PlayerInfo& lhs, PlayerInfo& rhs)
        {
            return !(lhs == rhs);
        }
    };
    PlayerInfo _playerInfo;
    PlayerInfo _playerInfoCache;
    
    struct Stats
    {
        uint8_t health;
        
        Stats()
        {
            health = 255;
        }
        
        friend bool operator==(Stats& lhs, Stats& rhs)
        {
            return
            lhs.health == rhs.health;
        }
        
        friend bool operator!=(Stats& lhs, Stats& rhs)
        {
            return !(lhs == rhs);
        }
    };
    Stats _stats;
    Stats _statsCache;
};

#endif /* defined(__noctisgames__RobotController__) */
