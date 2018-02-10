//
//  GameInputManager.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/game/GameInputManager.h>

#include <game/game/GameInputState.h>
#include <framework/network/portable/Move.h>
#include <game/game/GameEngine.h>

#include <framework/util/Timing.h>
#include <framework/input/CursorInputManager.h>
#include <framework/input/CursorEvent.h>
#include <framework/input/CursorConverter.h>
#include <framework/input/KeyboardInputManager.h>
#include <framework/input/KeyboardEvent.h>
#include <framework/input/GamePadInputManager.h>
#include <framework/input/GamePadEvent.h>
#include <framework/util/Constants.h>
#include <framework/input/KeyboardLookup.h>
#include <framework/util/StringUtil.h>
#include <framework/util/MathUtil.h>
#include <framework/util/Constants.h>
#include <framework/util/PlatformHelper.h>
#include <game/logic/GameConfig.h>

#include <sstream>

GameInputManager* GameInputManager::s_instance = NULL;

void GameInputManager::create()
{
    assert(!s_instance);
    
    s_instance = new GameInputManager();
}

GameInputManager * GameInputManager::getInstance()
{
    return s_instance;
}

void GameInputManager::destroy()
{
    assert(s_instance);
    
    delete s_instance;
    s_instance = NULL;
}

void GameInputManager::sRemoveProcessedMoves(float inLastMoveProcessedOnServerTimestamp)
{
    getInstance()->getMoveList().removeProcessedMoves(inLastMoveProcessedOnServerTimestamp, GameInputManager::sHandleInputStateRelease);
}

MoveList& GameInputManager::sGetMoveList()
{
    return getInstance()->getMoveList();
}

void GameInputManager::sOnPlayerWelcomed(uint8_t playerId)
{
    getInstance()->_currentState->activateNextPlayer(playerId);
}

void GameInputManager::sHandleInputStateRelease(InputState* inputState)
{
    GameInputState* mainInputState = static_cast<GameInputState*>(inputState);
    getInstance()->_inputStates.free(mainInputState);
}

void GameInputManager::setEngine(GameEngine* inValue)
{
    _engine = inValue;
}

void GameInputManager::update()
{
    CURSOR_INPUT_MANAGER->process();
    KEYBOARD_INPUT_MANAGER->process();
    GAME_PAD_INPUT_MANAGER->process();
    
    _inputState = GIMS_NONE;
    
    for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
    {
        KeyboardEvent& e = *(*i);
        switch (e.getKey())
        {
            case NG_KEY_M:
                _inputState = e.isDown() ? GIMS_CLIENT_MAIN_TOGGLE_MUSIC : _inputState;
                continue;
            case NG_KEY_S:
                _inputState = e.isDown() ? GIMS_CLIENT_MAIN_TOGGLE_SOUND : _inputState;
                continue;
            case NG_KEY_B:
                _inputState = e.isDown() ? GIMS_TOGGLE_PHYSICS_DISPLAY : _inputState;
                continue;
            case NG_KEY_I:
                _inputState = e.isDown() ? GIMS_TOGGLE_INTERPOLATION : _inputState;
                continue;
            case NG_KEY_L:
                _inputState = e.isDown() ? GIMS_TOGGLE_LIGHTING : _inputState;
                continue;
            case NG_KEY_T:
                _inputState = e.isDown() ? GIMS_SERVER_TOGGLE_MAP : _inputState;
                continue;
            case NG_KEY_U:
                _engine->_state ^= e.isDown() ? GameEngineState_DisplayUI : 0;
                continue;
            case NG_KEY_ARROW_UP:
                _playerLightZDelta = e.isPressed() ? 0.001f : 0;
                continue;
            case NG_KEY_ARROW_DOWN:
                _playerLightZDelta = e.isPressed() ? -0.001f : 0;
                continue;
            case NG_KEY_ESCAPE:
                if (e.isDown())
                {
                    dropPlayer(0);
                }
                continue;
            default:
            {
                switch (e.getKey())
                {
                    // Player 1
                    case NG_KEY_W:
                        _currentState->getPlayerInputState(0)._isJumping = e.isPressed();
                        continue;
                    case NG_KEY_A:
                        _currentState->getPlayerInputState(0)._isMovingLeft = e.isPressed();
                        continue;
                    case NG_KEY_D:
                        _currentState->getPlayerInputState(0)._isMovingRight = e.isPressed();
                        continue;
                    case NG_KEY_SPACE_BAR:
                        _currentState->getPlayerInputState(0)._isMainAction = e.isPressed();
                        continue;
#ifdef _DEBUG
                    // Add local players, debug Only
                    case NG_KEY_TWO:
                        _currentState->getPlayerInputState(1)._isJumping = e.isPressed();
                        continue;
                    case NG_KEY_THREE:
                        _currentState->getPlayerInputState(2)._isJumping = e.isPressed();
                        continue;
                    case NG_KEY_FOUR:
                        _currentState->getPlayerInputState(3)._isJumping = e.isPressed();
                        continue;
                    case NG_KEY_ARROW_LEFT:
                        _currentState->getPlayerInputState(1)._isMovingLeft = e.isPressed();
                        _currentState->getPlayerInputState(2)._isMovingLeft = e.isPressed();
                        _currentState->getPlayerInputState(3)._isMovingLeft = e.isPressed();
                        continue;
                    case NG_KEY_ARROW_RIGHT:
                        _currentState->getPlayerInputState(1)._isMovingRight = e.isPressed();
                        _currentState->getPlayerInputState(2)._isMovingRight = e.isPressed();
                        _currentState->getPlayerInputState(3)._isMovingRight = e.isPressed();
                        continue;
                    case NG_KEY_PERIOD:
                        _currentState->getPlayerInputState(1)._isMainAction = e.isPressed();
                        _currentState->getPlayerInputState(2)._isMainAction = e.isPressed();
                        _currentState->getPlayerInputState(3)._isMainAction = e.isPressed();
                        continue;
                    case NG_KEY_SEVEN:
                        if (e.isDown())
                        {
                            dropPlayer(1);
                        }
                        continue;
                    case NG_KEY_EIGHT:
                        if (e.isDown())
                        {
                            dropPlayer(2);
                        }
                        continue;
                    case NG_KEY_NINE:
                        if (e.isDown())
                        {
                            dropPlayer(3);
                        }
                        continue;
#endif
                    default:
                        continue;
                }
            }
        }
    }
    
    bool isMovingRight[4] = {false};
    bool isMovingLeft[4] = {false};
    bool isAction[4] = {false};
    
    for (std::vector<GamePadEvent *>::iterator i = GAME_PAD_INPUT_MANAGER->getEvents().begin(); i != GAME_PAD_INPUT_MANAGER->getEvents().end(); ++i)
    {
        switch ((*i)->getType())
        {
            case GamePadEventType_A_BUTTON:
                _currentState->getPlayerInputState((*i)->getIndex())._isJumping = (*i)->isPressed();
                continue;
            case GamePadEventType_D_PAD_RIGHT:
            {
                if (!isMovingRight[(*i)->getIndex()])
                {
                    _currentState->getPlayerInputState((*i)->getIndex())._isMovingRight = (*i)->isPressed();
                    isMovingRight[(*i)->getIndex()] = _currentState->getPlayerInputState((*i)->getIndex())._isMovingRight;
                }
            }
                continue;
            case GamePadEventType_D_PAD_LEFT:
            {
                if (!isMovingLeft[(*i)->getIndex()])
                {
                    _currentState->getPlayerInputState((*i)->getIndex())._isMovingLeft = (*i)->isPressed();
                    isMovingLeft[(*i)->getIndex()] = _currentState->getPlayerInputState((*i)->getIndex())._isMovingLeft;
                }
            }
                continue;
            case GamePadEventType_STICK_LEFT:
            {
                float val = sanitizeCloseToZeroValue((*i)->getX());
                if (!isMovingRight[(*i)->getIndex()])
                {
                    _currentState->getPlayerInputState((*i)->getIndex())._isMovingRight = val > 0;
                    isMovingRight[(*i)->getIndex()] = _currentState->getPlayerInputState((*i)->getIndex())._isMovingRight;
                }
                
                if (!isMovingLeft[(*i)->getIndex()])
                {
                    _currentState->getPlayerInputState((*i)->getIndex())._isMovingLeft = val < 0;
                    isMovingLeft[(*i)->getIndex()] = _currentState->getPlayerInputState((*i)->getIndex())._isMovingLeft;
                }
            }
                continue;
            case GamePadEventType_X_BUTTON:
            case GamePadEventType_TRIGGER:
                if (!isAction[(*i)->getIndex()])
                {
                    _currentState->getPlayerInputState((*i)->getIndex())._isMainAction = (*i)->getX() > 0 || (*i)->getY() > 0;
                    isAction[(*i)->getIndex()] = _currentState->getPlayerInputState((*i)->getIndex())._isMainAction;
                }
                continue;
            case GamePadEventType_BACK_BUTTON:
                if ((*i)->isPressed())
                {
                    dropPlayer((*i)->getIndex());
                }
                continue;
            default:
                continue;
        }
    }
    
    if (PlatformHelper::getPlatform() == NG_PLATFORM_ANDROID
        || PlatformHelper::getPlatform() == NG_PLATFORM_IOS)
    {
        for (std::vector<CursorEvent *>::iterator i = CURSOR_INPUT_MANAGER->getEvents().begin(); i != CURSOR_INPUT_MANAGER->getEvents().end(); ++i)
        {
            if ((*i)->getType() == CursorEventType_DOWN
                || (*i)->getType() == CursorEventType_DRAGGED)
            {
                Vector2& vec = CURSOR_CONVERTER->convert(*(*i));
                _currentState->getPlayerInputState(0)._isMovingLeft = vec.getX() < (GM_CFG->_camWidth / 2);
                _currentState->getPlayerInputState(0)._isMovingRight = vec.getX() > (GM_CFG->_camWidth / 2);
                
                if ((*i)->getType() == CursorEventType_DOWN)
                {
                    _currentState->getPlayerInputState(0)._isJumping = true;
                }
            }
            else
            {
                _currentState->getPlayerInputState(0)._isMovingLeft = false;
                _currentState->getPlayerInputState(0)._isMovingRight = false;
                _currentState->getPlayerInputState(0)._isJumping = false;
                
                continue;
            }
        }
    }
    
    GM_CFG->_playerLightZ = clamp(GM_CFG->_playerLightZ + _playerLightZDelta, 0.3f, -0.1f);
    _pendingMove = &sampleInputAsMove();
}

const Move* GameInputManager::getPendingMove()
{
    return _pendingMove;
}

void GameInputManager::clearPendingMove()
{
    _pendingMove = NULL;
}

GameInputState* GameInputManager::getInputState()
{
    return _currentState;
}

MoveList& GameInputManager::getMoveList()
{
    return _moveList;
}

int GameInputManager::getMenuState()
{
    return _inputState;
}

const Move& GameInputManager::sampleInputAsMove()
{
    GameInputState* inputState = _inputStates.obtain();
    _currentState->copyTo(inputState);
    
    return _moveList.addMove(inputState, Timing::getInstance()->getFrameStartTime());
}

void GameInputManager::dropPlayer(int index)
{
    _currentState->getPlayerInputState(index)._playerId = INPUT_UNASSIGNED;
    _inputState = GIMS_LOCAL_PLAYER_DROP_OUT_0 + index;
}

GameInputManager::GameInputManager() :
_currentState(_inputStates.obtain()),
_pendingMove(NULL),
_inputState(GIMS_NONE),
_isTimeToProcessInput(false),
_engine(NULL),
_playerLightZDelta(0)
{
    // Empty
}
