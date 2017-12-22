//
//  MainInputState.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/MainInputState.h"

#include "framework/network/portable/OutputMemoryBitStream.h"
#include "framework/network/portable/InputMemoryBitStream.h"

#include "framework/util/FrameworkConstants.h"

#include <assert.h>

NGRTTI_IMPL(MainInputState, InputState);

MainInputState::MainInputState() : InputState(), _menuState(MENU_STATE_NONE)
{
    // Empty
}

bool MainInputState::write(OutputMemoryBitStream& inOutputStream) const
{
    bool ret = false;
    
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (_gameMainInputStates[i]._playerId != INPUT_UNASSIGNED)
        {
            inOutputStream.write(true);
            _gameMainInputStates[i].write(inOutputStream);
            
            ret = true;
        }
        else
        {
            inOutputStream.write(false);
        }
    }
    
    return ret;
}

bool MainInputState::read(InputMemoryBitStream& inInputStream)
{
    bool ret = false;
    
    bool isInputAssignedBit;
    
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        inInputStream.read(isInputAssignedBit);
        if (isInputAssignedBit)
        {
            _gameMainInputStates[i].read(inInputStream);
            
            ret = true;
        }
    }
    
    return ret;
}

void MainInputState::reset()
{
    for (GameInputState& gis : _gameMainInputStates)
    {
        gis._playerId = INPUT_UNASSIGNED;
        gis._isMovingRight = false;
        gis._isMovingLeft = false;
        gis._isJumping = false;
        gis._isShooting = false;
        gis._isSprinting = false;
    }
}

bool MainInputState::isEqual(InputState* inInputState) const
{
    MainInputState* in = static_cast<MainInputState*>(inInputState);
    
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (in->_gameMainInputStates[i]._playerId != _gameMainInputStates[i]._playerId) { return false; }
        if (in->_gameMainInputStates[i]._isMovingRight != _gameMainInputStates[i]._isMovingRight) { return false; }
        if (in->_gameMainInputStates[i]._isMovingLeft != _gameMainInputStates[i]._isMovingLeft) { return false; }
        if (in->_gameMainInputStates[i]._isJumping != _gameMainInputStates[i]._isJumping) { return false; }
        if (in->_gameMainInputStates[i]._isShooting != _gameMainInputStates[i]._isShooting) { return false; }
        if (in->_gameMainInputStates[i]._isSprinting != _gameMainInputStates[i]._isSprinting) { return false; }
    }
    
    return true;
}

void MainInputState::copyTo(InputState* inInputState) const
{
    MainInputState* inMainInputState = static_cast<MainInputState*>(inInputState);
    
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        inMainInputState->_gameMainInputStates[i]._playerId = _gameMainInputStates[i]._playerId;
        inMainInputState->_gameMainInputStates[i]._isMovingRight = _gameMainInputStates[i]._isMovingRight;
        inMainInputState->_gameMainInputStates[i]._isMovingLeft = _gameMainInputStates[i]._isMovingLeft;
        inMainInputState->_gameMainInputStates[i]._isJumping = _gameMainInputStates[i]._isJumping;
        inMainInputState->_gameMainInputStates[i]._isShooting = _gameMainInputStates[i]._isShooting;
        inMainInputState->_gameMainInputStates[i]._isSprinting = _gameMainInputStates[i]._isSprinting;
    }
}

void MainInputState::activateNextPlayer(uint8_t playerId)
{
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (_gameMainInputStates[i]._playerId == INPUT_UNASSIGNED)
        {
            _gameMainInputStates[i]._playerId = playerId;
            return;
        }
    }
}

MainInputState::GameInputState* MainInputState::getGameInputStateForPlayerId(uint8_t playerId)
{
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (_gameMainInputStates[i]._playerId == playerId)
        {
            return &_gameMainInputStates[i];
        }
    }
    
    return NULL;
}

bool MainInputState::isPlayerIdLocalHost(uint8_t playerId)
{
    return _gameMainInputStates[0]._playerId == playerId;
}

bool MainInputState::isRequestingToAddLocalPlayer() const
{
    for (int i = 1; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (_gameMainInputStates[i]._playerId == INPUT_UNASSIGNED
            && _gameMainInputStates[i]._isJumping)
        {
            return true;
        }
    }
    
    return false;
}

int MainInputState::getMenuState() const
{
    return _menuState;
}

MainInputState::GameInputState& MainInputState::getGameInputState(int index)
{
    return _gameMainInputStates[index];
}

void MainInputState::GameInputState::write(OutputMemoryBitStream& inOutputStream) const
{
    inOutputStream.write<uint8_t, 3>(_playerId);
    inOutputStream.write(_isMovingRight);
    inOutputStream.write(_isMovingLeft);
    inOutputStream.write(_isJumping);
    inOutputStream.write(_isShooting);
    inOutputStream.write(_isSprinting);
}

void MainInputState::GameInputState::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.read<uint8_t, 3>(_playerId);
    inInputStream.read(_isMovingRight);
    inInputStream.read(_isMovingLeft);
    inInputStream.read(_isJumping);
    inInputStream.read(_isShooting);
    inInputStream.read(_isSprinting);
}

int MainInputState::GameInputState::getDesiredRightAmount()
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

bool MainInputState::GameInputState::isJumping()
{
    return _isJumping;
}

bool MainInputState::GameInputState::isShooting()
{
    return _isShooting;
}

bool MainInputState::GameInputState::isSprinting()
{
    return _isSprinting;
}

MainInputState::GameInputState::GameInputState() :
_playerId(INPUT_UNASSIGNED),
_isMovingRight(false),
_isMovingLeft(false),
_isJumping(false),
_isShooting(false),
_isSprinting(false)
{
    // Empty
}
