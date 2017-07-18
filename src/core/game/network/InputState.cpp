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

#include "FrameworkConstants.h"

#include <assert.h>

InputState::InputState() : IInputState(), m_iMenuState(MENU_STATE_NONE)
{
    // Empty
}

InputState::~InputState()
{
    // Empty
}

bool InputState::write(OutputMemoryBitStream& inOutputStream) const
{
    bool ret = false;
    
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (m_gameInputStates[i].m_iPlayerId != INPUT_UNASSIGNED)
        {
            inOutputStream.write((bool)true);
            m_gameInputStates[i].write(inOutputStream);
            
            ret = true;
        }
        else
        {
            inOutputStream.write((bool)false);
        }
    }
    
    return ret;
}

bool InputState::read(InputMemoryBitStream& inInputStream)
{
    bool ret = false;
    
    bool isInputAssignedBit;
    
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        inInputStream.read(isInputAssignedBit);
        if (isInputAssignedBit)
        {
            m_gameInputStates[i].read(inInputStream);
            
            ret = true;
        }
    }
    
    return ret;
}

void InputState::reset()
{
    for (GameInputState& gis : m_gameInputStates)
    {
        gis.m_iPlayerId = INPUT_UNASSIGNED;
        gis.m_isMovingRight = false;
        gis.m_isMovingLeft = false;
        gis.m_isJumping = false;
        gis.m_isShooting = false;
        gis.m_isSprinting = false;
    }
}

void InputState::copyTo(InputState* inInputState)
{
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        inInputState->m_gameInputStates[i].m_iPlayerId = m_gameInputStates[i].m_iPlayerId;
        inInputState->m_gameInputStates[i].m_isMovingRight = m_gameInputStates[i].m_isMovingRight;
        inInputState->m_gameInputStates[i].m_isMovingLeft = m_gameInputStates[i].m_isMovingLeft;
        inInputState->m_gameInputStates[i].m_isJumping = m_gameInputStates[i].m_isJumping;
        inInputState->m_gameInputStates[i].m_isShooting = m_gameInputStates[i].m_isShooting;
        inInputState->m_gameInputStates[i].m_isSprinting = m_gameInputStates[i].m_isSprinting;
    }
}

void InputState::activateNextPlayer(uint8_t playerId)
{
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (m_gameInputStates[i].m_iPlayerId == INPUT_UNASSIGNED)
        {
            m_gameInputStates[i].m_iPlayerId = playerId;
            return;
        }
    }
}

InputState::GameInputState* InputState::getGameInputStateForPlayerId(uint8_t playerId)
{
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (m_gameInputStates[i].m_iPlayerId == playerId)
        {
            return &m_gameInputStates[i];
        }
    }
    
    return nullptr;
}

bool InputState::isRequestingToAddLocalPlayer() const
{
    for (int i = 1; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (m_gameInputStates[i].m_iPlayerId == INPUT_UNASSIGNED
            && m_gameInputStates[i].m_isJumping)
        {
            return true;
        }
    }
    
    return false;
}

int InputState::getMenuState() const
{
    return m_iMenuState;
}

InputState::GameInputState& InputState::getGameInputState(int index)
{
    return m_gameInputStates[index];
}

void InputState::GameInputState::write(OutputMemoryBitStream& inOutputStream) const
{
    inOutputStream.write(m_iPlayerId, 3);
    inOutputStream.write(m_isMovingRight);
    inOutputStream.write(m_isMovingLeft);
    inOutputStream.write(m_isJumping);
    inOutputStream.write(m_isShooting);
    inOutputStream.write(m_isSprinting);
}

void InputState::GameInputState::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.read(m_iPlayerId, 3);
    inInputStream.read(m_isMovingRight);
    inInputStream.read(m_isMovingLeft);
    inInputStream.read(m_isJumping);
    inInputStream.read(m_isShooting);
    inInputStream.read(m_isSprinting);
}

float InputState::GameInputState::getDesiredRightAmount()
{
    int ret = 0;
    
    if (m_isMovingRight)
    {
        ++ret;
    }
    
    if (m_isMovingLeft)
    {
        --ret;
    }
    
    return ret;
}

bool InputState::GameInputState::isJumping()
{
    return m_isJumping;
}

bool InputState::GameInputState::isShooting()
{
    return m_isShooting;
}

bool InputState::GameInputState::isSprinting()
{
    return m_isSprinting;
}

InputState::GameInputState::GameInputState() :
m_iPlayerId(INPUT_UNASSIGNED),
m_isMovingRight(false),
m_isMovingLeft(false),
m_isJumping(false),
m_isShooting(false),
m_isSprinting(false)
{
    // Empty
}

RTTI_IMPL(InputState, IInputState);
