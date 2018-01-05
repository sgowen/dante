//
//  GameInputManager.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/GameInputManager.h"

#include "game/logic/MainInputState.h"
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
#include "framework/util/PlatformHelper.h"

#include <sstream>

GameInputManager* GameInputManager::getInstance()
{
    static GameInputManager instance = GameInputManager();
    return &instance;
}

void GameInputManager::sRemoveProcessedMoves(float inLastMoveProcessedOnServerTimestamp)
{
    getInstance()->getMoveList().removeProcessedMoves(inLastMoveProcessedOnServerTimestamp);
}

MoveList& GameInputManager::sGetMoveList()
{
    return getInstance()->getMoveList();
}

void GameInputManager::sOnPlayerWelcomed(uint8_t playerId)
{
    getInstance()->_currentState->activateNextPlayer(playerId);
}

void GameInputManager::update()
{
    CURSOR_INPUT_MANAGER->process();
    KEYBOARD_INPUT_MANAGER->process();
    GAME_PAD_INPUT_MANAGER->process();
    
    _menuState = MIS_NONE;
    
    for (std::vector<KeyboardEvent *>::iterator i = KEYBOARD_INPUT_MANAGER->getEvents().begin(); i != KEYBOARD_INPUT_MANAGER->getEvents().end(); ++i)
    {
        switch ((*i)->getKey())
        {
            case NG_KEY_M:
                _menuState = (*i)->isUp() ? MIS_CLIENT_MAIN_TOGGLE_MUSIC : _menuState;
                continue;
            case NG_KEY_S:
                _menuState = (*i)->isUp() ? MIS_CLIENT_MAIN_TOGGLE_SOUND : _menuState;
                continue;
            case NG_KEY_E:
                _menuState = (*i)->isUp() ? MIS_SERVER_TOGGLE_ENEMIES : _menuState;
                continue;
            case NG_KEY_O:
                _menuState = (*i)->isUp() ? MIS_SERVER_TOGGLE_OBJECTS : _menuState;
                continue;
            case NG_KEY_I:
                _menuState = (*i)->isUp() ? MIS_SERVER_TOGGLE_SERVER_DISPLAY : _menuState;
                continue;
            case NG_KEY_P:
                _menuState = (*i)->isUp() ? MIS_SERVER_TOGGLE_PHYSICS_DISPLAY : _menuState;
                continue;
            case NG_KEY_ESCAPE:
                _menuState = (*i)->isUp() ? MIS_ESCAPE : _menuState;
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
                        _menuState = (*i)->isDown() ? MIS_LOCAL_PLAYER_DROP_OUT_1 : MIS_NONE;
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
                    _menuState = !(*i)->isPressed() ? MIS_LOCAL_PLAYER_DROP_OUT_3 : MIS_NONE;
                }
                else if ((*i)->getIndex() == 2)
                {
                    _currentState->getGameInputState((*i)->getIndex())._playerId = INPUT_UNASSIGNED;
                    _menuState = !(*i)->isPressed() ? MIS_LOCAL_PLAYER_DROP_OUT_2 : MIS_NONE;
                }
                else if ((*i)->getIndex() == 1)
                {
                    _currentState->getGameInputState((*i)->getIndex())._playerId = INPUT_UNASSIGNED;
                    _menuState = !(*i)->isPressed() ? MIS_LOCAL_PLAYER_DROP_OUT_1 : MIS_NONE;
                }
                else
                {
                    _menuState = (*i)->isPressed() ? MIS_ESCAPE : _menuState;
                }
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
                Vector2& vec = CURSOR_CONVERTER->touchToWorld(*(*i));
                _currentState->getGameInputState(0)._isMovingLeft = vec.getX() < (CAM_WIDTH / 2);
                _currentState->getGameInputState(0)._isMovingRight = vec.getX() > (CAM_WIDTH / 2);
                
                if ((*i)->getType() == CursorEventType_DOWN)
                {
                    _currentState->getGameInputState(0)._isJumping = true;
                    _currentState->getGameInputState(0)._isShooting = true;
                }
            }
            else
            {
                _currentState->getGameInputState(0)._isMovingLeft = false;
                _currentState->getGameInputState(0)._isMovingRight = false;
                
                _currentState->getGameInputState(0)._isJumping = false;
                _currentState->getGameInputState(0)._isShooting = false;
                
                continue;
            }
        }
    }
    
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

MainInputState* GameInputManager::getInputState()
{
    return _currentState;
}

MoveList& GameInputManager::getMoveList()
{
    return _moveList;
}

bool GameInputManager::isPlayerIdLocalHost(uint8_t playerId)
{
    return _currentState->isPlayerIdLocalHost(playerId);
}

int GameInputManager::getMenuState()
{
    return _menuState;
}

const Move& GameInputManager::sampleInputAsMove()
{
    MainInputState* inputState = static_cast<MainInputState*>(POOLED_OBJ_MGR->borrowInputState());
    _currentState->copyTo(inputState);
    
    return _moveList.addMove(inputState, Timing::getInstance()->getFrameStartTime());
}

GameInputManager::GameInputManager() :
_currentState(static_cast<MainInputState*>(POOLED_OBJ_MGR->borrowInputState())),
_pendingMove(NULL),
_menuState(MIS_NONE),
_isTimeToProcessInput(false)
{
    // Empty
}
