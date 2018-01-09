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

#define MIS_NONE 0
#define MIS_ENTER_STUDIO 1
#define MIS_ACTIVATE_STEAM 2
#define MIS_DEACTIVATE_STEAM 3
#define MIS_START_SERVER 4
#define MIS_JOIN_LOCAL_SERVER 5
#define MIS_STEAM_REFRESH_LAN_SERVERS 6
#define MIS_STEAM_REFRESH_INTERNET_SERVERS 7
#define MIS_STEAM_JOIN_SERVER_1 8
#define MIS_STEAM_JOIN_SERVER_2 9
#define MIS_STEAM_JOIN_SERVER_3 10
#define MIS_STEAM_JOIN_SERVER_4 11
#define MIS_ESCAPE 12
#define MIS_LOCAL_PLAYER_DROP_OUT_1 13
#define MIS_LOCAL_PLAYER_DROP_OUT_2 14
#define MIS_LOCAL_PLAYER_DROP_OUT_3 15
#define MIS_SERVER_TOGGLE_ENEMIES 16
#define MIS_SERVER_TOGGLE_OBJECTS 17
#define MIS_SERVER_TOGGLE_SERVER_DISPLAY 18
#define MIS_TOGGLE_PHYSICS_DISPLAY 19
#define MIS_CLIENT_MAIN_TOGGLE_MUSIC 20
#define MIS_CLIENT_MAIN_TOGGLE_SOUND 21

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
        
        bool isMainAction();
        
        bool isSprinting();
        
    private:
        uint8_t _playerId;
        bool _isMovingRight;
        bool _isMovingLeft;
        bool _isJumping;
        bool _isMainAction;
        bool _isSprinting;
        
        GameInputState();
    };
    
    MainInputState();
    
    virtual void write(OutputMemoryBitStream& inOutputStream) const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
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
