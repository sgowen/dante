//
//  MainInputState.h
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__MainInputState__
#define __noctisgames__MainInputState__

#include "framework/network/portable/InputState.h"

#include "framework/util/NGRTTI.h"

#define MS_NONE 0
#define MS_ACTIVATE_STEAM 1
#define MS_DEACTIVATE_STEAM 2
#define MS_START_SERVER 3
#define MS_JOIN_LOCAL_SERVER 4
#define MS_STEAM_REFRESH_LAN_SERVERS 5
#define MS_STEAM_REFRESH_INTERNET_SERVERS 6
#define MS_STEAM_JOIN_SERVER_1 7
#define MS_STEAM_JOIN_SERVER_2 8
#define MS_STEAM_JOIN_SERVER_3 9
#define MS_STEAM_JOIN_SERVER_4 10
#define MS_ESCAPE 11
#define MS_LOCAL_PLAYER_DROP_OUT_1 12
#define MS_LOCAL_PLAYER_DROP_OUT_2 13
#define MS_LOCAL_PLAYER_DROP_OUT_3 14
#define MS_SERVER_TOGGLE_ENEMIES 15
#define MS_SERVER_TOGGLE_OBJECTS 16
#define MS_SERVER_TOGGLE_SERVER_DISPLAY 17
#define MS_SERVER_TOGGLE_PHYSICS_DISPLAY 18
#define MS_CLIENT_MAIN_TOGGLE_MUSIC 19
#define MS_CLIENT_MAIN_TOGGLE_SOUND 20

class OutputMemoryBitStream;
class InputMemoryBitStream;

class MainInputState : public InputState
{
    NGRTTI_DECL;
    
    friend class GameInputManager;
    
public:
    class GameInputState
    {
        friend class MainInputState;
        friend class GameInputManager;
        
    public:
        void write(OutputMemoryBitStream& inOutputStream) const;
        
        void read(InputMemoryBitStream& inInputStream);
        
        int getDesiredRightAmount();
        
        bool isJumping();
        
        bool isShooting();
        
        bool isSprinting();
        
    private:
        uint8_t _playerId;
        bool _isMovingRight;
        bool _isMovingLeft;
        bool _isJumping;
        bool _isShooting;
        bool _isSprinting;
        
        GameInputState();
    };
    
    MainInputState();
    
    virtual bool write(OutputMemoryBitStream& inOutputStream) const;
    
    virtual bool read(InputMemoryBitStream& inInputStream);
    
    virtual void reset();
    
    virtual bool isEqual(InputState* inInputState) const;
    
    virtual void copyTo(InputState* inInputState) const;
    
    void activateNextPlayer(uint8_t playerId);
    
    GameInputState* getGameInputStateForPlayerId(uint8_t playerId);
    
    bool isPlayerIdLocalHost(uint8_t playerId);
    
    bool isRequestingToAddLocalPlayer() const;
    
private:
    GameInputState _gameMainInputStates[4];
    
    GameInputState& getGameInputState(int index);
};

#endif /* defined(__noctisgames__MainInputState__) */
