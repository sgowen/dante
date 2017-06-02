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

InputState::InputState() : IInputState(),
m_fDesiredRightAmount(0),
m_fDesiredLeftAmount(0),
m_fDesiredForwardAmount(0),
m_fDesiredBackAmount(0)
{
    // Empty
}

InputState::~InputState()
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

void InputState::copyTo(InputState* inInputState)
{
    inInputState->m_fDesiredRightAmount = m_fDesiredRightAmount;
    inInputState->m_fDesiredLeftAmount = m_fDesiredLeftAmount;
    inInputState->m_fDesiredForwardAmount = m_fDesiredForwardAmount;
    inInputState->m_fDesiredBackAmount = m_fDesiredBackAmount;
}

float InputState::getDesiredHorizontalDelta() const
{
    return m_fDesiredRightAmount - m_fDesiredLeftAmount;
}

float InputState::getDesiredVerticalDelta() const
{
    return m_fDesiredForwardAmount - m_fDesiredBackAmount;
}

RTTI_IMPL(InputState, IInputState);
