//
//  IInputState.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "IInputState.h"

#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"

IInputState::IInputState()
{
    // Empty
}

IInputState::~IInputState()
{
    // Empty
}

void IInputState::writeSignedBinaryValue(OutputMemoryBitStream& inOutputStream, float inValue) const
{
    bool isNonZero = (inValue != 0.f);
    inOutputStream.write(isNonZero);
    if (isNonZero)
    {
        inOutputStream.write(inValue > 0.f);
    }
}

void IInputState::readSignedBinaryValue(InputMemoryBitStream& inInputStream, float& outValue) const
{
    bool isNonZero;
    inInputStream.read(isNonZero);
    if (isNonZero)
    {
        bool isPositive;
        inInputStream.read(isPositive);
        outValue = isPositive ? 1.f : -1.f;
    }
    else
    {
        outValue = 0.f;
    }
}

RTTI_IMPL_NOPARENT(IInputState);
