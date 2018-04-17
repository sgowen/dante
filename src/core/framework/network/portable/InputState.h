//
//  InputState.h
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InputState__
#define __noctisgames__InputState__

#include <framework/util/RTTI.h>

class OutputMemoryBitStream;
class InputMemoryBitStream;

class InputState
{
    DECL_RTTI;
    
public:
    InputState();
    virtual ~InputState();
    
    virtual void write(OutputMemoryBitStream& inOutputStream) const = 0;
    virtual void read(InputMemoryBitStream& inInputStream) = 0;
    virtual void reset() = 0;
    virtual bool isEqual(InputState* inInputState) const = 0;
    virtual void copyTo(InputState* inInputState) const = 0;
};

#endif /* defined(__noctisgames__InputState__) */
