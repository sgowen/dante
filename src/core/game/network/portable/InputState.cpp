//
//  InputState.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "InputState.h"

#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"

namespace
{
    void writeSignedBinaryValue(OutputMemoryBitStream& inOutputStream, float inValue)
    {
        bool isNonZero = (inValue != 0.f);
        inOutputStream.write(isNonZero);
        if (isNonZero)
        {
            inOutputStream.write(inValue > 0.f);
        }
    }
    
    void readSignedBinaryValue(InputMemoryBitStream& inInputStream, float& outValue)
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
}

InputState::InputState() : m_fDesiredRightAmount(0), m_fDesiredLeftAmount(0), m_fDesiredForwardAmount(0), m_fDesiredBackAmount(0)
{
    // Empty
}

bool InputState::write(OutputMemoryBitStream& inOutputStream) const
{
    writeSignedBinaryValue(inOutputStream, getDesiredHorizontalDelta());
    writeSignedBinaryValue(inOutputStream, getDesiredVerticalDelta());
    
    return false;
}

bool InputState::read(InputMemoryBitStream& inInputStream)
{
    readSignedBinaryValue(inInputStream, m_fDesiredRightAmount);
    readSignedBinaryValue(inInputStream, m_fDesiredForwardAmount);
    
    return true;
}

float InputState::getDesiredHorizontalDelta() const
{
    return m_fDesiredRightAmount - m_fDesiredLeftAmount;
}

float InputState::getDesiredVerticalDelta() const
{
    return m_fDesiredForwardAmount - m_fDesiredBackAmount;
}
