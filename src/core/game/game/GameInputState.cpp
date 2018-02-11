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
        is._inputState = 0;
    }
}

bool GameInputState::isEqual(InputState* inInputState) const
{
    GameInputState* in = static_cast<GameInputState*>(inInputState);
    
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        if (in->_playerInputStates[i]._playerId != _playerInputStates[i]._playerId)
        {
            return false;
        }
        
        if (in->_playerInputStates[i]._inputState != _playerInputStates[i]._inputState)
        {
            return false;
        }
    }
    
    return true;
}

void GameInputState::copyTo(InputState* inInputState) const
{
    GameInputState* inGameInputState = static_cast<GameInputState*>(inInputState);
    
    for (int i = 0; i < MAX_NUM_PLAYERS_PER_SERVER; ++i)
    {
        inGameInputState->_playerInputStates[i]._playerId = _playerInputStates[i]._playerId;
        inGameInputState->_playerInputStates[i]._inputState = _playerInputStates[i]._inputState;
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
            && _playerInputStates[i]._inputState > 0)
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
    inOutputStream.write<uint8_t, 5>(_inputState);
}

void GameInputState::PlayerInputState::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.read<uint8_t, 3>(_playerId);
    inInputStream.read<uint8_t, 5>(_inputState);
}

uint8_t GameInputState::PlayerInputState::getInputState()
{
    return _inputState;
}

GameInputState::PlayerInputState::PlayerInputState() :
_playerId(INPUT_UNASSIGNED),
_inputState(0)
{
    // Empty
}
