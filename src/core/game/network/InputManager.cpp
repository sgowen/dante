//
//  InputManager.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "InputManager.h"

#include "InputState.h"
#include "Move.h"

#include "Timing.h"
#include "ScreenInputManager.h"
#include "ScreenEvent.h"
#include "TouchConverter.h"
#include "KeyboardInputManager.h"
#include "KeyboardEvent.h"
#include "GamePadInputManager.h"
#include "GamePadEvent.h"
#include "PooledObjectsManager.h"
#include "GameConstants.h"
#include "KeyboardLookup.h"
#include "StringUtil.h"
#include "MathUtil.h"
#include "FrameworkConstants.h"

#include <sstream>

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
    getInstance()->m_currentState->activateNextPlayer(playerId);
}

void InputManager::update()
{
    SCREEN_INPUT_MANAGER->process();
    KEYBOARD_INPUT_MANAGER->process();
    GAME_PAD_INPUT_MANAGER->process();
    
    m_currentState->m_iMenuState = MENU_STATE_NONE;
    
    if (m_isConnected)
    {
        for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
        {
            switch ((*i)->getKey())
            {
                case NG_KEY_M:
                    m_currentState->m_iMenuState = (*i)->isUp() ? MENU_STATE_CLIENT_MAIN_TOGGLE_MUSIC : MENU_STATE_NONE;
                    continue;
                case NG_KEY_S:
                    m_currentState->m_iMenuState = (*i)->isUp() ? MENU_STATE_CLIENT_MAIN_TOGGLE_SOUND : MENU_STATE_NONE;
                    continue;
                case NG_KEY_T:
                    m_currentState->m_iMenuState = (*i)->isUp() ? MENU_STATE_SERVER_TOGGLE_ENEMIES : MENU_STATE_NONE;
                    continue;
                case NG_KEY_C:
                    m_currentState->m_iMenuState = (*i)->isUp() ? MENU_STATE_SERVER_TOGGLE_OBJECTS : MENU_STATE_NONE;
                    continue;
                case NG_KEY_I:
                    m_currentState->m_iMenuState = (*i)->isUp() ? MENU_STATE_SERVER_TOGGLE_SERVER_DISPLAY : MENU_STATE_NONE;
                    continue;
                case NG_KEY_ESCAPE:
                    m_currentState->m_iMenuState = (*i)->isUp() ? MENU_STATE_ESCAPE : MENU_STATE_NONE;
                    continue;
                default:
                {
                    if (!GAME_PAD_INPUT_MANAGER->isControllerConnected())
                    {
                        switch ((*i)->getKey())
                        {
                            // Player 1
                            case NG_KEY_W:
                                m_currentState->getGameInputState(0).m_isJumping = (*i)->isDown();
                                continue;
                            case NG_KEY_A:
                                m_currentState->getGameInputState(0).m_isMovingLeft = (*i)->isDown();
                                continue;
                            case NG_KEY_D:
                                m_currentState->getGameInputState(0).m_isMovingRight = (*i)->isDown();
                                continue;
                            case NG_KEY_ARROW_DOWN:
                                m_currentState->getGameInputState(0).m_isSprinting = (*i)->isDown();
                                continue;
                            case NG_KEY_SPACE_BAR:
                                m_currentState->getGameInputState(0).m_isShooting = (*i)->isDown();
                                continue;
#ifdef _DEBUG
                            // Player 2, Debug Only
                            case NG_KEY_ARROW_UP:
                                m_currentState->getGameInputState(1).m_isJumping = (*i)->isDown();
                                continue;
                            case NG_KEY_ARROW_LEFT:
                                m_currentState->getGameInputState(1).m_isMovingLeft = (*i)->isDown();
                                continue;
                            case NG_KEY_ARROW_RIGHT:
                                m_currentState->getGameInputState(1).m_isMovingRight = (*i)->isDown();
                                continue;
                            case NG_KEY_COMMA:
                                m_currentState->getGameInputState(1).m_isSprinting = (*i)->isDown();
                                continue;
							case NG_KEY_PERIOD:
                                m_currentState->getGameInputState(1).m_isShooting = (*i)->isDown();
                                continue;
                            case NG_KEY_CARRIAGE_RETURN:
                                m_currentState->getGameInputState(1).m_iPlayerId = INPUT_UNASSIGNED;
                                m_currentState->m_iMenuState = (*i)->isDown() ? MENU_STATE_LOCAL_PLAYER_DROP_OUT_1 : MENU_STATE_NONE;
                                continue;
#endif
                            default:
                                continue;
                        }
                    }
                }
                    continue;
            }
        }
        
        for (std::vector<GamePadEvent *>::iterator i = GAME_PAD_INPUT_MANAGER->getEvents().begin(); i != GAME_PAD_INPUT_MANAGER->getEvents().end(); ++i)
        {
            switch ((*i)->getType())
            {
                case GamePadEventType_A_BUTTON:
                    m_currentState->getGameInputState((*i)->getIndex()).m_isJumping = (*i)->isPressed();
                    continue;
                case GamePadEventType_STICK_LEFT:
                {
                    float val = sanitizeCloseToZeroValue((*i)->getX());
                    m_currentState->getGameInputState((*i)->getIndex()).m_isMovingRight = val > 0;
                    m_currentState->getGameInputState((*i)->getIndex()).m_isMovingLeft = val < 0;
                }
                    continue;
                case GamePadEventType_BUMPER_LEFT:
                case GamePadEventType_BUMPER_RIGHT:
                    m_currentState->getGameInputState((*i)->getIndex()).m_isSprinting = (*i)->isPressed();
                    continue;
                case GamePadEventType_X_BUTTON:
				case GamePadEventType_TRIGGER:
					m_currentState->getGameInputState((*i)->getIndex()).m_isShooting = (*i)->getX() > 0 || (*i)->getY() > 0;
					continue;
                case GamePadEventType_BACK_BUTTON:
                {
                    if ((*i)->getIndex() == 3)
                    {
                        m_currentState->getGameInputState((*i)->getIndex()).m_iPlayerId = INPUT_UNASSIGNED;
                        m_currentState->m_iMenuState = !(*i)->isPressed() ? MENU_STATE_LOCAL_PLAYER_DROP_OUT_3 : MENU_STATE_NONE;
                    }
                    else if ((*i)->getIndex() == 2)
                    {
                        m_currentState->getGameInputState((*i)->getIndex()).m_iPlayerId = INPUT_UNASSIGNED;
                        m_currentState->m_iMenuState = !(*i)->isPressed() ? MENU_STATE_LOCAL_PLAYER_DROP_OUT_2 : MENU_STATE_NONE;
                    }
                    else if ((*i)->getIndex() == 1)
                    {
                        m_currentState->getGameInputState((*i)->getIndex()).m_iPlayerId = INPUT_UNASSIGNED;
                        m_currentState->m_iMenuState = !(*i)->isPressed() ? MENU_STATE_LOCAL_PLAYER_DROP_OUT_1 : MENU_STATE_NONE;
                    }
                    else
                    {
                        m_currentState->m_iMenuState = (*i)->isPressed() ? MENU_STATE_ESCAPE : MENU_STATE_NONE;
                    }
                }
                    continue;
                default:
                    continue;
            }
        }
    }
    else if (m_isLiveMode)
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
                        m_isTimeToProcessInput = true;
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
                        m_liveInput += s;
                        if (m_liveInput.end() > m_liveInput.begin())
                        {
                            m_liveInput.erase(m_liveInput.end() - 1, m_liveInput.end());
                        }
                        return;
                    }
                    else
                    {
                        ss << StringUtil::format("%c", keyboardEvent->getKey());
                    }
                }
                else if (keyboardEvent->isUp())
                {
                    if (keyboardEvent->getKey() == NG_KEY_ESCAPE)
                    {
                        m_currentState->m_iMenuState = MENU_STATE_ESCAPE;
                        return;
                    }
                }
            }
        }
        
        std::string s = ss.str();
        m_liveInput += s;
        if (m_liveInput.length() > 16)
        {
            int sub = static_cast<int>(m_liveInput.length()) - 16;
            m_liveInput.erase(m_liveInput.end() - sub, m_liveInput.end());
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
                    m_currentState->m_iMenuState = MENU_STATE_ACTIVATE_STEAM;
                    continue;
                case NG_KEY_D:
                    m_currentState->m_iMenuState = MENU_STATE_DEACTIVATE_STEAM;
                    continue;
                case NG_KEY_S:
                    m_currentState->m_iMenuState = MENU_STATE_START_SERVER;
                    continue;
                case NG_KEY_J:
                    m_currentState->m_iMenuState = MENU_STATE_JOIN_LOCAL_SERVER;
                    continue;
                case NG_KEY_L:
                    m_currentState->m_iMenuState = MENU_STATE_STEAM_REFRESH_LAN_SERVERS;
                    continue;
                case NG_KEY_I:
                    m_currentState->m_iMenuState = MENU_STATE_STEAM_REFRESH_INTERNET_SERVERS;
                    continue;
                case NG_KEY_ONE:
                    m_currentState->m_iMenuState = MENU_STATE_STEAM_JOIN_SERVER_1;
                    continue;
                case NG_KEY_TWO:
                    m_currentState->m_iMenuState = MENU_STATE_STEAM_JOIN_SERVER_2;
                    continue;
                case NG_KEY_THREE:
                    m_currentState->m_iMenuState = MENU_STATE_STEAM_JOIN_SERVER_3;
                    continue;
                case NG_KEY_FOUR:
                    m_currentState->m_iMenuState = MENU_STATE_STEAM_JOIN_SERVER_4;
                    continue;
                case NG_KEY_ESCAPE:
                    m_currentState->m_iMenuState = MENU_STATE_ESCAPE;
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
                    m_currentState->m_iMenuState = MENU_STATE_START_SERVER;
                    continue;
                case GamePadEventType_BACK_BUTTON:
                    m_currentState->m_iMenuState = MENU_STATE_ESCAPE;
                    continue;
                default:
                    continue;
            }
        }
    }
    
    if (isTimeToSampleInput())
    {
        m_pendingMove = &sampleInputAsMove();
    }
}

const Move* InputManager::getPendingMove()
{
    return m_pendingMove;
}

void InputManager::clearPendingMove()
{
    m_pendingMove = nullptr;
}

void InputManager::setConnected(bool isConnected)
{
    m_isConnected = isConnected;
    
    m_moveList.clear();
    
    m_currentState->reset();
}

void InputManager::setLiveMode(bool isLiveMode)
{
    m_isLiveMode = isLiveMode;
    
    m_moveList.clear();
    
    m_liveInput.clear();
    
    m_isTimeToProcessInput = false;
}

void InputManager::resetLiveInput()
{
    m_liveInput.clear();
    
    m_isTimeToProcessInput = false;
}

bool InputManager::isLiveMode()
{
    return m_isLiveMode;
}

bool InputManager::isTimeToProcessInput()
{
    return m_isTimeToProcessInput;
}

InputState* InputManager::getInputState()
{
    return m_currentState;
}

MoveList& InputManager::getMoveList()
{
    return m_moveList;
}

std::string& InputManager::getLiveInputRef()
{
    return m_liveInput;
}

std::string InputManager::getLiveInput()
{
    return m_liveInput;
}

bool InputManager::isPlayerIdLocalHost(uint8_t playerId)
{
    return m_currentState->isPlayerIdLocalHost(playerId);
}

const Move& InputManager::sampleInputAsMove()
{
    InputState* inputState = static_cast<InputState*>(POOLED_OBJ_MGR->borrowInputState());
    m_currentState->copyTo(inputState);
    
    return m_moveList.addMove(inputState, Timing::getInstance()->getFrameStartTime());
}

bool InputManager::isTimeToSampleInput()
{
    if (!m_isConnected)
    {
        return false;
    }
    
    return true;
}

InputManager::InputManager() :
m_currentState(static_cast<InputState*>(POOLED_OBJ_MGR->borrowInputState())),
m_pendingMove(nullptr),
m_isConnected(false),
m_isLiveMode(false),
m_isTimeToProcessInput(false)
{
    // Empty
}

InputManager::~InputManager()
{
    // Empty
}
