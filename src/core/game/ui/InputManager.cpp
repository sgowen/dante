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
#include "ScreenInputManager.h"
#include "ScreenEvent.h"
#include "TouchConverter.h"
#include "KeyboardInputManager.h"
#include "KeyboardEvent.h"
#include "GamePadInputManager.h"
#include "GamePadEvent.h"

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
    
//    for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
//    {
//        switch ((*i)->getType())
//        {
//            case KeyboardEventType_W:
//                m_currentState.getDesiredVerticalDelta()
//                InputManager::getInstance()->handleInput((*i)->isUp() ? EIA_Released : EIA_Pressed, 'W');
//                continue;
//            case KeyboardEventType_A:
//                InputManager::getInstance()->handleInput((*i)->isUp() ? EIA_Released : EIA_Pressed, 'A');
//                continue;
//            case KeyboardEventType_S:
//                InputManager::getInstance()->handleInput((*i)->isUp() ? EIA_Released : EIA_Pressed, 'S');
//                continue;
//            case KeyboardEventType_D:
//                InputManager::getInstance()->handleInput((*i)->isUp() ? EIA_Released : EIA_Pressed, 'D');
//                continue;
//            default:
//                continue;
//        }
//    }
//    
//    for (std::vector<GamePadEvent *>::iterator i = GAME_PAD_INPUT_MANAGER->getEvents().begin(); i != GAME_PAD_INPUT_MANAGER->getEvents().end(); ++i)
//    {
//        switch ((*i)->getType())
//        {
//            case GamePadEventType_D_PAD_UP:
//                InputManager::getInstance()->handleInput((*i)->isButtonPressed() ? EIA_Pressed : EIA_Released, 'W');
//                continue;
//            case GamePadEventType_D_PAD_LEFT:
//                InputManager::getInstance()->handleInput((*i)->isButtonPressed() ? EIA_Pressed : EIA_Released, 'A');
//                continue;
//            case GamePadEventType_D_PAD_DOWN:
//                InputManager::getInstance()->handleInput((*i)->isButtonPressed() ? EIA_Pressed : EIA_Released, 'S');
//                continue;
//            case GamePadEventType_D_PAD_RIGHT:
//                InputManager::getInstance()->handleInput((*i)->isButtonPressed() ? EIA_Pressed : EIA_Released, 'D');
//                continue;
//            default:
//                continue;
//        }
//    }
    
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
