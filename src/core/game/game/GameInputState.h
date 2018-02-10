//
//  GameInputState.h
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__GameInputState__
#define __noctisgames__GameInputState__

#include <framework/network/portable/InputState.h>

#include <framework/util/NGRTTI.h>

class OutputMemoryBitStream;
class InputMemoryBitStream;

class GameInputState : public InputState
{
    NGRTTI_DECL;
    
    friend class GameInputManager;
    
public:
    class PlayerInputState
    {
        friend class GameInputState;
        friend class GameInputManager;
        
    public:
        void write(OutputMemoryBitStream& inOutputStream) const;
        void read(InputMemoryBitStream& inInputStream);
        int getDesiredRightAmount();
        bool isJumping();
        bool isMainAction();
        
    private:
        uint8_t _playerId;
        bool _isMovingRight;
        bool _isMovingLeft;
        bool _isJumping;
        bool _isMainAction;
        
        PlayerInputState();
    };
    
    GameInputState();
    
    virtual void write(OutputMemoryBitStream& inOutputStream) const;
    virtual void read(InputMemoryBitStream& inInputStream);
    virtual void reset();
    virtual bool isEqual(InputState* inInputState) const;
    virtual void copyTo(InputState* inInputState) const;
    
    void activateNextPlayer(uint8_t playerId);
    PlayerInputState* getPlayerInputStateForID(uint8_t playerId);
    bool isRequestingToAddLocalPlayer() const;
    
private:
    PlayerInputState _playerInputStates[4];
    
    PlayerInputState& getPlayerInputState(int index);
};

#endif /* defined(__noctisgames__GameInputState__) */
