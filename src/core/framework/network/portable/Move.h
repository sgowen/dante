//
//  Move.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Move__
#define __noctisgames__Move__

class IInputState;
class OutputMemoryBitStream;
class InputMemoryBitStream;

class Move
{
public:
    Move(IInputState* inInputState, float inTimestamp, float inDeltaTime);
    
    Move(IInputState* inInputState);
    
    ~Move();
    
    bool write(OutputMemoryBitStream& inOutputStream) const;
    
    bool read(InputMemoryBitStream& inInputStream);
    
    IInputState* getInputState() const;
    
    float getTimestamp() const;
    
    float getDeltaTime() const;
    
private:
    IInputState* m_inputState;
    float m_fTimestamp;
    float m_fDeltaTime;
};

#endif /* defined(__noctisgames__Move__) */
