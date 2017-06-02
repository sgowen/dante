//
//  IInputState.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__IInputState__
#define __noctisgames__IInputState__

class OutputMemoryBitStream;
class InputMemoryBitStream;

#include "RTTI.h"

class IInputState
{
    RTTI_DECL;
    
public:
    IInputState();
    
    virtual ~IInputState();
    
    virtual bool write(OutputMemoryBitStream& inOutputStream) const = 0;
    virtual bool read(InputMemoryBitStream& inInputStream) = 0;
};

#endif /* defined(__noctisgames__IInputState__) */
