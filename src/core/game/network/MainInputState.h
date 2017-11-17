//
//  MainInputState.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__MainInputState__
#define __noctisgames__MainInputState__

#include "InputState.h"

#include "RTTI.h"

#define MENU_STATE_NONE 0
#define MENU_STATE_ACTIVATE_STEAM 1
#define MENU_STATE_DEACTIVATE_STEAM 2
#define MENU_STATE_START_SERVER 3
#define MENU_STATE_JOIN_LOCAL_SERVER 4
#define MENU_STATE_STEAM_REFRESH_LAN_SERVERS 5
#define MENU_STATE_STEAM_REFRESH_INTERNET_SERVERS 6
#define MENU_STATE_STEAM_JOIN_SERVER_1 7
#define MENU_STATE_STEAM_JOIN_SERVER_2 8
#define MENU_STATE_STEAM_JOIN_SERVER_3 9
#define MENU_STATE_STEAM_JOIN_SERVER_4 10
#define MENU_STATE_ESCAPE 11
#define MENU_STATE_LOCAL_PLAYER_DROP_OUT_1 12
#define MENU_STATE_LOCAL_PLAYER_DROP_OUT_2 13
#define MENU_STATE_LOCAL_PLAYER_DROP_OUT_3 14
#define MENU_STATE_SERVER_TOGGLE_ENEMIES 15
#define MENU_STATE_SERVER_TOGGLE_OBJECTS 16
#define MENU_STATE_SERVER_TOGGLE_SERVER_DISPLAY 17
#define MENU_STATE_CLIENT_MAIN_TOGGLE_MUSIC 18
#define MENU_STATE_CLIENT_MAIN_TOGGLE_SOUND 19

class OutputMemoryBitStream;
class InputMemoryBitStream;

class MainInputState : public InputState
{
    RTTI_DECL;
    
    friend class InputManager;
    
public:
    class GameInputState
    {
        friend class MainInputState;
        friend class InputManager;
        
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
    
    virtual ~MainInputState();
    
    virtual bool write(OutputMemoryBitStream& inOutputStream) const;
    
    virtual bool read(InputMemoryBitStream& inInputStream);
    
    virtual void reset();
    
    virtual bool isEqual(InputState* inInputState) const;
    
    virtual void copyTo(InputState* inInputState) const;
    
    void activateNextPlayer(uint8_t playerId);
    
    GameInputState* getGameInputStateForPlayerId(uint8_t playerId);
    
    bool isPlayerIdLocalHost(uint8_t playerId);
    
    bool isRequestingToAddLocalPlayer() const;
    
    int getMenuState() const;
    
private:
    GameInputState _gameMainInputStates[4];
    
    int _menuState;
    
    GameInputState& getGameInputState(int index);
};

#endif /* defined(__noctisgames__MainInputState__) */
