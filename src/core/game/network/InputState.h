//
//  InputState.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InputState__
#define __noctisgames__InputState__

#include "IInputState.h"

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

class InputState : public IInputState
{
    RTTI_DECL;
    
public:
    class GameInputState
    {
    public:
        void write(OutputMemoryBitStream& inOutputStream) const;
        
        void read(InputMemoryBitStream& inInputStream);
        
        int getDesiredRightAmount();
        
        bool isJumping();
        
        bool isShooting();
        
        bool isSprinting();
        
    private:
        friend class InputState;
        friend class InputManager;
        
        uint8_t m_iPlayerId;
        bool m_isMovingRight;
        bool m_isMovingLeft;
        bool m_isJumping;
        bool m_isShooting;
        bool m_isSprinting;
        
        GameInputState();
    };
    
    InputState();
    
    virtual ~InputState();
    
    virtual bool write(OutputMemoryBitStream& inOutputStream) const;
    
    virtual bool read(InputMemoryBitStream& inInputStream);
    
    virtual void reset();
    
    virtual bool isEqual(IInputState* inIInputState) const;
    
    virtual void copyTo(IInputState* inIInputState) const;
    
    void activateNextPlayer(uint8_t playerId);
    
    GameInputState* getGameInputStateForPlayerId(uint8_t playerId);
    
    bool isPlayerIdLocalHost(uint8_t playerId);
    
    bool isRequestingToAddLocalPlayer() const;
    
    int getMenuState() const;
    
private:
    friend class InputManager;
    
    GameInputState m_gameInputStates[4];
    
    int m_iMenuState;
    
    GameInputState& getGameInputState(int index);
};

#endif /* defined(__noctisgames__InputState__) */
