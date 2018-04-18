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
    friend class PlayerNetworkController;
    
    DECL_RTTI;
    DECL_EntityController_create;
    
public:
    PlayerController(Entity* entity);
    virtual ~PlayerController();
    
    virtual void update();
    virtual void receiveMessage(uint16_t message, void* data = NULL);
    virtual void onFixturesCreated(std::vector<b2Fixture*>& fixtures);
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    
    void processInput(InputState* inInputState);
    
    /// Helpers
    void setAddressHash(uint64_t inValue);
    uint64_t getAddressHash() const;
    void setPlayerId(uint8_t inValue);
    uint8_t getPlayerId() const;
    void setMap(uint32_t inValue);
    uint32_t getMap() const;
    void setPlayerName(std::string inValue);
    std::string& getPlayerName();
    
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
        MainActionFlag_ReadyForSecondPunch = 1,
        MainActionFlag_ToThrowSecondPunch,
        MainActionFlag_ReadyForThirdPunch,
        MainActionFlag_ToThrowThirdPunch
    };
    
    enum ReadStateFlag
    {
        ReadStateFlag_PlayerInfo = 1 << 2,
        ReadStateFlag_Stats =      1 << 3
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
    PlayerInfo _playerInfoNetworkCache;
    
    struct Stats
    {
        uint8_t health;
        uint32_t target;
        
        Stats()
        {
            health = 3;
            target = 0;
        }
        
        friend bool operator==(Stats& lhs, Stats& rhs)
        {
            return
            lhs.health == rhs.health &&
            lhs.target == rhs.target;
        }
        
        friend bool operator!=(Stats& lhs, Stats& rhs)
        {
            return !(lhs == rhs);
        }
    };
    Stats _stats;
    Stats _statsNetworkCache;
    
    /// Non-Networked
    b2Fixture* _attackSensorFixture;
    
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
};

#include <framework/entity/EntityNetworkController.h>

class PlayerNetworkController : public EntityNetworkController
{
    DECL_RTTI;
    DECL_EntityNetworkController_create;
    
public:
    PlayerNetworkController(Entity* e, bool isServer);
    virtual ~PlayerNetworkController();
    
    virtual void read(InputMemoryBitStream& ip);
    virtual uint16_t write(OutputMemoryBitStream& op, uint16_t dirtyState);
    
    virtual void recallNetworkCache();
    virtual uint16_t getDirtyState();
    
    bool isLocalPlayer();
    
private:
    PlayerController* _controller;
    bool _isLocalPlayer;
};

#endif /* defined(__noctisgames__PlayerController__) */
