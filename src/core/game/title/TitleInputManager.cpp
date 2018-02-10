//
//  TitleInputManager.cpp
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/title/TitleInputManager.h>

#include <game/title/TitleEngine.h>

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

#include <sstream>

TitleInputManager* TitleInputManager::getInstance()
{
    static TitleInputManager instance = TitleInputManager();
    return &instance;
}

void TitleInputManager::setEngine(TitleEngine* inValue)
{
    _engine = inValue;
}

void TitleInputManager::update()
{
    CURSOR_INPUT_MANAGER->process();
    KEYBOARD_INPUT_MANAGER->process();
    GAME_PAD_INPUT_MANAGER->process();
    
    _inputState = TIMS_NONE;
    
    if (_isLiveMode)
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
                        char key = charForKey(keyboardEvent->getKey());
                        ss << StringUtil::format("%c", key);
                    }
                }
                else if (keyboardEvent->isUp())
                {
                    if (keyboardEvent->getKey() == NG_KEY_ESCAPE)
                    {
                        _inputState = TIMS_ESCAPE;
                        return;
                    }
                }
            }
        }
        
        if (PlatformHelper::getPlatform() == NG_PLATFORM_ANDROID
            || PlatformHelper::getPlatform() == NG_PLATFORM_IOS)
        {
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
        }
        
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
            KeyboardEvent& e = *(*i);
            
            switch (e.getKey())
            {
                case NG_KEY_E:
                    _inputState = e.isDown() ? TIMS_ENTER_STUDIO : TIMS_NONE;
                    continue;
                case NG_KEY_A:
                    _inputState = e.isDown() ? TIMS_ACTIVATE_STEAM : TIMS_NONE;
                    continue;
                case NG_KEY_D:
                    _inputState = e.isDown() ? TIMS_DEACTIVATE_STEAM : TIMS_NONE;
                    continue;
                case NG_KEY_S:
                    _inputState = e.isDown() ? TIMS_START_SERVER : TIMS_NONE;
                    continue;
                case NG_KEY_J:
                    _inputState = e.isDown() ? TIMS_JOIN_LOCAL_SERVER : TIMS_NONE;
                    continue;
                case NG_KEY_L:
                    _inputState = e.isDown() ? TIMS_STEAM_REFRESH_LAN_SERVERS : TIMS_NONE;
                    continue;
                case NG_KEY_I:
                    _inputState = e.isDown() ? TIMS_STEAM_REFRESH_INTERNET_SERVERS : TIMS_NONE;
                    continue;
                case NG_KEY_ONE:
                    _inputState = e.isDown() ? TIMS_STEAM_JOIN_SERVER_1 : TIMS_NONE;
                    continue;
                case NG_KEY_TWO:
                    _inputState = e.isDown() ? TIMS_STEAM_JOIN_SERVER_2 : TIMS_NONE;
                    continue;
                case NG_KEY_THREE:
                    _inputState = e.isDown() ? TIMS_STEAM_JOIN_SERVER_3 : TIMS_NONE;
                    continue;
                case NG_KEY_FOUR:
                    _inputState = e.isDown() ? TIMS_STEAM_JOIN_SERVER_4 : TIMS_NONE;
                    continue;
                case NG_KEY_ESCAPE:
                    _inputState = e.isDown() ? TIMS_ESCAPE : TIMS_NONE;
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
                    _inputState = TIMS_START_SERVER;
                    continue;
                case GamePadEventType_BACK_BUTTON:
                    _inputState = TIMS_ESCAPE;
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
                if ((*i)->getType() == CursorEventType_UP)
                {
                    _inputState = TIMS_START_SERVER;
                }
                else
                {
                    continue;
                }
            }
        }
    }
}

void TitleInputManager::setLiveInputMode(bool isLiveMode)
{
    _isLiveMode = isLiveMode;
    
    if (_isLiveMode)
    {
        _liveInput.clear();
    }
}

bool TitleInputManager::isLiveMode()
{
    return _isLiveMode;
}

bool TitleInputManager::isTimeToProcessInput()
{
    return _isTimeToProcessInput;
}

void TitleInputManager::onInputProcessed()
{
    _liveInput.clear();
    _isTimeToProcessInput = false;
}

int TitleInputManager::getMenuState()
{
    return _inputState;
}

std::string& TitleInputManager::getLiveInputRef()
{
    return _liveInput;
}

std::string TitleInputManager::getLiveInput()
{
    return _liveInput;
}

TitleInputManager::TitleInputManager() :
_inputState(TIMS_NONE),
_isLiveMode(false),
_isTimeToProcessInput(false),
_engine(NULL)
{
    // Empty
}
