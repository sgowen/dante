//
//  InputManager.cpp
//  noctisgames-framework
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

InputManager* InputManager::getInstance()
{
    static InputManager instance = InputManager();
    return &instance;
}

void InputManager::update()
{
    SCREEN_INPUT_MANAGER->process();
    KEYBOARD_INPUT_MANAGER->process();
    GAME_PAD_INPUT_MANAGER->process();
    
    for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
    {
        switch ((*i)->getType())
        {
            case KeyboardEventType_W:
                m_currentState->m_fDesiredJumpIntensity = (*i)->isUp() ? 0 : 1;
                continue;
            case KeyboardEventType_A:
                m_currentState->m_fDesiredLeftAmount = (*i)->isUp() ? 0 : 1;
                continue;
            case KeyboardEventType_D:
                m_currentState->m_fDesiredRightAmount = (*i)->isUp() ? 0 : 1;
                continue;
            case KeyboardEventType_SPACE:
                m_currentState->m_isShooting = (*i)->isUp() ? false : true;
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
                m_currentState->m_fDesiredJumpIntensity = (*i)->isButtonPressed() ? 1 : 0;
                continue;
            case GamePadEventType_D_PAD_LEFT:
                m_currentState->m_fDesiredLeftAmount = (*i)->isButtonPressed() ? 1 : 0;
                continue;
            case GamePadEventType_D_PAD_RIGHT:
                m_currentState->m_fDesiredRightAmount = (*i)->isButtonPressed() ? 1 : 0;
                continue;
            case GamePadEventType_A_BUTTON:
                m_currentState->m_isShooting = (*i)->isButtonPressed() ? true : false;
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
    InputState* inputState = POOLED_OBJ_MGR->borrow();
    m_currentState->copyTo(inputState);
    
    return m_moveList.addMove(inputState, Timing::getInstance()->getFrameStartTime());
}

bool InputManager::isTimeToSampleInput()
{
    float time = Timing::getInstance()->getFrameStartTime();
    if (time > m_fNextTimeToSampleInput)
    {
        static const float kTimeBetweenInputSamples = 0.03f;
        
        m_fNextTimeToSampleInput = m_fNextTimeToSampleInput + kTimeBetweenInputSamples;
        
        return true;
    }
    
    return false;
}

InputManager::InputManager() :
m_currentState(POOLED_OBJ_MGR->borrow()),
m_fNextTimeToSampleInput(0.f),
m_pendingMove(nullptr)
{
    // Empty
}

InputManager::~InputManager()
{
    // Empty
}
