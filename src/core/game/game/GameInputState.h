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

enum GameInputStateFlags
{
    GameInputStateFlag_MovingRight = 1 << 0,
    GameInputStateFlag_MovingLeft =  1 << 1,
    GameInputStateFlag_Jumping =     1 << 2,
    GameInputStateFlag_MainAction =  1 << 3
};

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
        uint8_t getInputState();
        
    private:
        uint8_t _playerId;
        uint8_t _inputState;
        
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
