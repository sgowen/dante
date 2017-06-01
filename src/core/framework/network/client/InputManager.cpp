//
//  InputManager.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "InputManager.h"

#include "Move.h"

#include "Timing.h"
#include "FrameworkConstants.h"

InputManager* InputManager::getInstance()
{
    static InputManager instance = InputManager();
    return &instance;
}

namespace
{
    inline void UpdateDesireFloatFromKey(EInputAction inInputAction, float& ioVariable)
    {
        if (inInputAction == EIA_Pressed)
        {
            ioVariable = 1.f;
        }
        else if (inInputAction == EIA_Released)
        {
            ioVariable = 0.f;
        }
    }
}

void InputManager::handleInput(EInputAction inInputAction, char inKeyCode)
{
    switch(inKeyCode)
    {
        case 'D':
            UpdateDesireFloatFromKey(inInputAction, m_currentState.m_fDesiredLeftAmount);
            break;
        case 'A':
            UpdateDesireFloatFromKey(inInputAction, m_currentState.m_fDesiredRightAmount);
            break;
        case 'W':
            UpdateDesireFloatFromKey(inInputAction, m_currentState.m_fDesiredForwardAmount);
            break;
        case 'S':
            UpdateDesireFloatFromKey(inInputAction, m_currentState.m_fDesiredBackAmount);
            break;
    }
}

void InputManager::update()
{
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
    return m_moveList.AddMove(m_currentState, Timing::getInstance()->getFrameStartTime());
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

InputManager::InputManager() : m_fNextTimeToSampleInput(0.f), m_pendingMove(nullptr)
{
    // Empty
}
