//
//  AckRange.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "AckRange.h"

#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"

void AckRange::write(OutputMemoryBitStream& inOutputStream) const
{
    inOutputStream.write(mStart);
    bool hasCount = mCount > 1;
    inOutputStream.write(hasCount);
    if (hasCount)
    {
        //most you can ack is 255...
        uint32_t countMinusOne = mCount - 1;
        uint8_t countToAck = countMinusOne > 255 ? 255 : static_cast<uint8_t>(countMinusOne);
        inOutputStream.write(countToAck);
    }
}

void AckRange::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.read(mStart);
    bool hasCount;
    inInputStream.read(hasCount);
    if (hasCount)
    {
        uint8_t countMinusOne;
        inInputStream.read(countMinusOne);
        mCount = countMinusOne + 1;
    }
    else
    {
        //default!
        mCount = 1;
    }
}
