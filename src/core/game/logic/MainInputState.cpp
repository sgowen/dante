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

MainInputState::MainInputState() : InputState()
{
    // Empty
}

void MainInputState::write(OutputMemoryBitStream& inOutputStream) const
{
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        bool playerId = _gameMainInputStates[i]._playerId != INPUT_UNASSIGNED;
        inOutputStream.write(playerId);
        if (playerId)
        {
            _gameMainInputStates[i].write(inOutputStream);
        }
    }
}

void MainInputState::read(InputMemoryBitStream& inInputStream)
{
    bool isInputAssignedBit;
    
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        inInputStream.read(isInputAssignedBit);
        if (isInputAssignedBit)
        {
            _gameMainInputStates[i].read(inInputStream);
        }
    }
}

void MainInputState::reset()
{
    for (GameInputState& gis : _gameMainInputStates)
    {
        gis._playerId = INPUT_UNASSIGNED;
        gis._isMovingRight = false;
        gis._isMovingLeft = false;
        gis._isJumping = false;
        gis._isMainAction = false;
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
        if (in->_gameMainInputStates[i]._isMainAction != _gameMainInputStates[i]._isMainAction) { return false; }
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
        inMainInputState->_gameMainInputStates[i]._isMainAction = _gameMainInputStates[i]._isMainAction;
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
    inOutputStream.write(_isMainAction);
}

void MainInputState::GameInputState::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.read<uint8_t, 3>(_playerId);
    inInputStream.read(_isMovingRight);
    inInputStream.read(_isMovingLeft);
    inInputStream.read(_isJumping);
    inInputStream.read(_isMainAction);
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

bool MainInputState::GameInputState::isMainAction()
{
    return _isMainAction;
}

MainInputState::GameInputState::GameInputState() :
_playerId(INPUT_UNASSIGNED),
_isMovingRight(false),
_isMovingLeft(false),
_isJumping(false),
_isMainAction(false)
{
    // Empty
}
