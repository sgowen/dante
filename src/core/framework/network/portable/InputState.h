//
//  InputState.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InputState__
#define __noctisgames__InputState__

#include "framework/util/NGRTTI.h"

class OutputMemoryBitStream;
class InputMemoryBitStream;

class InputState
{
    NGRTTI_DECL;
    
public:
    InputState();
    
    virtual ~InputState();
    
    virtual bool write(OutputMemoryBitStream& inOutputStream) const = 0;
    
    virtual bool read(InputMemoryBitStream& inInputStream) = 0;
    
    virtual void reset() = 0;
    
    virtual bool isEqual(InputState* inInputState) const = 0;
    
    virtual void copyTo(InputState* inInputState) const = 0;
    
    void setInUse(bool inUse);
    
    bool isInUse() const;
    
private:
    bool _isInUse;
};

#endif /* defined(__noctisgames__InputState__) */
