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
    
    virtual void update();
    virtual void receiveMessage(uint16_t message, void* data = NULL);
    virtual void onFixturesCreated(std::vector<b2Fixture*>& fixtures);
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void read(InputMemoryBitStream& inInputStream, uint16_t& inReadState);
    virtual void recallLastReadState(uint16_t& inReadState);
    virtual uint16_t write(OutputMemoryBitStream& inOutputStream, uint16_t inWrittenState, uint16_t inDirtyState);
    
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
    bool isLocalPlayer();
    
private:
    enum State
    {
        State_Idle = 0,
        State_FirstPunch,
        State_SecondPunch,
        State_ThirdPunch,
        State_Running,
        State_Jumping,
        State_JumpCompleted
    };
    
    enum MainActionFlags
    {
        MainActionFlags_ReadyForSecondPunch = 1,
        MainActionFlags_ToThrowSecondPunch,
        MainActionFlags_ReadyForThirdPunch,
        MainActionFlags_ToThrowThirdPunch
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
            health = 3;
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
    
    /// Non-Networked
    b2Fixture* _attackSensorFixture;
    bool _isLocalPlayer;
    bool _isPendingInput;
    
    void processInputForIdleState(uint8_t inputState);
    void processInputForFirstPunchState(uint8_t inputState);
    void processInputForSecondPunchState(uint8_t inputState);
    void processInputForThirdPunchState(uint8_t inputState);
    void processInputForRunningState(uint8_t inputState);
    void processInputForJumpingState(uint8_t inputState);
    void processInputForFirstJumpCompletedState(uint8_t inputState);
    
    void handleMovementInput(uint8_t inputState);
    void handleMainActionInput(uint8_t inputState);
    void handleJumpInput(uint8_t inputState);
    void handleJumpCompletedInput(uint8_t inputState);
    
    void handleSound(uint8_t fromState, uint8_t toState);
};

#endif /* defined(__noctisgames__PlayerController__) */
