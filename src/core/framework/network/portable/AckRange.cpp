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

AckRange::AckRange() : _start(0), _count(0)
{
    // Empty
}

AckRange::AckRange(uint16_t inStart) : _start(inStart), _count(1)
{
    // Empty
}

void AckRange::write(OutputMemoryBitStream& inOutputStream) const
{
    inOutputStream.write(_start);
    bool hasCount = _count > 1;
    inOutputStream.write(hasCount);
    if (hasCount)
    {
        //most you can ack is 255...
        uint32_t countMinusOne = _count - 1;
        uint8_t countToAck = countMinusOne > 255 ? 255 : static_cast<uint8_t>(countMinusOne);
        inOutputStream.write(countToAck);
    }
}

void AckRange::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.read(_start);
    bool hasCount;
    inInputStream.read(hasCount);
    if (hasCount)
    {
        uint8_t countMinusOne;
        inInputStream.read(countMinusOne);
        _count = countMinusOne + 1;
    }
    else
    {
        //default!
        _count = 1;
    }
}

bool AckRange::extendIfShould(uint16_t inSequenceNumber)
{
    if (inSequenceNumber == _start + _count)
    {
        ++_count;
        return true;
    }
    else
    {
        return false;
    }
}

uint16_t AckRange::getStart() const
{
    return _start;
}

uint32_t AckRange::getCount() const
{
    return _count;
}
