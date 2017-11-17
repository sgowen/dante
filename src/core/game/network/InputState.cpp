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

InputState::InputState() : IInputState(), _menuState(MENU_STATE_NONE)
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
        if (_gameInputStates[i]._playerId != INPUT_UNASSIGNED)
        {
            inOutputStream.write((bool)true);
            _gameInputStates[i].write(inOutputStream);
            
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
            _gameInputStates[i].read(inInputStream);
            
            ret = true;
        }
    }
    
    return ret;
}

void InputState::reset()
{
    for (GameInputState& gis : _gameInputStates)
    {
        gis._playerId = INPUT_UNASSIGNED;
        gis._isMovingRight = false;
        gis._isMovingLeft = false;
        gis._isJumping = false;
        gis._isShooting = false;
        gis._isSprinting = false;
    }
}

bool InputState::isEqual(IInputState* inIInputState) const
{
    InputState* in = static_cast<InputState*>(inIInputState);
    
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (in->_gameInputStates[i]._playerId != _gameInputStates[i]._playerId) { return false; }
        if (in->_gameInputStates[i]._isMovingRight != _gameInputStates[i]._isMovingRight) { return false; }
        if (in->_gameInputStates[i]._isMovingLeft != _gameInputStates[i]._isMovingLeft) { return false; }
        if (in->_gameInputStates[i]._isJumping != _gameInputStates[i]._isJumping) { return false; }
        if (in->_gameInputStates[i]._isShooting != _gameInputStates[i]._isShooting) { return false; }
        if (in->_gameInputStates[i]._isSprinting != _gameInputStates[i]._isSprinting) { return false; }
    }
    
    return true;
}

void InputState::copyTo(IInputState* inIInputState) const
{
    InputState* inInputState = static_cast<InputState*>(inIInputState);
    
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        inInputState->_gameInputStates[i]._playerId = _gameInputStates[i]._playerId;
        inInputState->_gameInputStates[i]._isMovingRight = _gameInputStates[i]._isMovingRight;
        inInputState->_gameInputStates[i]._isMovingLeft = _gameInputStates[i]._isMovingLeft;
        inInputState->_gameInputStates[i]._isJumping = _gameInputStates[i]._isJumping;
        inInputState->_gameInputStates[i]._isShooting = _gameInputStates[i]._isShooting;
        inInputState->_gameInputStates[i]._isSprinting = _gameInputStates[i]._isSprinting;
    }
}

void InputState::activateNextPlayer(uint8_t playerId)
{
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (_gameInputStates[i]._playerId == INPUT_UNASSIGNED)
        {
            _gameInputStates[i]._playerId = playerId;
            return;
        }
    }
}

InputState::GameInputState* InputState::getGameInputStateForPlayerId(uint8_t playerId)
{
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (_gameInputStates[i]._playerId == playerId)
        {
            return &_gameInputStates[i];
        }
    }
    
    return nullptr;
}

bool InputState::isPlayerIdLocalHost(uint8_t playerId)
{
    return _gameInputStates[0]._playerId == playerId;
}

bool InputState::isRequestingToAddLocalPlayer() const
{
    for (int i = 1; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (_gameInputStates[i]._playerId == INPUT_UNASSIGNED
            && _gameInputStates[i]._isJumping)
        {
            return true;
        }
    }
    
    return false;
}

int InputState::getMenuState() const
{
    return _menuState;
}

InputState::GameInputState& InputState::getGameInputState(int index)
{
    return _gameInputStates[index];
}

void InputState::GameInputState::write(OutputMemoryBitStream& inOutputStream) const
{
    inOutputStream.write<uint8_t, 3>(_playerId);
    inOutputStream.write(_isMovingRight);
    inOutputStream.write(_isMovingLeft);
    inOutputStream.write(_isJumping);
    inOutputStream.write(_isShooting);
    inOutputStream.write(_isSprinting);
}

void InputState::GameInputState::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.read<uint8_t, 3>(_playerId);
    inInputStream.read(_isMovingRight);
    inInputStream.read(_isMovingLeft);
    inInputStream.read(_isJumping);
    inInputStream.read(_isShooting);
    inInputStream.read(_isSprinting);
}

int InputState::GameInputState::getDesiredRightAmount()
{
    int ret = 0;
    
    if (_isMovingRight)
    {
        ++ret;
    }
    
    if (_isMovingLeft)
    {
        --ret;
    }
    
    return ret;
}

bool InputState::GameInputState::isJumping()
{
    return _isJumping;
}

bool InputState::GameInputState::isShooting()
{
    return _isShooting;
}

bool InputState::GameInputState::isSprinting()
{
    return _isSprinting;
}

InputState::GameInputState::GameInputState() :
_playerId(INPUT_UNASSIGNED),
_isMovingRight(false),
_isMovingLeft(false),
_isJumping(false),
_isShooting(false),
_isSprinting(false)
{
    // Empty
}

RTTI_IMPL(InputState, IInputState);
