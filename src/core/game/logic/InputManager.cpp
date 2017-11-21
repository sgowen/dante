//
//  InputManager.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/InputManager.h"

#include "game/network/MainInputState.h"
#include "framework/network/portable/Move.h"

#include "framework/util/Timing.h"
#include "framework/input/CursorInputManager.h"
#include "framework/input/CursorEvent.h"
#include "framework/input/CursorConverter.h"
#include "framework/input/KeyboardInputManager.h"
#include "framework/input/KeyboardEvent.h"
#include "framework/input/GamePadInputManager.h"
#include "framework/input/GamePadEvent.h"
#include "game/logic/PooledObjectsManager.h"
#include "game/logic/GameConstants.h"
#include "framework/input/KeyboardLookup.h"
#include "framework/util/StringUtil.h"
#include "framework/math/MathUtil.h"
#include "framework/util/FrameworkConstants.h"

#include <sstream>

#if defined __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        #define NG_MOBILE
    #endif
#elif defined __ANDROID__
    #define NG_MOBILE
#endif

InputManager* InputManager::getInstance()
{
    static InputManager instance = InputManager();
    return &instance;
}

void InputManager::sRemoveProcessedMoves(float inLastMoveProcessedOnServerTimestamp)
{
    getInstance()->getMoveList().removeProcessedMoves(inLastMoveProcessedOnServerTimestamp);
}

MoveList& InputManager::sGetMoveList()
{
    return getInstance()->getMoveList();
}

void InputManager::sOnPlayerWelcomed(uint8_t playerId)
{
    getInstance()->_currentState->activateNextPlayer(playerId);
}

void InputManager::update()
{
    CURSOR_INPUT_MANAGER->process();
    KEYBOARD_INPUT_MANAGER->process();
    GAME_PAD_INPUT_MANAGER->process();
    
    _currentState->_menuState = MENU_STATE_NONE;
    
    if (_isConnected)
    {
        for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
        {
            switch ((*i)->getKey())
            {
                case NG_KEY_M:
                    _currentState->_menuState = (*i)->isUp() ? MENU_STATE_CLIENT_MAIN_TOGGLE_MUSIC : _currentState->_menuState;
                    continue;
                case NG_KEY_S:
                    _currentState->_menuState = (*i)->isUp() ? MENU_STATE_CLIENT_MAIN_TOGGLE_SOUND : _currentState->_menuState;
                    continue;
                case NG_KEY_T:
                    _currentState->_menuState = (*i)->isUp() ? MENU_STATE_SERVER_TOGGLE_ENEMIES : _currentState->_menuState;
                    continue;
                case NG_KEY_C:
                    _currentState->_menuState = (*i)->isUp() ? MENU_STATE_SERVER_TOGGLE_OBJECTS : _currentState->_menuState;
                    continue;
                case NG_KEY_I:
                    _currentState->_menuState = (*i)->isUp() ? MENU_STATE_SERVER_TOGGLE_SERVER_DISPLAY : _currentState->_menuState;
                    continue;
                case NG_KEY_ESCAPE:
                    _currentState->_menuState = (*i)->isUp() ? MENU_STATE_ESCAPE : _currentState->_menuState;
                    continue;
                default:
                {
                    switch ((*i)->getKey())
                    {
                        // Player 1
                        case NG_KEY_W:
                            _currentState->getGameInputState(0)._isJumping = (*i)->isDown();
                            continue;
                        case NG_KEY_A:
                            _currentState->getGameInputState(0)._isMovingLeft = (*i)->isDown();
                            continue;
                        case NG_KEY_D:
                            _currentState->getGameInputState(0)._isMovingRight = (*i)->isDown();
                            continue;
                        case NG_KEY_ARROW_DOWN:
                            _currentState->getGameInputState(0)._isSprinting = (*i)->isDown();
                            continue;
                        case NG_KEY_SPACE_BAR:
                            _currentState->getGameInputState(0)._isShooting = (*i)->isDown();
                            continue;
#ifdef _DEBUG
                        // Player 2, Debug Only
                        case NG_KEY_ARROW_UP:
                            _currentState->getGameInputState(1)._isJumping = (*i)->isDown();
                            continue;
                        case NG_KEY_ARROW_LEFT:
                            _currentState->getGameInputState(1)._isMovingLeft = (*i)->isDown();
                            continue;
                        case NG_KEY_ARROW_RIGHT:
                            _currentState->getGameInputState(1)._isMovingRight = (*i)->isDown();
                            continue;
                        case NG_KEY_COMMA:
                            _currentState->getGameInputState(1)._isSprinting = (*i)->isDown();
                            continue;
                        case NG_KEY_PERIOD:
                            _currentState->getGameInputState(1)._isShooting = (*i)->isDown();
                            continue;
                        case NG_KEY_CARRIAGE_RETURN:
                            _currentState->getGameInputState(1)._playerId = INPUT_UNASSIGNED;
                            _currentState->_menuState = (*i)->isDown() ? MENU_STATE_LOCAL_PLAYER_DROP_OUT_1 : MENU_STATE_NONE;
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
        bool isSprinting[4] = {false};
        bool isShooting[4] = {false};
        
        for (std::vector<GamePadEvent *>::iterator i = GAME_PAD_INPUT_MANAGER->getEvents().begin(); i != GAME_PAD_INPUT_MANAGER->getEvents().end(); ++i)
        {
            switch ((*i)->getType())
            {
                case GamePadEventType_A_BUTTON:
                    _currentState->getGameInputState((*i)->getIndex())._isJumping = (*i)->isPressed();
                    continue;
                case GamePadEventType_D_PAD_RIGHT:
                {
                    if (!isMovingRight[(*i)->getIndex()])
                    {
                        _currentState->getGameInputState((*i)->getIndex())._isMovingRight = (*i)->isPressed();
                        isMovingRight[(*i)->getIndex()] = _currentState->getGameInputState((*i)->getIndex())._isMovingRight;
                    }
                }
                    continue;
                case GamePadEventType_D_PAD_LEFT:
                {
                    if (!isMovingLeft[(*i)->getIndex()])
                    {
                        _currentState->getGameInputState((*i)->getIndex())._isMovingLeft = (*i)->isPressed();
                        isMovingLeft[(*i)->getIndex()] = _currentState->getGameInputState((*i)->getIndex())._isMovingLeft;
                    }
                }
                    continue;
                case GamePadEventType_STICK_LEFT:
                {
                    float val = sanitizeCloseToZeroValue((*i)->getX());
                    if (!isMovingRight[(*i)->getIndex()])
                    {
                        _currentState->getGameInputState((*i)->getIndex())._isMovingRight = val > 0;
                        isMovingRight[(*i)->getIndex()] = _currentState->getGameInputState((*i)->getIndex())._isMovingRight;
                    }
                    
                    if (!isMovingLeft[(*i)->getIndex()])
                    {
                        _currentState->getGameInputState((*i)->getIndex())._isMovingLeft = val < 0;
                        isMovingLeft[(*i)->getIndex()] = _currentState->getGameInputState((*i)->getIndex())._isMovingLeft;
                    }
                }
                    continue;
                case GamePadEventType_BUMPER_LEFT:
                case GamePadEventType_BUMPER_RIGHT:
                    if (!isSprinting[(*i)->getIndex()])
                    {
                        _currentState->getGameInputState((*i)->getIndex())._isSprinting = (*i)->isPressed();
                        isSprinting[(*i)->getIndex()] = _currentState->getGameInputState((*i)->getIndex())._isSprinting;
                    }
                    continue;
                case GamePadEventType_X_BUTTON:
				case GamePadEventType_TRIGGER:
                    if (!isShooting[(*i)->getIndex()])
                    {
                        _currentState->getGameInputState((*i)->getIndex())._isShooting = (*i)->getX() > 0 || (*i)->getY() > 0;
                        isShooting[(*i)->getIndex()] = _currentState->getGameInputState((*i)->getIndex())._isShooting;
                    }
					continue;
                case GamePadEventType_BACK_BUTTON:
                {
                    if ((*i)->getIndex() == 3)
                    {
                        _currentState->getGameInputState((*i)->getIndex())._playerId = INPUT_UNASSIGNED;
                        _currentState->_menuState = !(*i)->isPressed() ? MENU_STATE_LOCAL_PLAYER_DROP_OUT_3 : MENU_STATE_NONE;
                    }
                    else if ((*i)->getIndex() == 2)
                    {
                        _currentState->getGameInputState((*i)->getIndex())._playerId = INPUT_UNASSIGNED;
                        _currentState->_menuState = !(*i)->isPressed() ? MENU_STATE_LOCAL_PLAYER_DROP_OUT_2 : MENU_STATE_NONE;
                    }
                    else if ((*i)->getIndex() == 1)
                    {
                        _currentState->getGameInputState((*i)->getIndex())._playerId = INPUT_UNASSIGNED;
                        _currentState->_menuState = !(*i)->isPressed() ? MENU_STATE_LOCAL_PLAYER_DROP_OUT_1 : MENU_STATE_NONE;
                    }
                    else
                    {
                        _currentState->_menuState = (*i)->isPressed() ? MENU_STATE_ESCAPE : _currentState->_menuState;
                    }
                }
                    continue;
                default:
                    continue;
            }
        }
        
#ifdef NG_MOBILE
        for (std::vector<CursorEvent *>::iterator i = CURSOR_INPUT_MANAGER->getEvents().begin(); i != CURSOR_INPUT_MANAGER->getEvents().end(); ++i)
        {
            if ((*i)->getType() == CursorEventType_DOWN
                || (*i)->getType() == CursorEventType_DRAGGED)
            {
                Vector2& vec = CURSOR_CONVERTER->touchToWorld(*(*i));
                _currentState->getGameInputState(0)._isMovingLeft = vec.getX() < (CAM_WIDTH / 2);
                _currentState->getGameInputState(0)._isMovingRight = vec.getX() > (CAM_WIDTH / 2);
            }
            else
            {
                _currentState->getGameInputState(0)._isMovingLeft = false;
                _currentState->getGameInputState(0)._isMovingRight = false;
                
                continue;
            }
        }
#endif
    }
    else if (_isLiveMode)
    {
        std::stringstream ss;
        
        for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
        {
            KeyboardEvent* keyboardEvent = (*i);
            if (isKeySupported(keyboardEvent->getKey()))
            {
                if (keyboardEvent->isDown()
                    && !keyboardEvent->isHeld())
                {
                    if (keyboardEvent->getKey() == NG_KEY_CARRIAGE_RETURN)
                    {
                        _isTimeToProcessInput = true;
                        return;
                    }
                    else if (keyboardEvent->getKey() == NG_KEY_ESCAPE)
                    {
                        return;
                    }
                    else if (keyboardEvent->getKey() == NG_KEY_BACK_SPACE
                             || keyboardEvent->getKey() == NG_KEY_DELETE)
                    {
                        std::string s = ss.str();
                        _liveInput += s;
                        if (_liveInput.end() > _liveInput.begin())
                        {
                            _liveInput.erase(_liveInput.end() - 1, _liveInput.end());
                        }
                        return;
                    }
                    else
                    {
						unsigned short key = keyboardEvent->getKey();
#ifdef _WIN32
						if (key == NG_KEY_PERIOD)
						{
							key = NG_KEY_ASCII_PERIOD;
						}
#endif
                        ss << StringUtil::format("%c", key);
                    }
                }
                else if (keyboardEvent->isUp())
                {
                    if (keyboardEvent->getKey() == NG_KEY_ESCAPE)
                    {
                        _currentState->_menuState = MENU_STATE_ESCAPE;
                        return;
                    }
                }
            }
        }
        
#ifdef NG_MOBILE
        for (std::vector<CursorEvent *>::iterator i = CURSOR_INPUT_MANAGER->getEvents().begin(); i != CURSOR_INPUT_MANAGER->getEvents().end(); ++i)
        {
            if ((*i)->getType() == CursorEventType_UP)
            {
                if (_liveInput.length() > 0)
                {
                    _isTimeToProcessInput = true;
                }
                else
                {
                    ss << "mobile";
                }
            }
            else
            {
                continue;
            }
        }
#endif
        
        std::string s = ss.str();
        _liveInput += s;
        if (_liveInput.length() > 16)
        {
            int sub = static_cast<int>(_liveInput.length()) - 16;
            _liveInput.erase(_liveInput.end() - sub, _liveInput.end());
        }
    }
    else
    {
        for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
        {
            if (!(*i)->isUp())
            {
                continue;
            }
            
            switch ((*i)->getKey())
            {
                case NG_KEY_A:
                    _currentState->_menuState = MENU_STATE_ACTIVATE_STEAM;
                    continue;
                case NG_KEY_D:
                    _currentState->_menuState = MENU_STATE_DEACTIVATE_STEAM;
                    continue;
                case NG_KEY_S:
                    _currentState->_menuState = MENU_STATE_START_SERVER;
                    continue;
                case NG_KEY_J:
                    _currentState->_menuState = MENU_STATE_JOIN_LOCAL_SERVER;
                    continue;
                case NG_KEY_L:
                    _currentState->_menuState = MENU_STATE_STEAM_REFRESH_LAN_SERVERS;
                    continue;
                case NG_KEY_I:
                    _currentState->_menuState = MENU_STATE_STEAM_REFRESH_INTERNET_SERVERS;
                    continue;
                case NG_KEY_ONE:
                    _currentState->_menuState = MENU_STATE_STEAM_JOIN_SERVER_1;
                    continue;
                case NG_KEY_TWO:
                    _currentState->_menuState = MENU_STATE_STEAM_JOIN_SERVER_2;
                    continue;
                case NG_KEY_THREE:
                    _currentState->_menuState = MENU_STATE_STEAM_JOIN_SERVER_3;
                    continue;
                case NG_KEY_FOUR:
                    _currentState->_menuState = MENU_STATE_STEAM_JOIN_SERVER_4;
                    continue;
                case NG_KEY_ESCAPE:
                    _currentState->_menuState = MENU_STATE_ESCAPE;
                    continue;
                default:
                    continue;
            }
        }
        
        for (std::vector<GamePadEvent *>::iterator i = GAME_PAD_INPUT_MANAGER->getEvents().begin(); i != GAME_PAD_INPUT_MANAGER->getEvents().end(); ++i)
        {
            if (!(*i)->isPressed())
            {
                continue;
            }
            
            switch ((*i)->getType())
            {
                case GamePadEventType_START_BUTTON:
                    _currentState->_menuState = MENU_STATE_START_SERVER;
                    continue;
                case GamePadEventType_BACK_BUTTON:
                    _currentState->_menuState = MENU_STATE_ESCAPE;
                    continue;
                default:
                    continue;
            }
        }
        
#ifdef NG_MOBILE
        for (std::vector<CursorEvent *>::iterator i = CURSOR_INPUT_MANAGER->getEvents().begin(); i != CURSOR_INPUT_MANAGER->getEvents().end(); ++i)
        {
            if ((*i)->getType() == CursorEventType_UP)
            {
                _currentState->_menuState = MENU_STATE_START_SERVER;
            }
            else
            {
                continue;
            }
        }
#endif
    }
    
    if (isTimeToSampleInput())
    {
        _pendingMove = &sampleInputAsMove();
    }
}

const Move* InputManager::getPendingMove()
{
    return _pendingMove;
}

void InputManager::clearPendingMove()
{
    _pendingMove = NULL;
}

void InputManager::setConnected(bool isConnected)
{
    _isConnected = isConnected;
    
    _moveList.clear();
    
    _currentState->reset();
}

void InputManager::setLiveMode(bool isLiveMode)
{
    _isLiveMode = isLiveMode;
    
    _moveList.clear();
    
    _liveInput.clear();
    
    _isTimeToProcessInput = false;
}

void InputManager::resetLiveInput()
{
    _liveInput.clear();
    
    _isTimeToProcessInput = false;
}

bool InputManager::isLiveMode()
{
    return _isLiveMode;
}

bool InputManager::isTimeToProcessInput()
{
    return _isTimeToProcessInput;
}

MainInputState* InputManager::getInputState()
{
    return _currentState;
}

MoveList& InputManager::getMoveList()
{
    return _moveList;
}

std::string& InputManager::getLiveInputRef()
{
    return _liveInput;
}

std::string InputManager::getLiveInput()
{
    return _liveInput;
}

bool InputManager::isPlayerIdLocalHost(uint8_t playerId)
{
    return _currentState->isPlayerIdLocalHost(playerId);
}

const Move& InputManager::sampleInputAsMove()
{
    MainInputState* inputState = static_cast<MainInputState*>(POOLED_OBJ_MGR->borrowInputState());
    _currentState->copyTo(inputState);
    
    return _moveList.addMove(inputState, Timing::getInstance()->getFrameStartTime());
}

bool InputManager::isTimeToSampleInput()
{
    if (!_isConnected)
    {
        return false;
    }
    
    return true;
}

InputManager::InputManager() :
_currentState(static_cast<MainInputState*>(POOLED_OBJ_MGR->borrowInputState())),
_pendingMove(NULL),
_isConnected(false),
_isLiveMode(false),
_isTimeToProcessInput(false)
{
    // Empty
}

InputManager::~InputManager()
{
    // Empty
}
