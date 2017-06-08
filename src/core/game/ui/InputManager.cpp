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

void InputManager::update()
{
    if (isTimeToSampleInput())
    {
        SCREEN_INPUT_MANAGER->process();
        KEYBOARD_INPUT_MANAGER->process();
        GAME_PAD_INPUT_MANAGER->process();
        
        for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
        {
            switch ((*i)->getType())
            {
                case KeyboardEventType_W:
                    m_currentState->m_fDesiredJumpIntensity = (*i)->isDown() && !(*i)->isHeld() ? 1 : 0;
                    continue;
                case KeyboardEventType_A:
                    m_currentState->m_fDesiredLeftAmount = (*i)->isDown() ? 1 : 0;
                    continue;
                case KeyboardEventType_D:
                    m_currentState->m_fDesiredRightAmount = (*i)->isDown() ? 1 : 0;
                    continue;
                case KeyboardEventType_ARROW_KEY_DOWN:
                    m_currentState->m_isSprinting = (*i)->isDown() ? true : false;
                    continue;
                case KeyboardEventType_SPACE:
                    m_currentState->m_isShooting = (*i)->isDown() ? true : false;
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
                default:
                    continue;
            }
        }
        
        for (std::vector<GamePadEvent *>::iterator i = GAME_PAD_INPUT_MANAGER->getEvents().begin(); i != GAME_PAD_INPUT_MANAGER->getEvents().end(); ++i)
        {
            switch ((*i)->getType())
            {
                case GamePadEventType_D_PAD_UP:
                    m_currentState->m_fDesiredJumpIntensity = (*i)->isPressed() ? 1 : 0;
                    continue;
                case GamePadEventType_D_PAD_LEFT:
                    m_currentState->m_fDesiredLeftAmount = (*i)->isPressed() ? 1 : 0;
                    continue;
                case GamePadEventType_D_PAD_RIGHT:
                    m_currentState->m_fDesiredRightAmount = (*i)->isPressed() ? 1 : 0;
                    continue;
                case GamePadEventType_BUMPER_RIGHT:
                case GamePadEventType_BUMPER_LEFT:
                    m_currentState->m_isSprinting = (*i)->isPressed() ? true : false;
                    continue;
                case GamePadEventType_A_BUTTON:
                    m_currentState->m_isShooting = (*i)->isPressed() ? true : false;
                    continue;
                case GamePadEventType_BACK_BUTTON:
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
                default:
                    continue;
            }
        }
        
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
    InputState* inputState = static_cast<InputState*>(POOLED_OBJ_MGR->borrowInputState());
    m_currentState->copyTo(inputState);
    
    return m_moveList.addMove(inputState, Timing::getInstance()->getFrameStartTime());
}

bool InputManager::isTimeToSampleInput()
{
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
m_fNextTimeToSampleInput(0.0f)
{
    // Empty
}

InputManager::~InputManager()
{
    // Empty
}
