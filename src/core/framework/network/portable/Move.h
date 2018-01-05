//
//  Move.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Move__
#define __noctisgames__Move__

#include "Box2D/Common/b2Math.h"

class InputState;
class OutputMemoryBitStream;
class InputMemoryBitStream;

class Move
{
public:
    Move(InputState* inInputState, float inTimestamp);
    
    Move(InputState* inInputState);
    
    ~Move();
    
    bool write(OutputMemoryBitStream& inOutputStream) const;
    
    bool read(InputMemoryBitStream& inInputStream);
    
    bool isEqual(const Move* inMove) const;
    
    InputState* getInputState() const;
    
    float getTimestamp() const;
    
    void setTimestamp(float timeStamp);
    
    void copyInputState(InputState* inInputState);
    
private:
    InputState* _inputState;
    float _timestamp;
};

#endif /* defined(__noctisgames__Move__) */
