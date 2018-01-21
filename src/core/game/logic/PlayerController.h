//
//  PlayerController.h
//  dante
//
//  Created by Stephen Gowen on 1/5/18.
//  Copyright © 2018 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__PlayerController__
#define __noctisgames__PlayerController__

#include <framework/entity/EntityController.h>

#include <string>

class InputState;
class Move;

class PlayerController : public EntityController
{
    NGRTTI_DECL;
    
public:
    static EntityController* create(Entity* inEntity);
    
    PlayerController(Entity* inEntity);
    virtual ~PlayerController();
    
    virtual uint8_t update();
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void read(InputMemoryBitStream& inInputStream, uint16_t& inReadState);
    virtual void recallLastReadState(uint16_t& inReadState);
    virtual uint16_t write(OutputMemoryBitStream& inOutputStream, uint16_t inWrittenState, uint16_t inDirtyState);
    virtual std::vector<std::string> getStates()
    {
        std::vector<std::string> ret;
        ret.push_back("Idle");
        ret.push_back("Punching");
        ret.push_back("Running");
        ret.push_back("Jumping");
        return ret;
    }
    
    void processInput(InputState* inInputState, bool isPending = false);
    
    /// Helpers
    void setAddressHash(uint64_t inValue);
    uint64_t getAddressHash() const;
    void setPlayerId(uint8_t inValue);
    uint8_t getPlayerId() const;
    void setMap(uint32_t inValue);
    uint32_t getMap() const;
    void setPlayerName(std::string inValue);
    std::string& getPlayerName();
    uint8_t getHealth();
    uint8_t getNumJumps();
    bool isMainAction();
    bool isMoving();
    bool isLocalPlayer();
    
private:
    enum State
    {
        State_Idle = 0,
        State_Punching,
        State_Running,
        State_Jumping
    };
    
    enum StateFlags
    {
        StateFlag_MainAction = 1 << 0,
        StateFlag_FirstJump = 1 << 1,
        StateFlag_FirstJumpCompleted = 1 << 2,
        StateFlag_SecondJump = 1 << 3
    };
    
    enum ReadStateFlag
    {
        ReadStateFlag_PlayerInfo = 1 << 1,
        ReadStateFlag_Stats = 1 << 2
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
    
    float _maxXVelocity;
    float _maxYVelocity;
    bool _isLocalPlayer;
};

#endif /* defined(__noctisgames__PlayerController__) */
