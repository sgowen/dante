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
