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
#include "NGAudioEngine.h"
#include "KeyboardLookup.h"
#include "StringUtil.h"

#include <sstream>

InputManager* InputManager::getInstance()
{
    static InputManager instance = InputManager();
    return &instance;
}

void InputManager::staticRemoveProcessedMoves(float inLastMoveProcessedOnServerTimestamp)
{
    getInstance()->getMoveList().removeProcessedMoves(inLastMoveProcessedOnServerTimestamp);
}

MoveList& InputManager::staticGetMoveList()
{
    return getInstance()->getMoveList();
}

void InputManager::update()
{
    SCREEN_INPUT_MANAGER->process();
    KEYBOARD_INPUT_MANAGER->process();
    GAME_PAD_INPUT_MANAGER->process();

	m_currentState->m_isStartingServer = false;
	m_currentState->m_isJoiningServer = false;
    m_currentState->m_isStartingSteamServer = false;
    m_currentState->m_isJoiningOnlineSteamServer = false;
    m_currentState->m_isJoiningLANSteamServer = false;
	m_currentState->m_isLeavingServer = false;
    
    if (m_isLiveMode)
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
                        m_liveInput.erase(m_liveInput.end() - 1, m_liveInput.end());
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
                        m_isEscapeKeyPressed = true;
                        return;
                    }
                }
            }
        }
        
        std::string s = ss.str();
        m_liveInput += s;
    }
    else
    {
        for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
        {
            switch ((*i)->getKey())
            {
                case NG_KEY_W:
                    m_currentState->m_isJumping = (*i)->isDown();
                    continue;
                case NG_KEY_A:
                    m_currentState->m_fDesiredLeftAmount = (*i)->isDown() ? 1 : 0;
                    continue;
                case NG_KEY_D:
                    m_currentState->m_fDesiredRightAmount = (*i)->isDown() ? 1 : 0;
                    continue;
                case NG_KEY_ARROW_DOWN:
                    m_currentState->m_isSprinting = (*i)->isDown();
                    continue;
                case NG_KEY_SPACE_BAR:
                    m_currentState->m_isShooting = (*i)->isDown();
                    continue;
                case NG_KEY_M:
                    if ((*i)->isUp())
                    {
                        if (NG_AUDIO_ENGINE->isMusicPlaying())
                        {
                            NG_AUDIO_ENGINE->stopMusic();
                        }
                        else
                        {
                            NG_AUDIO_ENGINE->playMusic(true);
                        }
                    }
                    continue;
                case NG_KEY_S:
                    m_currentState->m_isStartingSteamServer = (*i)->isUp();
                    continue;
                case NG_KEY_J:
                    m_currentState->m_isJoiningOnlineSteamServer = (*i)->isUp();
                    continue;
                case NG_KEY_K:
                    m_currentState->m_isJoiningLANSteamServer = (*i)->isUp();
                    continue;
                case NG_KEY_L:
                    m_currentState->m_isStartingServer = (*i)->isUp();
                    continue;
                case NG_KEY_P:
                    m_currentState->m_isJoiningServer = (*i)->isUp();
                    continue;
                case NG_KEY_ESCAPE:
                    m_currentState->m_isLeavingServer = (*i)->isUp();
                    continue;
                default:
                    continue;
            }
        }
    }
    
    for (std::vector<GamePadEvent *>::iterator i = GAME_PAD_INPUT_MANAGER->getEvents().begin(); i != GAME_PAD_INPUT_MANAGER->getEvents().end(); ++i)
    {
        switch ((*i)->getType())
        {
            case GamePadEventType_D_PAD_UP:
            case GamePadEventType_A_BUTTON:
                m_currentState->m_isJumping = (*i)->isPressed();
                continue;
            case GamePadEventType_D_PAD_LEFT:
                m_currentState->m_fDesiredLeftAmount = (*i)->isPressed() ? 1 : 0;
                continue;
            case GamePadEventType_D_PAD_RIGHT:
                m_currentState->m_fDesiredRightAmount = (*i)->isPressed() ? 1 : 0;
                continue;
            case GamePadEventType_STICK_LEFT:
                m_currentState->m_fDesiredRightAmount = (*i)->getX();
                continue;
            case GamePadEventType_BUMPER_RIGHT:
            case GamePadEventType_BUMPER_LEFT:
                m_currentState->m_isSprinting = (*i)->isPressed();
                continue;
            case GamePadEventType_X_BUTTON:
                m_currentState->m_isShooting = (*i)->isPressed();
                continue;
            case GamePadEventType_Y_BUTTON:
                if ((*i)->isPressed())
                {
                    if (NG_AUDIO_ENGINE->isMusicPlaying())
                    {
                        NG_AUDIO_ENGINE->stopMusic();
                    }
                    else
                    {
                        NG_AUDIO_ENGINE->playMusic(true);
                    }
                }
                continue;
            case GamePadEventType_START_BUTTON:
                m_currentState->m_isJoiningOnlineSteamServer = (*i)->isPressed();
                continue;
            case GamePadEventType_BACK_BUTTON:
                m_currentState->m_isStartingSteamServer = (*i)->isPressed();
                continue;
            case GamePadEventType_B_BUTTON:
                m_currentState->m_isLeavingServer = (*i)->isPressed();
                continue;
            default:
                continue;
        }
    }
    
    if (isTimeToSampleInput())
    {
        m_pendingMove = &sampleInputAsMove();
    }
}

const Move* InputManager::getAndClearPendingMove()
{
    auto toRet = m_pendingMove;
    m_pendingMove = nullptr;
    
    return toRet;
}

void InputManager::setConnected(bool isConnected)
{
    m_isConnected = isConnected;
    
    if (!m_isConnected)
    {
        m_moveList.clear();
    }
}

void InputManager::setLiveMode(bool isLiveMode)
{
    m_isLiveMode = isLiveMode;
    
    m_moveList.clear();
    
    m_liveInput.clear();
    
    m_isTimeToProcessInput = false;
    m_isEscapeKeyPressed = false;
}

void InputManager::resetLiveInput()
{
    m_liveInput.clear();
    
    m_isTimeToProcessInput = false;
    m_isEscapeKeyPressed = false;
}

bool InputManager::isLiveMode()
{
    return m_isLiveMode;
}

bool InputManager::isTimeToProcessInput()
{
    return m_isTimeToProcessInput;
}

bool InputManager::isEscapeKeyPressed()
{
    return m_isEscapeKeyPressed;
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
    
    float time = Timing::getInstance()->getFrameStartTime();
    if (time > m_fNextTimeToSampleInput)
    {
        m_fNextTimeToSampleInput = time + FRAME_RATE;
        
        return true;
    }
    
    return false;
}

InputManager::InputManager() :
m_currentState(static_cast<InputState*>(POOLED_OBJ_MGR->borrowInputState())),
m_pendingMove(nullptr),
m_fNextTimeToSampleInput(0.0f),
m_isConnected(false),
m_isLiveMode(false),
m_isTimeToProcessInput(false),
m_isEscapeKeyPressed(false)
{
    // Empty
}

InputManager::~InputManager()
{
    // Empty
}
