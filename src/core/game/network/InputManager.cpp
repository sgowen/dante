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
	m_currentState->m_isLeavingServer = false;
    
    for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
    {
        switch ((*i)->getType())
        {
            case KeyboardEventType_W:
                m_currentState->m_isJumping = (*i)->isDown();
                continue;
            case KeyboardEventType_A:
                m_currentState->m_fDesiredLeftAmount = (*i)->isDown() ? 1 : 0;
                continue;
            case KeyboardEventType_D:
                m_currentState->m_fDesiredRightAmount = (*i)->isDown() ? 1 : 0;
                continue;
            case KeyboardEventType_ARROW_KEY_DOWN:
                m_currentState->m_isSprinting = (*i)->isDown();
                continue;
            case KeyboardEventType_SPACE:
                m_currentState->m_isShooting = (*i)->isDown();
                continue;
            case KeyboardEventType_M:
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
            case KeyboardEventType_S:
				m_currentState->m_isStartingServer = (*i)->isUp();
                continue;
            case KeyboardEventType_J:
                m_currentState->m_isJoiningServer = (*i)->isUp();
                continue;
            case KeyboardEventType_ESCAPE:
                m_currentState->m_isLeavingServer = (*i)->isUp();
                continue;
            default:
                continue;
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
                m_currentState->m_isJoiningServer = (*i)->isPressed();
                continue;
            case GamePadEventType_BACK_BUTTON:
                m_currentState->m_isStartingServer = (*i)->isPressed();
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

void InputManager::setConnected(bool isConnected)
{
    m_isConnected = isConnected;
    
    if (!m_isConnected)
    {
        m_moveList.clear();
    }
}

InputState* InputManager::getInputState()
{
    return m_currentState;
}

MoveList& InputManager::getMoveList()
{
    return m_moveList;
}

const Move* InputManager::getAndClearPendingMove()
{
    auto toRet = m_pendingMove;
    m_pendingMove = nullptr;
    
    return toRet;
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
m_isConnected(false)
{
    // Empty
}

InputManager::~InputManager()
{
    // Empty
}
