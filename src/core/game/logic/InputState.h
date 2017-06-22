//
//  InputState.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InputState__
#define __noctisgames__InputState__

class OutputMemoryBitStream;
class InputMemoryBitStream;

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

class InputState : public IInputState
{
    RTTI_DECL;
    
public:
    InputState();
    
    virtual ~InputState();
    
    virtual bool write(OutputMemoryBitStream& inOutputStream) const;
    virtual bool read(InputMemoryBitStream& inInputStream);
    
    virtual void reset();
    
    void copyTo(InputState* inInputState);
    
    bool isEqualTo(InputState* inInputState);
    
    float getDesiredHorizontalDelta() const;
    
    bool isJumping() const;
    
    bool isShooting() const;
    
    bool isSprinting() const;
    
    int getMenuState() const;
    
private:
    friend class InputManager;
    
    float m_fDesiredRightAmount;
    float m_fDesiredLeftAmount;
    bool m_isJumping;
    bool m_isShooting;
    bool m_isSprinting;
    
    int m_iMenuState;
};

#endif /* defined(__noctisgames__InputState__) */
