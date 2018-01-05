//
//  StudioInputManager.cpp
//  dante
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/logic/StudioInputManager.h"

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

#include <sstream>

StudioInputManager* StudioInputManager::getInstance()
{
    static StudioInputManager instance = StudioInputManager();
    return &instance;
}

void StudioInputManager::update()
{
    CURSOR_INPUT_MANAGER->process();
    KEYBOARD_INPUT_MANAGER->process();
    GAME_PAD_INPUT_MANAGER->process();
    
    _menuState = MIS_NONE;
    
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
                        _menuState = MIS_ESCAPE;
                        return;
                    }
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
            if (!(*i)->isUp())
            {
                continue;
            }
            
            switch ((*i)->getKey())
            {
                case NG_KEY_ESCAPE:
                    _menuState = MIS_ESCAPE;
                    continue;
                default:
                    continue;
            }
        }
    }
}

void StudioInputManager::setLiveInputMode(bool isLiveMode)
{
    _isLiveMode = isLiveMode;
    
    if (_isLiveMode)
    {
        _liveInput.clear();
    }
}

bool StudioInputManager::isLiveMode()
{
    return _isLiveMode;
}

bool StudioInputManager::isTimeToProcessInput()
{
    return _isTimeToProcessInput;
}

void StudioInputManager::onInputProcessed()
{
    _liveInput.clear();
    _isTimeToProcessInput = false;
}

int StudioInputManager::getMenuState()
{
    return _menuState;
}

std::string& StudioInputManager::getLiveInputRef()
{
    return _liveInput;
}

std::string StudioInputManager::getLiveInput()
{
    return _liveInput;
}

StudioInputManager::StudioInputManager() :
_menuState(MIS_NONE),
_isLiveMode(false),
_isTimeToProcessInput(false)
{
    // Empty
}
