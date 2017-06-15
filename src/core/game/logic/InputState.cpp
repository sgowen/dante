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
m_fDesiredJumpIntensity(0),
m_isShooting(false),
m_isSprinting(false),
m_isStartingServer(false),
m_isJoiningServer(false)
{
    // Empty
}

InputState::~InputState()
{
    // Empty
}

bool InputState::write(OutputMemoryBitStream& inOutputStream) const
{
    inOutputStream.writeSignedBinaryValue(getDesiredHorizontalDelta());
    inOutputStream.writeSignedBinaryValue(m_fDesiredJumpIntensity);
    inOutputStream.write(m_isShooting);
    inOutputStream.write(m_isSprinting);
    
    return false;
}

bool InputState::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.readSignedBinaryValue(m_fDesiredRightAmount);
    inInputStream.readSignedBinaryValue(m_fDesiredJumpIntensity);
    inInputStream.read(m_isShooting);
    inInputStream.read(m_isSprinting);
    
    return true;
}

void InputState::reset()
{
    m_fDesiredRightAmount = 0;
    m_fDesiredLeftAmount = 0;
    m_fDesiredJumpIntensity = 0;
    m_isShooting = false;
    m_isSprinting = false;
}

void InputState::copyTo(InputState* inInputState)
{
    inInputState->m_fDesiredRightAmount = m_fDesiredRightAmount;
    inInputState->m_fDesiredLeftAmount = m_fDesiredLeftAmount;
    inInputState->m_fDesiredJumpIntensity = m_fDesiredJumpIntensity;
    inInputState->m_isShooting = m_isShooting;
    inInputState->m_isSprinting = m_isSprinting;
    inInputState->m_isStartingServer = m_isStartingServer;
    inInputState->m_isJoiningServer = m_isJoiningServer;
}

float InputState::getDesiredHorizontalDelta() const
{
    return m_fDesiredRightAmount - m_fDesiredLeftAmount;
}

float InputState::getDesiredJumpIntensity() const
{
    return m_fDesiredJumpIntensity;
}

bool InputState::isShooting() const
{
    return m_isShooting;
}

bool InputState::isSprinting() const
{
    return m_isSprinting;
}

bool InputState::isStartingServer() const
{
    return m_isStartingServer;
}

bool InputState::isJoiningServer() const
{
    return m_isJoiningServer;
}

RTTI_IMPL(InputState, IInputState);
