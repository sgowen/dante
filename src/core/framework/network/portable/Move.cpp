//
//  Move.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Move.h"

#include "MemoryBitStream.h"

bool Move::write(OutputMemoryBitStream& inOutputStream) const
{
    mInputState.write(inOutputStream);
    inOutputStream.write(mTimestamp);
    
    return true;
}

bool Move::read(InputMemoryBitStream& inInputStream)
{
    mInputState.read(inInputStream);
    inInputStream.read(mTimestamp);
    
    return true;
}
