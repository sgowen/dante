//
//  GameInputState.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/game/GameInputState.h>

#include <framework/network/portable/OutputMemoryBitStream.h>
#include <framework/network/portable/InputMemoryBitStream.h>

#include <framework/util/Constants.h>

#include <assert.h>

NGRTTI_IMPL(GameInputState, InputState);

GameInputState::GameInputState() : InputState()
{
    // Empty
}

void GameInputState::write(OutputMemoryBitStream& inOutputStream) const
{
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        bool playerId = _playerInputStates[i]._playerId != INPUT_UNASSIGNED;
        inOutputStream.write(playerId);
        if (playerId)
        {
            _playerInputStates[i].write(inOutputStream);
        }
    }
}

void GameInputState::read(InputMemoryBitStream& inInputStream)
{
    bool isInputAssignedBit;
    
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        inInputStream.read(isInputAssignedBit);
        if (isInputAssignedBit)
        {
            _playerInputStates[i].read(inInputStream);
        }
    }
}

void GameInputState::reset()
{
    for (PlayerInputState& is : _playerInputStates)
    {
        is._playerId = INPUT_UNASSIGNED;
        is._isMovingRight = false;
        is._isMovingLeft = false;
        is._isJumping = false;
        is._isMainAction = false;
    }
}

bool GameInputState::isEqual(InputState* inInputState) const
{
    GameInputState* in = static_cast<GameInputState*>(inInputState);
    
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (in->_playerInputStates[i]._playerId != _playerInputStates[i]._playerId) { return false; }
        if (in->_playerInputStates[i]._isMovingRight != _playerInputStates[i]._isMovingRight) { return false; }
        if (in->_playerInputStates[i]._isMovingLeft != _playerInputStates[i]._isMovingLeft) { return false; }
        if (in->_playerInputStates[i]._isJumping != _playerInputStates[i]._isJumping) { return false; }
        if (in->_playerInputStates[i]._isMainAction != _playerInputStates[i]._isMainAction) { return false; }
    }
    
    return true;
}

void GameInputState::copyTo(InputState* inInputState) const
{
    GameInputState* inGameInputState = static_cast<GameInputState*>(inInputState);
    
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        inGameInputState->_playerInputStates[i]._playerId = _playerInputStates[i]._playerId;
        inGameInputState->_playerInputStates[i]._isMovingRight = _playerInputStates[i]._isMovingRight;
        inGameInputState->_playerInputStates[i]._isMovingLeft = _playerInputStates[i]._isMovingLeft;
        inGameInputState->_playerInputStates[i]._isJumping = _playerInputStates[i]._isJumping;
        inGameInputState->_playerInputStates[i]._isMainAction = _playerInputStates[i]._isMainAction;
    }
}

void GameInputState::activateNextPlayer(uint8_t playerId)
{
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (_playerInputStates[i]._playerId == INPUT_UNASSIGNED)
        {
            _playerInputStates[i]._playerId = playerId;
            return;
        }
    }
}

GameInputState::PlayerInputState* GameInputState::getPlayerInputStateForID(uint8_t playerId)
{
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (_playerInputStates[i]._playerId == playerId)
        {
            return &_playerInputStates[i];
        }
    }
    
    return NULL;
}

bool GameInputState::isRequestingToAddLocalPlayer() const
{
    for (int i = 1; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (_playerInputStates[i]._playerId == INPUT_UNASSIGNED
            && _playerInputStates[i]._isJumping)
        {
            return true;
        }
    }
    
    return false;
}

GameInputState::PlayerInputState& GameInputState::getPlayerInputState(int index)
{
    return _playerInputStates[index];
}

void GameInputState::PlayerInputState::write(OutputMemoryBitStream& inOutputStream) const
{
    inOutputStream.write<uint8_t, 3>(_playerId);
    inOutputStream.write(_isMovingRight);
    inOutputStream.write(_isMovingLeft);
    inOutputStream.write(_isJumping);
    inOutputStream.write(_isMainAction);
}

void GameInputState::PlayerInputState::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.read<uint8_t, 3>(_playerId);
    inInputStream.read(_isMovingRight);
    inInputStream.read(_isMovingLeft);
    inInputStream.read(_isJumping);
    inInputStream.read(_isMainAction);
}

int GameInputState::PlayerInputState::getDesiredRightAmount()
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

bool GameInputState::PlayerInputState::isJumping()
{
    return _isJumping;
}

bool GameInputState::PlayerInputState::isMainAction()
{
    return _isMainAction;
}

GameInputState::PlayerInputState::PlayerInputState() :
_playerId(INPUT_UNASSIGNED),
_isMovingRight(false),
_isMovingLeft(false),
_isJumping(false),
_isMainAction(false)
{
    // Empty
}
