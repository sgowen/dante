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
#include <framework/audio/portable/NGAudioEngine.h>
#include <game/logic/Server.h>
#include <framework/network/client/NetworkManagerClient.h>
#include <framework/util/macros.h>

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
                if (e.isDown())
                {
                    NG_AUDIO_ENGINE->setMusicDisabled(!NG_AUDIO_ENGINE->isMusicDisabled());
                }
                continue;
            case NG_KEY_N:
                if (e.isDown())
                {
                    NG_AUDIO_ENGINE->setSoundsDisabled(!NG_AUDIO_ENGINE->areSoundsDisabled());
                }
                continue;
            case NG_KEY_B:
                _engine->_state ^= e.isDown() ? GameEngineState_DisplayBox2D : 0;
                continue;
            case NG_KEY_I:
                _engine->_state ^= e.isDown() ? GameEngineState_Interpolation : 0;
                continue;
            case NG_KEY_L:
                _engine->_state ^= e.isDown() ? GameEngineState_Lighting : 0;
                continue;
            case NG_KEY_T:
                if (e.isDown() && Server::getInstance())
                {
                    Server::getInstance()->toggleMap();
                }
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
                        SET_BIT(_currentState->getPlayerInputState(0)._inputState, GameInputStateFlag_Jumping, e.isPressed());
                        continue;
                    case NG_KEY_A:
                        SET_BIT(_currentState->getPlayerInputState(0)._inputState, GameInputStateFlag_MovingLeft, e.isPressed());
                        continue;
                    case NG_KEY_D:
                        SET_BIT(_currentState->getPlayerInputState(0)._inputState, GameInputStateFlag_MovingRight, e.isPressed());
                        continue;
                    case NG_KEY_SPACE_BAR:
                        SET_BIT(_currentState->getPlayerInputState(0)._inputState, GameInputStateFlag_MainAction, e.isPressed());
                        continue;
#ifdef _DEBUG
                    // Add local players, debug Only
                    case NG_KEY_TWO:
                        SET_BIT(_currentState->getPlayerInputState(1)._inputState, GameInputStateFlag_Jumping, e.isPressed());
                        continue;
                    case NG_KEY_THREE:
                        SET_BIT(_currentState->getPlayerInputState(2)._inputState, GameInputStateFlag_Jumping, e.isPressed());
                        continue;
                    case NG_KEY_FOUR:
                        SET_BIT(_currentState->getPlayerInputState(3)._inputState, GameInputStateFlag_Jumping, e.isPressed());
                        continue;
                    case NG_KEY_ARROW_LEFT:
                        SET_BIT(_currentState->getPlayerInputState(1)._inputState, GameInputStateFlag_MovingLeft, e.isPressed());
                        SET_BIT(_currentState->getPlayerInputState(2)._inputState, GameInputStateFlag_MovingLeft, e.isPressed());
                        SET_BIT(_currentState->getPlayerInputState(3)._inputState, GameInputStateFlag_MovingLeft, e.isPressed());
                        continue;
                    case NG_KEY_ARROW_RIGHT:
                        SET_BIT(_currentState->getPlayerInputState(1)._inputState, GameInputStateFlag_MovingRight, e.isPressed());
                        SET_BIT(_currentState->getPlayerInputState(2)._inputState, GameInputStateFlag_MovingRight, e.isPressed());
                        SET_BIT(_currentState->getPlayerInputState(3)._inputState, GameInputStateFlag_MovingRight, e.isPressed());
                        continue;
                    case NG_KEY_PERIOD:
                        SET_BIT(_currentState->getPlayerInputState(1)._inputState, GameInputStateFlag_MainAction, e.isPressed());
                        SET_BIT(_currentState->getPlayerInputState(2)._inputState, GameInputStateFlag_MainAction, e.isPressed());
                        SET_BIT(_currentState->getPlayerInputState(3)._inputState, GameInputStateFlag_MainAction, e.isPressed());
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
        GamePadEvent& e = *(*i);
        switch (e.getType())
        {
            case GamePadEventType_A_BUTTON:
                SET_BIT(_currentState->getPlayerInputState(e.getIndex())._inputState, GameInputStateFlag_Jumping, e.isPressed());
                continue;
            case GamePadEventType_D_PAD_RIGHT:
            {
                if (!isMovingRight[e.getIndex()])
                {
                    SET_BIT(_currentState->getPlayerInputState(e.getIndex())._inputState, GameInputStateFlag_MovingRight, e.isPressed());
                    isMovingRight[e.getIndex()] = _currentState->getPlayerInputState(e.getIndex())._inputState & GameInputStateFlag_MovingRight;
                }
            }
                continue;
            case GamePadEventType_D_PAD_LEFT:
            {
                if (!isMovingLeft[e.getIndex()])
                {
                    SET_BIT(_currentState->getPlayerInputState(e.getIndex())._inputState, GameInputStateFlag_MovingLeft, e.isPressed());
                    isMovingLeft[e.getIndex()] = _currentState->getPlayerInputState(e.getIndex())._inputState & GameInputStateFlag_MovingLeft;
                }
            }
                continue;
            case GamePadEventType_STICK_LEFT:
            {
                float val = sanitizeCloseToZeroValue(e.getX());
                if (!isMovingRight[e.getIndex()])
                {
                    SET_BIT(_currentState->getPlayerInputState(e.getIndex())._inputState, GameInputStateFlag_MovingRight, val > 0);
                    
                    isMovingRight[e.getIndex()] = _currentState->getPlayerInputState(e.getIndex())._inputState & GameInputStateFlag_MovingRight;
                }
                
                if (!isMovingLeft[e.getIndex()])
                {
                    SET_BIT(_currentState->getPlayerInputState(e.getIndex())._inputState, GameInputStateFlag_MovingLeft, val < 0);
                    isMovingLeft[e.getIndex()] = _currentState->getPlayerInputState(e.getIndex())._inputState & GameInputStateFlag_MovingLeft;
                }
            }
                continue;
            case GamePadEventType_X_BUTTON:
            case GamePadEventType_TRIGGER:
                if (!isAction[e.getIndex()])
                {
                    SET_BIT(_currentState->getPlayerInputState(e.getIndex())._inputState, GameInputStateFlag_MainAction, e.getX() > 0 || e.getY() > 0);
                    isAction[e.getIndex()] = _currentState->getPlayerInputState(e.getIndex())._inputState & GameInputStateFlag_MainAction;
                }
                continue;
            case GamePadEventType_BACK_BUTTON:
                if (e.isPressed())
                {
                    dropPlayer(e.getIndex());
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
            CursorEvent& e = *(*i);
            if (e.getType() == CursorEventType_DOWN
                || e.getType() == CursorEventType_DRAGGED)
            {
                Vector2& vec = CURSOR_CONVERTER->convert(*(*i));
                SET_BIT(_currentState->getPlayerInputState(0)._inputState, GameInputStateFlag_MovingLeft, vec.getX() < (GM_CFG->_camWidth / 2));
                SET_BIT(_currentState->getPlayerInputState(0)._inputState, GameInputStateFlag_MovingRight, vec.getX() > (GM_CFG->_camWidth / 2));
                
                if (e.getType() == CursorEventType_DOWN)
                {
                    SET_BIT(_currentState->getPlayerInputState(0)._inputState, GameInputStateFlag_Jumping, true);
                }
            }
            else
            {
                _currentState->getPlayerInputState(0)._inputState = 0;
                
                continue;
            }
        }
    }
    
    if (_currentState->isRequestingToAddLocalPlayer())
    {
        NG_CLIENT->requestToAddLocalPlayer();
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
    
    return _moveList.addMove(inputState, NG_TIME->getTime());
}

void GameInputManager::dropPlayer(int index)
{
    _currentState->getPlayerInputState(index)._playerId = INPUT_UNASSIGNED;
    
    if (index > 0)
    {
        NG_CLIENT->requestToDropLocalPlayer(1);
    }
    else
    {
        _inputState = GIMS_ESCAPE;
    }
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

GameInputManager::~GameInputManager()
{
    _inputStates.free(_currentState);
}
